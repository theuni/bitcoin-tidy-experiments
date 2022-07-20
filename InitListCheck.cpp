// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "InitListCheck.h"

#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>


namespace {
} // namespace

namespace bitcoin {

void InitListCheck::registerMatchers(clang::ast_matchers::MatchFinder *finder)
{
    using namespace clang::ast_matchers;
    finder->addMatcher(
     implicitValueInitExpr(
      hasParent(
       initListExpr(has(designatedInitExpr())).bind("initlist"))
      ).bind("implicitval")
    , this);
}

void InitListCheck::check(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
    std::string membertype;
    if (const auto* member = Result.Nodes.getNodeAs<clang::ImplicitValueInitExpr>("implicitval")) {
        membertype = member->getType().getAsString();
    }
    if (const auto* init = Result.Nodes.getNodeAs<clang::InitListExpr>("initlist")) {
        diag(init->getBeginLoc(), "Designated initializer with uninitialized member of type: " + membertype);
    }
}

} // namespace bitcoin
