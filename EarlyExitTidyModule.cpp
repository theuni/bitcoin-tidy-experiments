// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "EarlyExitTidyModule.h"

#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

namespace bitcoin {

  void PropagateEarlyExitCheck::registerMatchers(clang::ast_matchers::MatchFinder *Finder) {
  using namespace clang::ast_matchers;


    // Match functions which call into at least one function return
    // early_exit_t but which do not themselves return early_exit_t.
    Finder->addMatcher(
     functionDecl(
      hasDescendant(
       callExpr(
        callee(
         functionDecl(
          returns(
           qualType(
            hasDeclaration(
             classTemplateSpecializationDecl(
              hasName("early_exit_t"))))))))),
     unless(
      returns(
       qualType(
        hasDeclaration(
         classTemplateSpecializationDecl(
          hasName("early_exit_t")))))),
     hasDescendant(
      returnStmt())
     ).bind("func_should_early_exit"), this);


    // Match bare "return;" statements in a functions that should now
    // return early_exit_t.
    Finder->addMatcher(
     returnStmt(
      hasAncestor(
       functionDecl(
        hasDescendant(
         callExpr(
          callee(
           functionDecl(
            returns(
             qualType(
              hasDeclaration(
               classTemplateSpecializationDecl(
                hasName("early_exit_t"))))))))))),
      unless(
       has(
        expr()))).bind("naked_return"), this);

    // Match for functions with no return statement that now need to return
    // early_exit_t.
    Finder->addMatcher(
     functionDecl(
      hasDescendant(
       callExpr(callee(
        functionDecl(
         returns(
          qualType(
           hasDeclaration(
            classTemplateSpecializationDecl(
             hasName("early_exit_t"))))))))),
      unless(
       returns(
        qualType(
         hasDeclaration(
          classTemplateSpecializationDecl(
           hasName("early_exit_t")))))),
      unless(
       hasDescendant(
        returnStmt()))
     ).bind("func_should_early_exit_noreturn"), this);


  }


  void PropagateEarlyExitCheck::check(const clang::ast_matchers::MatchFinder::MatchResult &Result) {
    if (const auto *decl = Result.Nodes.getNodeAs<clang::FunctionDecl>("func_should_early_exit")) {
        auto user_diag = diag(decl->getBeginLoc(), "%0 should return early_exit_t.") << decl;
        recursiveChangeType(decl, user_diag);
    }
    if (const auto *decl = Result.Nodes.getNodeAs<clang::FunctionDecl>("func_should_early_exit_noreturn"))
    {
        auto user_diag = diag(decl->getBeginLoc(), "%0 should return early_exit_t.") << decl;
        recursiveChangeType(decl, user_diag);
        addReturn(decl, user_diag);
    }
    if (const auto* stmt = Result.Nodes.getNodeAs<clang::ReturnStmt>("naked_return"))
    {
        auto user_diag = diag(stmt->getBeginLoc(), "Should return something.");
        updateReturn(stmt, user_diag);
    }
  }

  void PropagateEarlyExitCheck::recursiveChangeType(const clang::FunctionDecl* decl, clang::DiagnosticBuilder& user_diag)
  {
    clang::SourceRange return_range = decl->getReturnTypeSourceRange();
    const auto* canon_decl = decl->getCanonicalDecl();

    if (return_range.isInvalid()) {
        // Happens (at least) with trailing return types
        // See: https://bugs.llvm.org/show_bug.cgi?id=39567
        return;
    }

    auto rettype = decl->getReturnType();
    std::string retstring;
    if (!rettype->isVoidType()) {
        retstring = rettype.getAsString();
    }

    auto loc = decl->getBeginLoc();
    if (loc.isMacroID()) {
        return;
    }

    user_diag << clang::FixItHint::CreateReplacement(return_range, (llvm::Twine("early_exit_t<") + retstring + ">").str());

    //TODO: This is very naive. Need to replace all occurances, not just canonical.
    if (canon_decl != decl) {
        clang::SourceRange canon_return_range = canon_decl->getReturnTypeSourceRange();
        user_diag << clang::FixItHint::CreateReplacement(canon_return_range, (llvm::Twine("early_exit_t<") + retstring + ">").str());
    }
    return;
  }

  void PropagateEarlyExitCheck::addReturn(const clang::FunctionDecl* decl, clang::DiagnosticBuilder& user_diag)
  {
    const auto* body = decl->getBody();
    user_diag << clang::FixItHint::CreateInsertion(body->getEndLoc(), "return {};\n");
  }

  void PropagateEarlyExitCheck::updateReturn(const clang::ReturnStmt* stmt, clang::DiagnosticBuilder& user_diag)
  {
    user_diag << clang::FixItHint::CreateReplacement(stmt->getReturnLoc(), "return {}");
  }

} // namespace
