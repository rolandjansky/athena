/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: tread_noconv_test.cxx,v 1.8 2009-01-21 04:34:40 ssnyder Exp $
/**
 * @file tread_noconv_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief TConvertingBranchElement regression tests.
 *
 * This reads back the data that twrite_test.cxx wrote, using
 * the same schema (so no conversion is done).
 */


#undef NDEBUG
#include "TTree.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "tcls1.h"
#include <iostream>
#include <cassert>

void dep_dummy()
{
  new TTree;
}

void test1 (TFile& f)
{
  std::cout << "test1\n";
  TTree* t = (TTree*)f.Get("tree");
  T1* c = new T1;
  TBranch* b = t->GetBranch ("T1");
  b->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    b->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->a == i);
    assert (c->b == i*2);
    assert (c->c == i*3);
  }
}


void test2 (TFile& f)
{
  std::cout << "test2\n";
  TTree* t = (TTree*)f.Get("tree2");
  A* a = new A;
  TBranch* b = t->GetBranch ("A");
  b->SetAddress (&a);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    b->GetEntry(i);
    std::cout << *a << "\n";
    assert (a->a == i);
    assert (a->t.a == i*2);
    assert (a->t.b == i*3);
    assert (a->t.c == i*4);
  }
}


void test3 (TFile& f)
{
  std::cout << "test3\n";
  TTree* t = (TTree*)f.Get("tree3");
  B* b = new B;
  TBranch* br = t->GetBranch ("B");
  br->SetAddress (&b);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *b << "\n";
    assert (b->d == i);
    assert (b->a == i*2);
    assert (b->b == i*3);
    assert (b->c == i*4);
  }
}


void test4 (TFile& f)
{
  std::cout << "test4\n";
  TTree* t = (TTree*)f.Get("tree4");
  C* c = new C;
  TBranch* br = t->GetBranch ("C");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->cc == i);
    assert (c->u.e == i*2);
    assert (c->u.t.a == i*3);
    assert (c->u.t.b == i*4);
    assert (c->u.t.c == i*5);
  }
}


void test5 (TFile& f)
{
  std::cout << "test5\n";
  TTree* t = (TTree*)f.Get("tree5");
  E* c = new E;
  TBranch* br = t->GetBranch ("E");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->g == i);
    assert (c->f == i*2);
    assert (c->h == i*3);
  }
}


void test6 (TFile& f)
{
  std::cout << "test6\n";
  TTree* t = (TTree*)f.Get("tree6");
  FX* c = new FX;
  TBranch* br = t->GetBranch ("FX");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->g == i);
    assert (c->a == i*2);
    assert (c->b == i*3);
    assert (c->c == i*4);
    assert (c->j == i*5);
  }
}


void test7 (TFile& f)
{
  std::cout << "test7\n";
  TTree* t = (TTree*)f.Get("tree7");
  G* c = new G;
  TBranch* br = t->GetBranch ("G");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->a == i);
    assert (c->b == i*2);
    assert (c->c == i*3);
    assert (c->k == i*4);
    assert (c->t.a == i*5);
    assert (c->t.b == i*6);
    assert (c->t.c == i*7);
  }
}


void test8 (TFile& f)
{
  std::cout << "test8\n";
  TTree* t = (TTree*)f.Get("tree8");
  I* c = new I;
  TBranch* br = t->GetBranch ("I");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->n == i);
    assert (c->i.m == i*2);
    assert (c->i.t4.l == i*3);
    assert (c->i.t4.d.g == i*4);
  }
}


void test9 (TFile& f)
{
  std::cout << "test9\n";
  TTree* t = (TTree*)f.Get("tree9");
  K* c = new K;
  TBranch* br = t->GetBranch ("K");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->ik == i);
    assert (c->j.ij == i*2);
    assert (c->j.a.a == i*3);
    assert (c->j.a.t.a == i*4);
    assert (c->j.a.t.b == i*5);
    assert (c->j.a.t.c == i*6);
  }
}


