// testing check for static/mutable members

#pragma ATLAS thread_safe


struct S
{
  S() : a(), b(), c() {}
  mutable int a;
  mutable int b [[gnu::thread_safe]];
  mutable int *c[10];

  static int d;
  static int e [[gnu::thread_safe]];
};


