// testing check_assign_address_of_static

#pragma ATLAS thread_safe


static int y1;
static const int y2 = 0;
//static int y3[10];


int* f1()
{
  return &y1;
}


const int* f2()
{
  return &y1;
}


int& f3()
{
  return y1;
}


const int& f4()
{
  return y1;
}


#if 0
int f5()
{
  // warning is different btn 4.9 and 5.
  int* p = &(y3[0]) + 3;
  return *p;
}


int f6()
{
  // warning is different btn 4.9 and 5.
  const int* p = &(y3[0]) + 3;
  return *p;
}
#endif


const int* f7()
{
  return &y2;
}


struct Y
{
  int y1;
  int y2;
  void foo();
};