void test10 (TFile& f)
{
  std::cout << "test10\n";
  TTree* t = (TTree*)f.Get("tree10");
  L* c = new L;
  TBranch* br = t->GetBranch ("L");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->il == i);
    assert (c->t5.i5 == i*2);
    assert (c->t5.a.a == i*3);
    assert (c->t5.a.t.a == i*4);
    assert (c->t5.a.t.b == i*5);
    assert (c->t5.a.t.c == i*6);
  }
}


void test11 (TFile& f)
{
  std::cout << "test11\n";
  TTree* t = (TTree*)f.Get("tree11");
  M* c = new M;
  TBranch* br = t->GetBranch ("M");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->im == i);
    assert (c->h == i*2);
    assert (c->f == i*3);
    assert (c->g == i*4);
  }
}


void test12 (TFile& f)
{
  std::cout << "test12\n";
  TTree* t = (TTree*)f.Get("tree12");
  O* c = new O;
  TBranch* br = t->GetBranch ("O");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->io == i);
    assert (c->in == i*2);
    assert (c->d == i*3);
    assert (c->a == i*4);
    assert (c->b == i*5);
    assert (c->c == i*6);
  }
}


void test13 (TFile& f)
{
  std::cout << "test13\n";
  TTree* t = (TTree*)f.Get("tree13");
  P* c = new P;
  TBranch* br = t->GetBranch ("P");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->ip == i);
    assert (c->i6 == i*2);
    assert (c->d == i*3);
    assert (c->a == i*4);
    assert (c->b == i*5);
    assert (c->c == i*6);
  }
}


void test14 (TFile& f)
{
  std::cout << "test14\n";
  TTree* t = (TTree*)f.Get("tree14");
  R* c = new R;
  TBranch* br = t->GetBranch ("R");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->a == i);
    assert (c->t.a == i*2);
    assert (c->t.b == i*3);
    assert (c->t.c == i*4);
  }
}


void test15 (TFile& f)
{
  std::cout << "test15\n";
  TTree* t = (TTree*)f.Get("tree15");
  T8* c = new T8;
  TBranch* br = t->GetBranch ("T8");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->a.a == i);
    assert (c->a.t.a == i*2);
    assert (c->a.t.b == i*3);
    assert (c->a.t.c == i*4);
    assert (c->h.m == i*5);
    assert (c->h.t4.l == i*6);
    assert (c->h.t4.d.g == i*7);
    assert (c->i8 == i*8);
    assert (c->g == i*9);
    assert (c->f == i*10);
    assert (c->i7 == i*11);
    assert (c->gg == i*12);
    assert (c->t1.a == i*13);
    assert (c->t1.b == i*14);
    assert (c->t1.c == i*15);
  }
}


void test16 (TFile& f)
{
  std::cout << "test16\n";
  TTree* t = (TTree*)f.Get("tree16");
  SX* c = new SX;
  TBranch* br = t->GetBranch ("SX");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->is == i);
    assert (c->t3.f == i*2);
    assert (c->t3.g == i*3);
  }
}


void test17 (TFile& f)
{
  std::cout << "test17\n";
  TTree* t = (TTree*)f.Get("tree17");
  U* c = new U;
  TBranch* br = t->GetBranch ("U");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->iu == i);
    assert (c->b.d == i*2);
    assert (c->b.a == i*3);
    assert (c->b.b == i*4);
    assert (c->b.c == i*5);
  }
}


void test18 (TFile& f)
{
  std::cout << "test18\n";
  TTree* t = (TTree*)f.Get("tree18");
  V* c = new V;
  TBranch* br = t->GetBranch ("V");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->iv == i);
    assert (c->e.g == i*2);
    assert (c->e.f == i*3);
    assert (c->e.h == i*4);
    assert (c->t9.i9 == i*5);
    assert (c->t9.b.d == i*6);
    assert (c->t9.b.a == i*7);
    assert (c->t9.b.b == i*8);
    assert (c->t9.b.c == i*9);
  }
}


