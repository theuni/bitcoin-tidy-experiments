// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef EARLY_EXIT_TIDY_MODULE_H
#define EARLY_EXIT_TIDY_MODULE_H

#include <clang-tidy/ClangTidyCheck.h>

namespace bitcoin {

class PropagateEarlyExitCheck final : public clang::tidy::ClangTidyCheck {

public:
  PropagateEarlyExitCheck(clang::StringRef Name, clang::tidy::ClangTidyContext *Context)
      : clang::tidy::ClangTidyCheck(Name, Context) {}

  bool isLanguageVersionSupported(const clang::LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus;
  }
  void registerMatchers(clang::ast_matchers::MatchFinder *Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
private:
  void recursiveChangeType(const clang::FunctionDecl*, clang::DiagnosticBuilder&);
  void addReturn(const clang::FunctionDecl*, clang::DiagnosticBuilder&);
  void updateReturn(const clang::ReturnStmt*, clang::DiagnosticBuilder&);
};

} // namespace bitcoin

#endif // EARLY_EXIT_TIDY_MODULE_H
