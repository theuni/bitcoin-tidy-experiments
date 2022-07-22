// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef EXPORT_MAIN_CHECK_H
#define EXPORT_MAIN_CHECK_H

#include <clang-tidy/ClangTidyCheck.h>

namespace bitcoin {

class ExportMainCheck final : public clang::tidy::ClangTidyCheck {

public:
  ExportMainCheck(clang::StringRef Name, clang::tidy::ClangTidyContext *Context)
      : clang::tidy::ClangTidyCheck(Name, Context) {}

  bool isLanguageVersionSupported(const clang::LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus;
  }
  void registerMatchers(clang::ast_matchers::MatchFinder *Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace bitcoin

#endif // EXPORT_MAIN_CHECK_H
