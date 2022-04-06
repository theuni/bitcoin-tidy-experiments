// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "EarlyExitTidyModule.h"

#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

namespace bitcoin {

  void PropagateEarlyExitCheck::registerMatchers(clang::ast_matchers::MatchFinder *Finder) {
  using namespace clang::ast_matchers;

    auto matchtype = qualType(hasDeclaration(classTemplateSpecializationDecl(hasName("MaybeEarlyExit"))));
    Finder->addMatcher(
     callExpr(
       hasType(matchtype),
       forCallable(functionDecl(
         unless(isMain()),
         unless(returns(matchtype)),
         optionally(hasBody(compoundStmt(unless(hasAnySubstatement(returnStmt()))).bind("stmt_needs_return"))),
         optionally(hasDescendant(returnStmt(unless(has(expr()))).bind("naked_return")))
       ).bind("func_should_early_exit"))
     ).bind("early_exit_call")
    , this);
  }

  void PropagateEarlyExitCheck::check(const clang::ast_matchers::MatchFinder::MatchResult &Result) {
    static llvm::SmallSet<int64_t, 8> g_decls;

    const auto& sm = *Result.SourceManager;
    if (const auto *decl = Result.Nodes.getNodeAs<clang::FunctionDecl>("func_should_early_exit")) {
        if(!g_decls.insert(decl->getID()).second) {
            return;
        }
        auto user_diag = diag(decl->getBeginLoc(), "%0 should return MaybeEarlyExit.") << decl;
        recursiveChangeType(decl, user_diag);
    }
    if (const auto *body = Result.Nodes.getNodeAs<clang::Stmt>("stmt_needs_return"))
    {
        auto user_diag = diag(body->getEndLoc(), " now needs return statement.");
        addReturn(body, user_diag, sm);
    }
    if (const auto* stmt = Result.Nodes.getNodeAs<clang::ReturnStmt>("naked_return"))
    {
        auto user_diag = diag(stmt->getBeginLoc(), "Should return something.");
        updateReturn(stmt, user_diag);
    }
    if (const auto* expr = Result.Nodes.getNodeAs<clang::CallExpr>("early_exit_call"))
    {
        // TODO: Maybe filter more?
        // TODO: "if (early_exit_call() == foo)"    -> "if (*early_exit_call() == foo)"
        // TODO: "auto foo = early_exit_call().bar" -> "auto foo = early_exit_call()->bar"
    }
  }

  void PropagateEarlyExitCheck::recursiveChangeType(const clang::FunctionDecl* decl, clang::DiagnosticBuilder& user_diag)
  {
    const auto& return_loc = decl->getTypeSourceInfo()->getTypeLoc().getAs<clang::FunctionTypeLoc>().getReturnLoc();
    clang::SourceRange return_range{return_loc.getBeginLoc(), return_loc.getEndLoc()};

    const auto* canon_decl = decl->getCanonicalDecl();

    if (return_range.isInvalid()) {
        return;
    }

    const auto& ctx = decl->getASTContext();
    const auto& opts = ctx.getLangOpts();
    clang::PrintingPolicy Policy(opts);
    auto rettype = decl->getDeclaredReturnType();

    std::string retstring;
    if (!rettype->isVoidType()) {
        retstring = rettype.getAsString(Policy);
    }

    auto loc = decl->getBeginLoc();
    if (loc.isMacroID()) {
        return;
    }

    user_diag << clang::FixItHint::CreateReplacement(return_range, (llvm::Twine("MaybeEarlyExit<") + retstring + ">").str());
    //TODO: This is very naive. Need to replace all occurances, not just canonical.
    if (canon_decl != decl) {
        clang::SourceRange canon_return_range = canon_decl->getReturnTypeSourceRange();
        user_diag << clang::FixItHint::CreateReplacement(canon_return_range, (llvm::Twine("MaybeEarlyExit<") + retstring + ">").str());
    }
    return;
  }

  void PropagateEarlyExitCheck::addReturn(const clang::Stmt* body, clang::DiagnosticBuilder& user_diag, const clang::SourceManager &sm)
  {
    const clang::Stmt* laststmt;
    // Attempt to align the return to the column of the previous statement
    for (auto child = body->child_begin(); child != body->child_end(); child++)
    {
        laststmt = *child;
    }
    const auto& prevbegin = laststmt->getBeginLoc();
    auto indentlevel = sm.getPresumedColumnNumber(prevbegin) - 1;

    user_diag << clang::FixItHint::CreateInsertion(body->getEndLoc(), std::string(indentlevel, ' ') + "return {};\n");
  }

  void PropagateEarlyExitCheck::updateReturn(const clang::ReturnStmt* stmt, clang::DiagnosticBuilder& user_diag)
  {
    user_diag << clang::FixItHint::CreateReplacement(stmt->getReturnLoc(), "return {}");
  }

} // namespace