void test19 (TFile& f)
{
  std::cout << "test19\n";
  TTree* t = (TTree*)f.Get("tree19");
  T10* c = new T10;
  TBranch* br = t->GetBranch ("T10");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->a == i);
    assert (c->b == i*2);
    assert (c->c == i*3);
  }
}


void test20 (TFile& f)
{
  std::cout << "test20\n";
  TTree* t = (TTree*)f.Get("tree20");
  T11* c = new T11;
  TBranch* br = t->GetBranch ("T11");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->a == i);
    assert (c->b == i*2);
    assert (c->c == i*3);
  }
}


void test21 (TFile& f)
{
  std::cout << "test21\n";
  TTree* t = (TTree*)f.Get("tree21");
  W* c = new W;
  TBranch* br = t->GetBranch ("W");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->iw == i);
    for (int j=0; j<3; j++) {
      assert (c->t1[j].a == i*2 + j);
      assert (c->t1[j].b == i*3 + j);
      assert (c->t1[j].c == i*4 + j);
    }
  }
}


void test22 (TFile& f)
{
  std::cout << "test22\n";
  TTree* t = (TTree*)f.Get("tree22");
  XX* c = new XX;
  TBranch* br = t->GetBranch ("XX");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert  (c->ix== i);
    assert (c->t1.size() == (unsigned)i);
    for (int j=0; j<i; j++) {
      assert (c->t1[j].a == i*2 + j);
      assert (c->t1[j].b == i*3 + j);
      assert (c->t1[j].c == i*4 + j);
    }
  }
}


void test23 (TFile& f)
{
  std::cout << "test23\n";
  TTree* t = (TTree*)f.Get("tree23");
  YY* c = new YY;
  TBranch* br = t->GetBranch ("YY");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->iy == i);
    assert (c->vi.size() == 3);
    for (int j=0; j<3; j++) {
      assert (c->vi[j].n == i*2 + j);
      assert (c->vi[j].i.m == i*3 + j);
      assert (c->vi[j].i.t4.l == i*4 + j);
      assert (c->vi[j].i.t4.d.g == i*5 + j);
    }
  }
}


void test24 (TFile& f)
{
  std::cout << "test24\n";
  TTree* t = (TTree*)f.Get("tree24");
  ZZ* c = new ZZ;
  TBranch* br = t->GetBranch ("ZZ");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->iz == i);
    assert (c->yy.iy == i*10);
    assert (c->yy.vi.size() == 3);
    for (int j=0; j<3; j++) {
      assert (c->yy.vi[j].n == i*2 + j);
      assert (c->yy.vi[j].i.m == i*3 + j);
      assert (c->yy.vi[j].i.t4.l == i*4 + j);
      assert (c->yy.vi[j].i.t4.d.g == i*5 + j);
    }
  }
}


void test25 (TFile& f)
{
  std::cout << "test25\n";
  TTree* t = (TTree*)f.Get("tree25");
  AA* c = new AA;
  TBranch* br = t->GetBranch ("AA");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->iaa == i);
    assert (c->vy.size() == 2);
    for (int k=0; k < 2; k++) {
      assert (c->vy[k].iy == i*10);
      assert (c->vy[k].vi.size() == 3);
      for (int j=0; j<3; j++) {
        assert (c->vy[k].vi[j].n == i*2 + j + k*10);
        assert (c->vy[k].vi[j].i.m == i*3 + j + k*10);
        assert (c->vy[k].vi[j].i.t4.l == i*4 + j + k*10);
        assert (c->vy[k].vi[j].i.t4.d.g == i*5 + j + k*10);
      }
    }
  }
}


void test26 (TFile& f)
{
  std::cout << "test26\n";
  TTree* t = (TTree*)f.Get("tree26");
  DD* c = new DD;
  TBranch* br = t->GetBranch ("DD");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->idd == i);
    assert (c->vcc.size() == 3);
    for (int j=0; j < 3; j++) {
      assert (c->vcc[j].t9.i9 == i*5 + j);
      assert (c->vcc[j].t9.b.d == i*6 + j);
      assert (c->vcc[j].t9.b.a == i*7 + j);
      assert (c->vcc[j].t9.b.b == i*8 + j);
      assert (c->vcc[j].t9.b.c == i*9 + j);
    }
  }
}


