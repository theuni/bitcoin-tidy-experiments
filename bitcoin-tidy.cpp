// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "EarlyExitTidyModule.h"
#include "ExportMainCheck.h"
#include "LogPrintfCheck.h"
#include "NoADLCheck.h"

#include <clang-tidy/ClangTidyModule.h>
#include <clang-tidy/ClangTidyModuleRegistry.h>

class BitcoinModule final : public clang::tidy::ClangTidyModule {
public:
  void addCheckFactories(clang::tidy::ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<bitcoin::PropagateEarlyExitCheck>("bitcoin-propagate-early-exit");
    CheckFactories.registerCheck<bitcoin::LogPrintfCheck>("bitcoin-unterminated-logprintf");
    CheckFactories.registerCheck<bitcoin::NoADLCheck>("bitcoin-adl-use");
    CheckFactories.registerCheck<bitcoin::ExportMainCheck>("bitcoin-export-main");
  }
};

static clang::tidy::ClangTidyModuleRegistry::Add<BitcoinModule>
    X("bitcoin-module", "Adds bitcoin checks.");

volatile int PropagateEarlyExitCheckAnchorSource = 0;
