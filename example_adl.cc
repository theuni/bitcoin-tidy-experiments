  namespace NS {
    struct X {};
    void y(X);
  }

  void y(...);

  void test() {
    NS::X x;
    y(x); // Matches
    NS::y(x); // Doesn't match
    y(42); // Doesn't match
    using NS::y;
    y(x); // Found by both unqualified lookup and ADL, doesn't match
   }