void test27 (TFile& f)
{
  std::cout << "test27\n";
  TTree* t = (TTree*)f.Get("tree27");
  FF* c = new FF;
  TBranch* br = t->GetBranch ("FF");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->iff == i);
    assert (c->vee.size() == 3);
    for (int j=0; j < 3; j++) {
      assert (c->vee[j].t12.i12 == i*5 + j);
      assert (c->vee[j].t12.d.g == i*6 + j);
    }
  }
}


void test28 (TFile& f)
{
  std::cout << "test28\n";
  TTree* t = (TTree*)f.Get("tree28");
  BB* c = new BB;
  TBranch* br = t->GetBranch ("BB");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->ibb == i);
    assert (c->vv.size() == 3);
    for (int j=0; j < 3; j++) {
      assert (c->vv[j].iv == i + j);
      assert (c->vv[j].e.g == i*2 + j);
      assert (c->vv[j].e.f == i*3 + j);
      assert (c->vv[j].e.h == i*4 + j);
      assert (c->vv[j].t9.i9 == i*5 + j);
      assert (c->vv[j].t9.b.d == i*6 + j);
      assert (c->vv[j].t9.b.a == i*7 + j);
      assert (c->vv[j].t9.b.b == i*8 + j);
      assert (c->vv[j].t9.b.c == i*9 + j);
    }
  }
}


void test29 (TFile& f)
{
  std::cout << "test29\n";
  TTree* t = (TTree*)f.Get("tree29");
  GG* c = new GG;
  TBranch* br = t->GetBranch ("GG");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->igg == i);
    assert (c->vt8.size() == 3);
    for (int j=0; j < 3; j++) {
      assert (c->vt8[j].a.a == i + j);
      assert (c->vt8[j].a.t.a == i*2 + j);
      assert (c->vt8[j].a.t.b == i*3 + j);
      assert (c->vt8[j].a.t.c == i*4 + j);
      assert (c->vt8[j].h.m == i*5 + j);
      assert (c->vt8[j].h.t4.l == i*6 + j);
      assert (c->vt8[j].h.t4.d.g == i*7 + j);
      assert (c->vt8[j].i8 == i*8 + j);
      assert (c->vt8[j].g == i*9 + j);
      assert (c->vt8[j].f == i*10 + j);
      assert (c->vt8[j].i7 == i*11 + j);
      assert (c->vt8[j].gg == i*12 + j);
      assert (c->vt8[j].t1.a == i*13 + j);
      assert (c->vt8[j].t1.b == i*14 + j);
      assert (c->vt8[j].t1.c == i*15 + j);
    }
  }
}


void test30 (TFile& f)
{
  std::cout << "test30\n";
  TTree* t = (TTree*)f.Get("tree30");
  HH* c = new HH;
  TBranch* br = t->GetBranch ("HH");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->ihh == i);
    assert (c->lt8.size() == 3);
    int j = 0;
    for (std::list<T8>::iterator it = c->lt8.begin();
         it != c->lt8.end();
         ++it, ++j)
    {
      assert (it->a.a == i + j);
      assert (it->a.t.a == i*2 + j);
      assert (it->a.t.b == i*3 + j);
      assert (it->a.t.c == i*4 + j);
      assert (it->h.m == i*5 + j);
      assert (it->h.t4.l == i*6 + j);
      assert (it->h.t4.d.g == i*7 + j);
      assert (it->i8 == i*8 + j);
      assert (it->g == i*9 + j);
      assert (it->f == i*10 + j);
      assert (it->i7 == i*11 + j);
      assert (it->gg == i*12 + j);
      assert (it->t1.a == i*13 + j);
      assert (it->t1.b == i*14 + j);
      assert (it->t1.c == i*15 + j);
    }
  }
}


