// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "LogPrintfCheck.h"

#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>


namespace {
AST_MATCHER(clang::StringLiteral, unterminated) {
    size_t len = Node.getLength();
    if(len == 0) {
        return false;
    }
    if(Node.getCodeUnit(len-1) == '\n') {
        return false;
    }
    if (len > 2 &&
        Node.getCodeUnit(len-1) == '.' &&
        Node.getCodeUnit(len-2) == '.' &&
        Node.getCodeUnit(len-3) == '.')
    {
        return false;
    }
    return true;
}
} // namespace

namespace bitcoin {

void LogPrintfCheck::registerMatchers(clang::ast_matchers::MatchFinder *finder)
{
    using namespace clang::ast_matchers;
    finder->addMatcher(
      callExpr(
        callee(functionDecl(hasName("LogPrintf_"))),
        hasArgument(5, stringLiteral(unterminated()).bind("logstring"))
      ).bind("logprintf"),
    this);
}

void LogPrintfCheck::check(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
    if (const auto* lit = Result.Nodes.getNodeAs<clang::StringLiteral>("logstring")) {
        const auto& ctx = *Result.Context;
        const auto user_diag = diag(lit->getEndLoc(), "Unterminated LogPrintf");
        auto len = lit->getByteLength();
        const auto& loc = lit->getLocationOfByte(len, *Result.SourceManager, ctx.getLangOpts(), ctx.getTargetInfo());
        user_diag << clang::FixItHint::CreateInsertion(loc, "\\n");
    }
}
}
