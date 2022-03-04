Hacking on a clang-tidy plugin for Bitcoin Core

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
`clang-tidy -checks='-*,bitcoin-*' -load="./libbitcoin-tidy.so" ../example.cc --`

```
1 warning generated.
/home/cory/dev/bitcoin-tidy/build/../example.cc:26:1: warning: 'caller2' should return early_exit_t. [bitcoin-propagate-early-exit]
void caller2() // should warn for not returning early_exit_t.
^~~~
early_exit_t<>
/home/cory/dev/bitcoin-tidy/build/../example.cc:26:1: note: FIX-IT applied suggested code changes
/home/cory/dev/bitcoin-tidy/build/../example.h:8:1: note: FIX-IT applied suggested code changes
void caller2();
^
clang-tidy applied 2 of 2 suggested fixes.
```

### Caveats:

The clang/clang-tidy libs are not ABI safe, so the clang-tidy runtime version
must be the same as the headers used to build the plugin.
