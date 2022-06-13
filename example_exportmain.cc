#if TEST_DECLSPEC
#define EXPORT_ATTR __declspec(dllexport)
#elif TEST_VISIBLE
#define EXPORT_ATTR __attribute__ ((visibility ("default")))
#else
#define EXPORT_ATTR
#endif

EXPORT_ATTR int main(int argc, char* argv[]){}

