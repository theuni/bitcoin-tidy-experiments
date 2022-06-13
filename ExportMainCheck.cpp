// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "ExportMainCheck.h"

#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/TargetInfo.h>

#include <cstdio>

namespace {
} // namespace

namespace bitcoin {

void ExportMainCheck::registerMatchers(clang::ast_matchers::MatchFinder *finder)
{
    using namespace clang::ast_matchers;
    finder->addMatcher(
      functionDecl(isMain()).bind("mainfunc")
    , this);
}

void ExportMainCheck::check(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
    const auto* lit = Result.Nodes.getNodeAs<clang::FunctionDecl>("mainfunc");
    if (!lit) return;
    const auto& ctx = *Result.Context;

    // This does not seem to work as expected
    // if (!ctx.getTargetInfo().getCXXABI().isMicrosoft()) return;

    // This one does
    if (!ctx.getTargetInfo().getTriple().isOSWindows()) return;

    if (lit->hasAttr<clang::DLLExportAttr>()) return;
    if (lit->hasAttr<clang::VisibilityAttr>()) return;
    const auto user_diag = diag(lit->getBeginLoc(), "Un-exported main function");
}

} // namespace bitcoin
