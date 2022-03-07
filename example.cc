// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// Warn about any function that calls into another function that returns
// an early_exit_t. The intention is to force _any_ returned early_exit_t to
// bubble all the way back up to main. Like manual exception catch/rethrowing.

#include "example.h"

early_exit_t<int> maybe_early_exit()
{
    return {};
}

early_exit_t<> caller()
{
    auto foo = maybe_early_exit();
    auto bar = maybe_early_exit();
    return foo;
}

void caller2() // should warn for not returning early_exit_t.
{
    auto foo = maybe_early_exit();
    auto bar = maybe_early_exit();
    return; // should rewrite as "return {};".
}

void caller3() // should warn for not returning early_exit_t.
{
    auto foo = maybe_early_exit();
    auto bar = maybe_early_exit();
    // Should add "return {};"
}
