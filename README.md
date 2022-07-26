Hacking on a clang-tidy plugin for Bitcoin Core

This started with the propagate-early-exit check/fix as a way to transform
Core's shutdown code, but a few other checks have been added as well. This is
a catch-all repo for hacking until we decide to do something with these checks.

### Dependencies:
llvm 14+ headers and clang-tidy 14+ are required, but libs are not.

To install them, build llvm with clang-tools-extra enabled, and:
`make install-llvm-headers install-clang-tidy-headers install-clang-headers-stripped`

### Building the plugin:
- mkdir build
- cd build
- cmake ..
- make

### Using the plugin:

Example propagate-early-exit usage:

``clang-tidy --load=`pwd`/libbitcoin-tidy-experiments.so -checks='-*,bitcoin-propagate-early-exit' -fix ../example.cc -- -std=c++17``

```
25 warnings generated.
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:18:5: warning: Adding Macros [bitcoin-propagate-early-exit]
    auto foo = maybe_early_exit();
    ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    EXIT_OR_DECL(auto foo, maybe_early_exit());
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:18:5: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:19:5: warning: Adding Macros [bitcoin-propagate-early-exit]
    auto bar = maybe_early_exit();
    ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    EXIT_OR_DECL(auto bar, maybe_early_exit());
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:19:5: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:25:1: warning: 'caller2' should return MaybeEarlyExit. [bitcoin-propagate-early-exit]
void caller2() // should warn for not returning MaybeEarlyExit.
^~~~
MaybeEarlyExit<>
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:23:1: note: FIX-IT applied suggested code changes
void caller2();
^
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:24:19: note: FIX-IT applied suggested code changes
auto caller2() -> void;
                  ^
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:25:1: note: FIX-IT applied suggested code changes
void caller2() // should warn for not returning MaybeEarlyExit.
^
/home/cory/dev/bitcoin-tidy-experiments/build/../example.h:10:1: note: FIX-IT applied suggested code changes
void caller2();
^
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:27:5: warning: Adding Macros [bitcoin-propagate-early-exit]
    auto foo(maybe_early_exit());
    ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    EXIT_OR_DECL(auto foo, maybe_early_exit());
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:27:5: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:28:5: warning: Adding Macros [bitcoin-propagate-early-exit]
    const auto& bar = maybe_early_exit();
    ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    EXIT_OR_DECL(const auto& bar, maybe_early_exit());
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:28:5: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:30:5: warning: Adding Macros [bitcoin-propagate-early-exit]
    castret = maybe_early_exit();
    ^       ~
    EXIT_OR_ASSIGN( ,          )
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:30:5: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:30:13: note: FIX-IT applied suggested code changes
    castret = maybe_early_exit();
            ^
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:30:32: note: FIX-IT applied suggested code changes
    castret = maybe_early_exit();
                               ^
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:31:5: warning: Should return something. [bitcoin-propagate-early-exit]
    return; // should rewrite as "return {};".
    ^~~~~~
    return {}
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:31:5: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:34:1: warning: 'caller3' should return MaybeEarlyExit. [bitcoin-propagate-early-exit]
void caller3() // should warn for not returning MaybeEarlyExit.
^~~~
MaybeEarlyExit<>
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:34:1: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:36:5: warning: Adding Macros [bitcoin-propagate-early-exit]
    auto foo = maybe_early_exit();
    ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    EXIT_OR_DECL(auto foo, maybe_early_exit());
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:36:5: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:37:5: warning: Adding Macros [bitcoin-propagate-early-exit]
    if (maybe_early_exit()) {
    ^~~~
    EXIT_OR_IF(
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:37:5: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:38:9: warning: Should return something. [bitcoin-propagate-early-exit]
        return; // should rewrite as "return {};"
        ^~~~~~
        return {}
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:38:9: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:40:5: warning: Adding Macros [bitcoin-propagate-early-exit]
    if (!maybe_early_exit()) {
    ^~~~
    EXIT_OR_IF_NOT(
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:40:5: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:40:9: warning: Adding Macros [bitcoin-propagate-early-exit]
    if (!maybe_early_exit()) {
        ^
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:40:9: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:41:9: warning: Should return something. [bitcoin-propagate-early-exit]
        return; // should rewrite as "return {};"
        ^~~~~~
        return {}
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:41:9: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:47:1: warning:  now needs return statement. [bitcoin-propagate-early-exit]
}
^
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:47:1: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:54:1: warning: 'caller4' should return MaybeEarlyExit. [bitcoin-propagate-early-exit]
void caller4()
^~~~
MaybeEarlyExit<>
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:54:1: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:58:5: warning: Adding Macros [bitcoin-propagate-early-exit]
    const auto& foo = fpointer(1);
    ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    EXIT_OR_DECL(const auto& foo, fpointer(1));
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:58:5: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:59:1: warning:  now needs return statement. [bitcoin-propagate-early-exit]
}
^
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:59:1: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:60:1: warning: 'caller5' should return MaybeEarlyExit. [bitcoin-propagate-early-exit]
void caller5()
^~~~
MaybeEarlyExit<>
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:60:1: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:62:5: warning: Adding Macros [bitcoin-propagate-early-exit]
    maybe_early_exit();
    ^
    MAYBE_EXIT(      )
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:62:5: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:62:22: note: FIX-IT applied suggested code changes
    maybe_early_exit();
                     ^
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:63:1: warning:  now needs return statement. [bitcoin-propagate-early-exit]
}
^
/home/cory/dev/bitcoin-tidy-experiments/build/../example.cc:63:1: note: FIX-IT applied suggested code changes
clang-tidy applied 27 of 27 suggested fixes.
Suppressed 4 warnings (4 with check filters).
```

### Caveats:

The clang/clang-tidy libs are not ABI safe, so the clang-tidy runtime version
must be the same as the headers used to build the plugin.
