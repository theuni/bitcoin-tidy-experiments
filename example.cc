// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// Warn about any function that calls into another function that returns
// an MaybeEarlyExit. The intention is to force _any_ returned MaybeEarlyExit to
// bubble all the way back up to main. Like manual exception catch/rethrowing.

#include "example.h"

MaybeEarlyExit<int> maybe_early_exit()
{
    return {};
}

MaybeEarlyExit<> caller()
{
    auto foo = maybe_early_exit();
    auto bar = maybe_early_exit();
    return foo;
}

void caller2() // should warn for not returning MaybeEarlyExit.
{
    auto foo = maybe_early_exit();
    auto bar = maybe_early_exit();
    return; // should rewrite as "return {};".
}

void caller3() // should warn for not returning MaybeEarlyExit.
{
    auto foo = maybe_early_exit();
    auto bar = maybe_early_exit();
    if (1) {
        return; // should rewrite as "return {};"
    }
    // Should add "return {};"
}
