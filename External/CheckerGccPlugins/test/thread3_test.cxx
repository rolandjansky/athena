// testing check_pass_static_by_call.

#pragma ATLAS check_thread_safety


static int y1;
static int y2 [[gnu::thread_safe]];
static const int y3 = 10;
static thread_local int y4;


void foo1(int, int*);
void foo2(int, int&);
void foo3(int, const int*);


void f1()
{
  foo1(3, &y1);
  foo2(3, y1);
  foo3(3, &y1);
  foo3(3, &y2);
  foo3(3, &y3);
  foo3(3, &y4);
}


class C
{
public:
  void foo();
};


static C c1;


void f2()
{
  c1.foo();
}


void f3 [[gnu::not_reentrant]] ()
{
  foo1(3, &y1);
}


void f4 [[gnu::not_thread_safe]] ()
{
  foo1(3, &y1);
}