void test31 (TFile& f)
{
  std::cout << "test31\n";
  TTree* t = (TTree*)f.Get("tree31");
  II* c = new II;
  TBranch* br = t->GetBranch ("II");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->iii == i);
    assert (c->st1.size() == 3);
    int j = 0;
    for (std::set<T1>::iterator it = c->st1.begin();
         it != c->st1.end();
         ++it, ++j)
    {
      assert (it->a == i*2 + j);
      assert (it->b == i*3 + j);
      assert (it->c == i*4 + j);
    }
  }
}


void test32 (TFile& f)
{
  std::cout << "test32\n";
  TTree* t = (TTree*)f.Get("tree32");
  JJ* c = new JJ;
  TBranch* br = t->GetBranch ("JJ");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->ijj == i);
    assert (c->sv.size() == 3);
    int j = 0;
    for (std::set<V>::iterator it = c->sv.begin();
         it != c->sv.end();
         ++it, ++j)
    {
      assert (it->iv == i + j);
      assert (it->e.g == i*2 + j);
      assert (it->e.f == i*3 + j);
      assert (it->e.h == i*4 + j);
      assert (it->t9.i9 == i*5 + j);
      assert (it->t9.b.d == i*6 + j);
      assert (it->t9.b.a == i*7 + j);
      assert (it->t9.b.b == i*8 + j);
      assert (it->t9.b.c == i*9 + j);
    }
  }
}


void test33 (TFile& f)
{
  std::cout << "test33\n";
  TTree* t = (TTree*)f.Get("tree33");
  KK* c = new KK;
  TBranch* br = t->GetBranch ("KK");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->ikk == i);
    assert (c->mt1.size() == 3);
    int j = 0;
    for (std::map<int,T1>::iterator it = c->mt1.begin();
         it != c->mt1.end();
         ++it, ++j)
    {
      assert (it->first == j);
      assert (it->second.a == i*2 + j);
      assert (it->second.b == i*3 + j);
      assert (it->second.c == i*4 + j);
    }
  }
}


void test34 (TFile& f)
{
  std::cout << "test34\n";
  TTree* t = (TTree*)f.Get("tree34");
  MM* c = new MM;
  TBranch* br = t->GetBranch ("MM");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->imm == i);
    assert (c->mo.GetEntriesFast() == 3);
    for (int j=0; j < 3; j++) {
      LL* ll = (LL*)(c->mo[j]);
      assert (ll->ill == i*2 + j);
      assert (ll->t1.a == i*3 + j);
      assert (ll->t1.b == i*4 + j);
      assert (ll->t1.c == i*5 + j);
    }
  }
}


void test35 (TFile& f)
{
  std::cout << "test35\n";
  TTree* t = (TTree*)f.Get("tree35");
  NN* c = new NN;
  TBranch* br = t->GetBranch ("NN");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->inn == i);
    assert (c->no.GetEntriesFast() == 3);
    for (int j=0; j < 3; j++) {
      T13* t13 = dynamic_cast<T13*>(c->no[j]);
      assert (t13->a == i*3 + j);
      assert (t13->b == i*4 + j);
      assert (t13->c == i*5 + j);
    }
  }
}


void test36 (TFile& f)
{
  std::cout << "test36\n";
  TTree* t = (TTree*)f.Get("tree36");
  PP* c = new PP;
  TBranch* br = t->GetBranch ("PP");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->ipp == i);
    assert (c->po.GetEntriesFast() == 3);
    for (int j=0; j < 3; j++) {
      OO* oo = dynamic_cast<OO*>(c->po[j]);
      assert (oo->ioo == i*10 + j);
      assert (oo->iv == i + j);
      assert (oo->e.g == i*2 + j);
      assert (oo->e.f == i*3 + j);
      assert (oo->e.h == i*4 + j);
      assert (oo->t9.i9 == i*5 + j);
      assert (oo->t9.b.d == i*6 + j);
      assert (oo->t9.b.a == i*7 + j);
      assert (oo->t9.b.b == i*8 + j);
      assert (oo->t9.b.c == i*9 + j);
    }
  }
}


