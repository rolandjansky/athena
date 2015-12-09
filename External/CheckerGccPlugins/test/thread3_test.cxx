// testing check_pass_static_by_call.

#pragma ATLAS thread_safe


static int y1;
static int y2 [[gnu::thread_safe]];
static const int y3 = 10;


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
