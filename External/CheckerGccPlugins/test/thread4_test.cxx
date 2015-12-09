// testing const_cast check from check_assign_address_of_static

#pragma ATLAS thread_safe

int* f1(const int* y)
{
  return const_cast<int*>(y);
}


int* f2(const int* y)
{
  return (int*)(y);
}


int* f3(const int y[10])
{
  return (int*)(&y[5]);
}

struct S
{
  int x;
  const int y;
};

int* f4(const S& s)
{
  return &const_cast<int&>(s.x);
}

int* f5(S* s)
{
  return (int*)&s->y;
}


int* f6(const int* y)
{
  int* yy [[gnu::thread_safe]] = (int*)y;
  return yy;
}

