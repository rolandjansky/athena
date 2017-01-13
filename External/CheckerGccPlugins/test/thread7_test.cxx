// testing check for static/mutable members

#pragma ATLAS check_thread_safety


struct S
{
  S() : a(), b(), c() {}
  mutable int a;
  mutable int b [[gnu::thread_safe]];
  mutable int *c[10];

  static int d;
  static int e [[gnu::thread_safe]];
};


