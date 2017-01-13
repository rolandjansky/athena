// testing check_discarded_const_in_funcall

#pragma ATLAS check_thread_safety

const int* xx();
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

void f7 [[gnu::argument_not_const_thread_safe]] (const int* y)
{
  f1(const_cast<int*>(y));
}

void f7a [[gnu::not_const_thread_safe]] (const int* y)
{
  f1(const_cast<int*>(y));
}

void f8 [[gnu::not_const_thread_safe]] ()
{
  const int* y = xx();
  f1(const_cast<int*>(y));
}

void f8a [[gnu::argument_not_const_thread_safe]] ()
{
  const int* y = xx();
  f1(const_cast<int*>(y));
}

void f9 [[gnu::not_thread_safe]] ()
{
  const int* y = xx();
  f1(const_cast<int*>(y));
}
