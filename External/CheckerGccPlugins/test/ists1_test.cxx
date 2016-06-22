#include "ists1_test1.h"
#include "Control/p1/p1/p1a.h"
#include "Control/p1/p1/p1b.h"
#include "Control/p2/p2/p2a.h"
#include "Control/p2/p2/p2b.h"

void f1 [[gnu::thread_safe_debug]] () {}
void f2 [[gnu::thread_safe_debug]] [[gnu::thread_safe]] () {}


struct [[gnu::thread_safe_debug]] C1
{
  void g1 [[gnu::thread_safe_debug]] () {}
  void g2 [[gnu::thread_safe_debug]] ();
  void g3();
  void g4 [[gnu::thread_safe]] [[gnu::thread_safe_debug]] ();
};


void C1::g2() {}
void C1::g3 [[gnu::thread_safe_debug]] () {}
void C1::g4() {}


struct [[gnu::thread_safe]] C2
{
  void h1 [[gnu::thread_safe_debug]] () {}
  void h2();

  struct C3 {
    void h3();
  };
};

void C2::h2 [[gnu::thread_safe_debug]] () {}
void C2::C3::h3 [[gnu::thread_safe_debug]] () {}
