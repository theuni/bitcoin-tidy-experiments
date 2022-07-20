// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef INITLIST_CHECK_H
#define INITLIST_CHECK_H

#include <clang-tidy/ClangTidyCheck.h>

namespace bitcoin {

class InitListCheck final : public clang::tidy::ClangTidyCheck {

public:
  InitListCheck(clang::StringRef Name, clang::tidy::ClangTidyContext *Context)
      : clang::tidy::ClangTidyCheck(Name, Context) {}

  bool isLanguageVersionSupported(const clang::LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus;
  }
  void registerMatchers(clang::ast_matchers::MatchFinder *Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace bitcoin

#endif // NOADL_CHECK_H
