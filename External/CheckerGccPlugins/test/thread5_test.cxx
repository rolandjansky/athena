// testing const_cast check from check_pass_static_by_call

#pragma ATLAS thread_safe

void f1(int* y);
void f2(const int* y)
{
  f1(const_cast<int*>(y));
}

void f3(const int y[10])
{
  f1((int*)&y[5]);
}


struct S
{
  int x;
  int foo();
};

void f4(const S* s)
{
  f1((int*)&s->x);
}


void f5(const int* y)
{
  int* yy [[gnu::thread_safe]] = const_cast<int*>(y);
  f1(yy);
}

void f6(const S& s)
{
  const_cast<S&>(s).foo();
}
