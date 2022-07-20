struct A {
    struct Options {
        unsigned cache_size;
        signed delta;
        bool in_memory = false;
    };
    A(const Options&) {} // unsafe (may have fields uninitialized)
    A(unsigned cache_size, signed delta, bool in_memory = false) {}  // safe, all non-default args are initialized
};

int main() {
    (void)A{
        A::Options{
            .delta = -1,
        },
    };
}
