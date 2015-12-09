// testing mutable checks

#pragma ATLAS thread_safe


struct B
{
  int z;
};


struct S
{
  mutable int x;
  mutable B b;
  mutable B bb[5];
  mutable int w [[gnu::thread_safe]];
  void f1(int y) const;
  void f6(int y) const;
  void f9(int y) const;
  void f12(int y);
};


void S::f1(int y) const
{
  x = y;
}


void f2(const S* s)
{
  s->x = 10;
}


void f3(int* x);
void f4(const S* s)
{
  f3(&s->x);
}

int* f5(const S* s)
{
  return &s->x;
}


void S::f6(int y) const
{
  b.z = y;
}

int* f7(const S* s)
{
  return &s->b.z;
}

int* f8(const S* s)
{
  return &s->bb[2].z;
}


void S::f9(int y) const
{
  bb[0].z = y;
}


void f10(const S* s)
{
  s->w = 10;
}


int* f11(const S* s)
{
  return &s->w;
}


void S::f12(int y)
{
  x = y;
}


void f14(S* s)
{
  s->x = 10;
}