void test37 (TFile& f)
{
  std::cout << "test37\n";
  TTree* t = (TTree*)f.Get("tree37");
  QQ* c = new QQ;
  TBranch* br = t->GetBranch ("QQ");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->iqq == i);
    assert (c->t.a == i*2);
    assert (c->t.b == i*3);
    assert (c->t.c == i*4);
    assert (c->e == i*5);
    assert (c->t3.g == i*6);
    assert (c->t3.f == i*7);
    for (int j=0; j<3; j++) {
      const T1& t1 = (*c)[j];
      assert (t1.a == i*8 + j);
      assert (t1.b == i*9 + j);
      assert (t1.c == i*10 + j);
    }
  }
}


void test38 (TFile& f)
{
  std::cout << "test38\n";
  TTree* t = (TTree*)f.Get("tree38");
  RR* c = new RR;
  TBranch* br = t->GetBranch ("RR");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->irr == i);
    assert (c->t1->a == i*2);
    assert (c->t1->b == i*3);
    assert (c->t1->c == i*4);
  }
}


void test39 (TFile& f)
{
  std::cout << "test39\n";
  TTree* t = (TTree*)f.Get("tree39");
  SS* c = new SS;
  TBranch* br = t->GetBranch ("SS");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->iss == i);
    for (int j=0; j < 3; j++) {
      assert (c->vt1[j]->a == i*2 + j);
      assert (c->vt1[j]->b == i*3 + j);
      assert (c->vt1[j]->c == i*4 + j);
    }
    c->clear();
  }
}


void test40 (TFile& f)
{
  std::cout << "test40\n";
  TTree* t = (TTree*)f.Get("tree40");
  T14* c = new T14;
  TBranch* br = t->GetBranch ("T14");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    assert (c->a.size() == c->b.size());
    for (size_t j=0; j < c->a.size(); j++) {
      assert (c->a[j] == i + (float)j/8);
      assert (c->b[j] == i + (float)j/8 + 0.5);
      assert (c->c[j] == i + 1000*j);
      assert (c->d[j] == i + 1000*j + 10);
    }
  }
}


void test41 (TFile& f)
{
  std::cout << "test41\n";
  TTree* t = (TTree*)f.Get("tree41");
  T15* c = new T15;
  TBranch* br = t->GetBranch ("T15");
  br->SetAddress (&c);
  int n = (int)t->GetEntries();
  for (int i=0; i < n; i++) {
    br->GetEntry(i);
    std::cout << *c << "\n";
    T14* cc = c->a;
    assert (cc->a.size() == cc->b.size());
    for (size_t j=0; j < cc->a.size(); j++) {
      assert (cc->a[j] == i + (float)j/8);
      assert (cc->b[j] == i + (float)j/8 + 0.5);
      assert (cc->c[j] == i + 1000*j);
      assert (cc->d[j] == i + 1000*j + 10);
    }
  }
}


int main()
{
  TROOT root ("root", "root");
  gSystem->Load ("libSTLRflx");
  gSystem->Load ("libSTLAddRflx");
  gSystem->Load ("libAtlasSTLAddReflexDict");
  TFile f ("tcls.root");
#if 1
  test1 (f);
  test2 (f);
  test3 (f);
  test4 (f);
  test5 (f);
  test6 (f);
  test7 (f);
  test8 (f);
  test9 (f);
  test10 (f);
  test11 (f);
  test12 (f);
  test13 (f);
  test14 (f);
  test15 (f);
  test16 (f);
  test17 (f);
  test18 (f);
  test19 (f);
  test20 (f);
  test21 (f);
  test22 (f);
  test23 (f);
  test24 (f);
  test25 (f);
  test26 (f);
  test27 (f);
  test28 (f);
  test29 (f);
  test30 (f);
  test31 (f);
  test32 (f);
  test33 (f);
  test34 (f);
  test35 (f);
  test36 (f);
  test37 (f);
  test38 (f);
  test39 (f);
#endif
  test40 (f);
  test41 (f);
}
