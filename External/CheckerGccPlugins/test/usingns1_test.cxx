namespace std { int qq [[gnu::thread_safe]]; }
using namespace std;
using std::qq;
#include "usingns1_test.h"


//// Test for a crash.
void
tree_to_shwi()
{
  const char* s __attribute__((unused)) = __FUNCTION__;
}

