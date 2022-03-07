// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "EarlyExitTidyModule.h"

#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

namespace bitcoin {

  void PropagateEarlyExitCheck::registerMatchers(clang::ast_matchers::MatchFinder *Finder) {
  using namespace clang::ast_matchers;

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
          hasName("early_exit_t"))))))
     ).bind("func_should_early_exit"), this);

  }

  void PropagateEarlyExitCheck::check(const clang::ast_matchers::MatchFinder::MatchResult &Result) {

    const auto *decl = Result.Nodes.getNodeAs<clang::FunctionDecl>("func_should_early_exit");
    if (!decl) {
        return;
    }
    clang::SourceRange return_range = decl->getReturnTypeSourceRange();
    const auto* canon_decl = decl->getCanonicalDecl();

    if (return_range.isInvalid()) {
        // Happens (at least) with trailing return types
        // See: https://bugs.llvm.org/show_bug.cgi?id=39567
        diag(decl->getLocation(), "unknown return type");
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

    auto user_diag = diag(loc, "%0 should return early_exit_t.") << decl;
    user_diag << clang::FixItHint::CreateReplacement(return_range, (llvm::Twine("early_exit_t<") + retstring + ">").str());

    //TODO: This is very naive. Need to replace all occurances, not just canonical.
    if (canon_decl != decl) {
        clang::SourceRange canon_return_range = canon_decl->getReturnTypeSourceRange();
        user_diag << clang::FixItHint::CreateReplacement(canon_return_range, (llvm::Twine("early_exit_t<") + retstring + ">").str());
    }

  }
} // namespace
