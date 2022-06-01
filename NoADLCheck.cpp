// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "NoADLCheck.h"

#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>


namespace {
} // namespace

namespace bitcoin {

void NoADLCheck::registerMatchers(clang::ast_matchers::MatchFinder *finder)
{
    using namespace clang::ast_matchers;
    finder->addMatcher(
      callExpr(usesADL()).bind("adlexpr")
    , this);
}

void NoADLCheck::check(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
    if (const auto* lit = Result.Nodes.getNodeAs<clang::CallExpr>("adlexpr")) {
        const auto& ctx = *Result.Context;
        diag(lit->getEndLoc(), "Use of ADL");
    }
}

} // namespace bitcoin
