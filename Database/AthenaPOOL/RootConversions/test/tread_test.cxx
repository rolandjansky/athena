/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: tread_test.cxx,v 1.8 2009-01-21 04:34:40 ssnyder Exp $
/**
 * @file tread_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief TConvertingBranchElement regression tests.
 *
 * This reads back the data that twrite_test.cxx wrote, using
 * a different schema (converting the data).
 */


#undef NDEBUG
#include "TTree.h"
#include "RootConversions/TConvertingBranchElement.h"
#include "RootConversions/TConvertingStreamerInfo.h"
#include "RootConversions/TConverterRegistry.h"
#include "RootConversions/TVirtualConverter.h"
#include "RootConversions/VectorConverters.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "tcls2.h"
#include <iostream>
#include <cassert>

class T1_conv
  : public TVirtualConverter_T<T1, T1_old>
{
public:
  T1_conv();
  virtual void Convert (T1* newobj, const T1_old* oldobj);
};

T1_conv::T1_conv ()
  : TVirtualConverter_T<T1, T1_old> (186604412L)
{
}


void T1_conv::Convert (T1* newobj, const T1_old* oldobj)
{
  //std::cout << "convert " << newobj << " " << oldobj << "\n";
  newobj->a = oldobj->a;
  newobj->bb = oldobj->b;
  newobj->c = oldobj->c;
}


class T2_conv
  : public TVirtualConverter_T<T2, T2_old>
{
public:
  virtual void Convert (T2* newobj, const T2_old* oldobj);
};
void T2_conv::Convert (T2* newobj, const T2_old* oldobj)
{
  //std::cout << "convert 2 " << newobj << " " << oldobj << "\n";
  newobj->t = oldobj->t;
  newobj->ee = oldobj->e;
}


class T3_conv
  : public TVirtualConverter_T<T3, T3_old>
{
public:
  virtual void Convert (T3* newobj, const T3_old* oldobj);
};
void T3_conv::Convert (T3* newobj, const T3_old* oldobj)
{
  //std::cout << "convert 3 " << newobj << " " << oldobj << "\n";
  *(D*)newobj = *(D*)oldobj;
  newobj->ff = oldobj->f;
}


class T4_conv
  : public TVirtualConverter_T<T4, T4_old>
{
public:
  virtual void Convert (T4* newobj, const T4_old* oldobj);
};
void T4_conv::Convert (T4* newobj, const T4_old* oldobj)
{
  //std::cout << "convert 4 " << newobj << " " << oldobj << "\n";
  newobj->ll = oldobj->l;
  newobj->d = oldobj->d;
}


class T5_conv
  : public TVirtualConverter_T<T5, T5_old>
{
public:
  virtual void Convert (T5* newobj, const T5_old* oldobj);
};
void T5_conv::Convert (T5* newobj, const T5_old* oldobj)
{
  //std::cout << "convert 5 " << newobj << " " << oldobj << "\n";
  newobj->ii5 = oldobj->i5;
  newobj->a = oldobj->a;
}


class T6_conv
  : public TVirtualConverter_T<T6, T6_old>
{
public:
  virtual void Convert (T6* newobj, const T6_old* oldobj);
};
void T6_conv::Convert (T6* newobj, const T6_old* oldobj)
{
  //std::cout << "convert 6 " << newobj << " " << oldobj << "\n";
  newobj->ii6 = oldobj->i6;
  *(B*)newobj = *(B*)oldobj;
}


class T7_conv
  : public TVirtualConverter_T<T7, T7_old>
{
public:
  virtual void Convert (T7* newobj, const T7_old* oldobj);
};
void T7_conv::Convert (T7* newobj, const T7_old* oldobj)
{
  //std::cout << "convert 7 " << newobj << " " << oldobj << "\n";
  newobj->ii7 = oldobj->i7;
  *(QX*)newobj = *(QX*)oldobj;
}


class T8_conv
  : public TVirtualConverter_T<T8, T8_old>
{
public:
  virtual void Convert (T8* newobj, const T8_old* oldobj);
};
void T8_conv::Convert (T8* newobj, const T8_old* oldobj)
{
  //std::cout << "convert 8 " << newobj << " " << oldobj << "\n";
  newobj->ii8 = oldobj->i8;
  newobj->a = oldobj->a;
  newobj->h = oldobj->h;
  *(T3*)newobj = *(T3*)oldobj;
  *(T7*)newobj = *(T7*)oldobj;
}


class T9_conv
  : public TVirtualConverter_T<T9, T9_old>
{
public:
  virtual void Convert (T9* newobj, const T9_old* oldobj);
};
void T9_conv::Convert (T9* newobj, const T9_old* oldobj)
{
  //std::cout << "convert 9 " << newobj << " " << oldobj << "\n";
  newobj->ii9 = oldobj->i9;
  newobj->b = oldobj->b;
}


class T10_conv
  : public TVirtualConverter_T<T10, T10_old>
{
public:
  virtual void Convert (T10* newobj, const T10_old* oldobj);
};
void T10_conv::Convert (T10* newobj, const T10_old* oldobj)
{
  //std::cout << "convert 10 " << newobj << " " << oldobj << "\n";
  newobj->aa = oldobj->a;
  newobj->bb = oldobj->b;
  newobj->c.cc = oldobj->c;
}


class T11_conv
  : public TVirtualConverter_T<T11, T11_old>
{
public:
  virtual void Convert (T11* newobj, const T11_old* oldobj);
};
void T11_conv::Convert (T11* newobj, const T11_old* oldobj)
{
  //std::cout << "convert 11 " << newobj << " " << oldobj << "\n";
  newobj->aa = oldobj->a;
  newobj->b.bb = oldobj->b;
  newobj->b.c.cc = oldobj->c;
}


class T12_conv
  : public TVirtualConverter_T<T12, T12_old>
{
public:
  virtual void Convert (T12* newobj, const T12_old* oldobj);
};
void T12_conv::Convert (T12* newobj, const T12_old* oldobj)
{
  //std::cout << "convert 12 " << newobj << " " << oldobj << "\n";
  newobj->ii12 = oldobj->i12;
  newobj->d = oldobj->d;
}


class T13_conv
  : public TVirtualConverter_T<T13, T13_old>
{
public:
  virtual void Convert (T13* newobj, const T13_old* oldobj);
};
void T13_conv::Convert (T13* newobj, const T13_old* oldobj)
{
  //std::cout << "convert " << newobj << " " << oldobj << "\n";
  *(TObject*)newobj = *(TObject*)oldobj;
  newobj->a = oldobj->a;
  newobj->bb = oldobj->b;
  newobj->c = oldobj->c;
}


class vec_conv
  : public TVirtualConverter_T<std::vector<float>, std::vector<double> >
{
public:
  virtual void Convert (std::vector<float>* newobj, const std::vector<double>* oldobj);
  vec_conv() : TVirtualConverter_T<std::vector<float>, std::vector<double> >(269370212) {}
};
void vec_conv::Convert (std::vector<float>* newobj, const std::vector<double>* oldobj)
{
  newobj->assign (oldobj->begin(), oldobj->end());
}


void maybe_write (TTree* tree, TFile* fout)
{
  if (fout) {
    TDirectory::TContext ctx (fout);
    tree->Write();
    fout->Write();
  }
}


void test1 (TFile& f, TFile* fout)
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
    assert (c->bb == i*2);
    assert (c->c == i*3);
  }
  maybe_write (t, fout);
}


void test2 (TFile& f, TFile* fout)
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
    assert (a->t.bb == i*3);
    assert (a->t.c == i*4);
  }
  maybe_write (t, fout);
}


void test3 (TFile& f, TFile* fout)
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
    assert (b->bb == i*3);
    assert (b->c == i*4);
  }
  maybe_write (t, fout);
}


void test4 (TFile& f, TFile* fout)
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
    assert (c->u.ee == i*2);
    assert (c->u.t.a == i*3);
    assert (c->u.t.bb == i*4);
    assert (c->u.t.c == i*5);
  }
  maybe_write (t, fout);
}


void test5 (TFile& f, TFile* fout)
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
    assert (c->ff == i*2);
    assert (c->h == i*3);
  }
  maybe_write (t, fout);
}


void test6 (TFile& f, TFile* fout)
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
    assert (c->bb == i*3);
    assert (c->c == i*4);
    assert (c->j == i*5);
  }
  maybe_write (t, fout);
}


void test7 (TFile& f, TFile* fout)
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
    assert (c->bb == i*2);
    assert (c->c == i*3);
    assert (c->k == i*4);
    assert (c->t.a == i*5);
    assert (c->t.bb == i*6);
    assert (c->t.c == i*7);
  }
  maybe_write (t, fout);
}


void test8 (TFile& f, TFile* fout)
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
    assert (c->i.t4.ll == i*3);
    assert (c->i.t4.d.g == i*4);
  }
  maybe_write (t, fout);
}


void test9 (TFile& f, TFile* fout)
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
    assert (c->j.a.t.bb == i*5);
    assert (c->j.a.t.c == i*6);
  }
  maybe_write (t, fout);
}


void test10 (TFile& f, TFile* fout)
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
    assert (c->t5.ii5 == i*2);
    assert (c->t5.a.a == i*3);
    assert (c->t5.a.t.a == i*4);
    assert (c->t5.a.t.bb == i*5);
    assert (c->t5.a.t.c == i*6);
  }
  maybe_write (t, fout);
}


void test11 (TFile& f, TFile* fout)
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
    assert (c->ff == i*3);
    assert (c->g == i*4);
  }
  maybe_write (t, fout);
}


void test12 (TFile& f, TFile* fout)
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
    assert (c->bb == i*5);
    assert (c->c == i*6);
  }
  maybe_write (t, fout);
}


void test13 (TFile& f, TFile* fout)
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
    assert (c->ii6 == i*2);
    assert (c->d == i*3);
    assert (c->a == i*4);
    assert (c->bb == i*5);
    assert (c->c == i*6);
  }
  maybe_write (t, fout);
}


void test14 (TFile& f, TFile* fout)
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
    assert (c->t.bb == i*3);
    assert (c->t.c == i*4);
  }
  maybe_write (t, fout);
}


void test15 (TFile& f, TFile* fout)
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
    assert (c->a.t.bb == i*3);
    assert (c->a.t.c == i*4);
    assert (c->h.m == i*5);
    assert (c->h.t4.ll == i*6);
    assert (c->h.t4.d.g == i*7);
    assert (c->ii8 == i*8);
    assert (c->g == i*9);
    assert (c->ff == i*10);
    assert (c->ii7 == i*11);
    assert (c->gg == i*12);
    assert (c->t1.a == i*13);
    assert (c->t1.bb == i*14);
    assert (c->t1.c == i*15);
  }
  maybe_write (t, fout);
}


void test16 (TFile& f, TFile* fout)
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
    assert (c->t3.ff == i*2);
    assert (c->t3.g == i*3);
  }
  maybe_write (t, fout);
}


void test17 (TFile& f, TFile* fout)
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
    assert (c->b.bb == i*4);
    assert (c->b.c == i*5);
  }
  maybe_write (t, fout);
}


void test18 (TFile& f, TFile* fout)
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
    assert (c->e.ff == i*3);
    assert (c->e.h == i*4);
    assert (c->t9.ii9 == i*5);
    assert (c->t9.b.d == i*6);
    assert (c->t9.b.a == i*7);
    assert (c->t9.b.bb == i*8);
    assert (c->t9.b.c == i*9);
  }
  maybe_write (t, fout);
}


void test19 (TFile& f, TFile* fout)
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
    assert (c->aa == i);
    assert (c->bb == i*2);
    assert (c->c.cc == i*3);
  }
  maybe_write (t, fout);
}


void test20 (TFile& f, TFile* fout)
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
    assert (c->aa == i);
    assert (c->b.bb == i*2);
    assert (c->b.c.cc == i*3);
  }
  maybe_write (t, fout);
}


void test21 (TFile& f, TFile* fout)
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
      assert (c->t1[j].bb == i*3 + j);
      assert (c->t1[j].c == i*4 + j);
    }
  }
  maybe_write (t, fout);
}


void test22 (TFile& f, TFile* fout)
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
      assert (c->t1[j].bb == i*3 + j);
      assert (c->t1[j].c == i*4 + j);
    }
  }
  maybe_write (t, fout);
}


void test23 (TFile& f, TFile* fout)
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
      assert (c->vi[j].i.t4.ll == i*4 + j);
      assert (c->vi[j].i.t4.d.g == i*5 + j);
    }
  }
  maybe_write (t, fout);
}


void test24 (TFile& f, TFile* fout)
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
      assert (c->yy.vi[j].i.t4.ll == i*4 + j);
      assert (c->yy.vi[j].i.t4.d.g == i*5 + j);
    }
  }
  maybe_write (t, fout);
}


void test25 (TFile& f, TFile* fout)
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
        assert (c->vy[k].vi[j].i.t4.ll == i*4 + j + k*10);
        assert (c->vy[k].vi[j].i.t4.d.g == i*5 + j + k*10);
      }
    }
  }
  maybe_write (t, fout);
}


void test26 (TFile& f, TFile* fout)
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
      assert (c->vcc[j].t9.ii9 == i*5 + j);
      assert (c->vcc[j].t9.b.d == i*6 + j);
      assert (c->vcc[j].t9.b.a == i*7 + j);
      assert (c->vcc[j].t9.b.bb == i*8 + j);
      assert (c->vcc[j].t9.b.c == i*9 + j);
    }
  }
  maybe_write (t, fout);
}


void test27 (TFile& f, TFile* fout)
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
      assert (c->vee[j].t12.ii12 == i*5 + j);
      assert (c->vee[j].t12.d.g == i*6 + j);
    }
  }
  maybe_write (t, fout);
}


void test28 (TFile& f, TFile* fout)
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
      assert (c->vv[j].e.ff == i*3 + j);
      assert (c->vv[j].e.h == i*4 + j);
      assert (c->vv[j].t9.ii9 == i*5 + j);
      assert (c->vv[j].t9.b.d == i*6 + j);
      assert (c->vv[j].t9.b.a == i*7 + j);
      assert (c->vv[j].t9.b.bb == i*8 + j);
      assert (c->vv[j].t9.b.c == i*9 + j);
    }
  }
  maybe_write (t, fout);
}


void test29 (TFile& f, TFile* fout)
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
      assert (c->vt8[j].a.t.bb == i*3 + j);
      assert (c->vt8[j].a.t.c == i*4 + j);
      assert (c->vt8[j].h.m == i*5 + j);
      assert (c->vt8[j].h.t4.ll == i*6 + j);
      assert (c->vt8[j].h.t4.d.g == i*7 + j);
      assert (c->vt8[j].ii8 == i*8 + j);
      assert (c->vt8[j].g == i*9 + j);
      assert (c->vt8[j].ff == i*10 + j);
      assert (c->vt8[j].ii7 == i*11 + j);
      assert (c->vt8[j].gg == i*12 + j);
      assert (c->vt8[j].t1.a == i*13 + j);
      assert (c->vt8[j].t1.bb == i*14 + j);
      assert (c->vt8[j].t1.c == i*15 + j);
    }
  }
  maybe_write (t, fout);
}


void test30 (TFile& f, TFile* fout)
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
      assert (it->a.t.bb == i*3 + j);
      assert (it->a.t.c == i*4 + j);
      assert (it->h.m == i*5 + j);
      assert (it->h.t4.ll == i*6 + j);
      assert (it->h.t4.d.g == i*7 + j);
      assert (it->ii8 == i*8 + j);
      assert (it->g == i*9 + j);
      assert (it->ff == i*10 + j);
      assert (it->ii7 == i*11 + j);
      assert (it->gg == i*12 + j);
      assert (it->t1.a == i*13 + j);
      assert (it->t1.bb == i*14 + j);
      assert (it->t1.c == i*15 + j);
    }
  }
  maybe_write (t, fout);
}


void test31 (TFile& f, TFile* fout)
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
      assert (it->bb == i*3 + j);
      assert (it->c == i*4 + j);
    }
  }
  maybe_write (t, fout);
}


void test32 (TFile& f, TFile* fout)
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
      assert (it->e.ff == i*3 + j);
      assert (it->e.h == i*4 + j);
      assert (it->t9.ii9 == i*5 + j);
      assert (it->t9.b.d == i*6 + j);
      assert (it->t9.b.a == i*7 + j);
      assert (it->t9.b.bb == i*8 + j);
      assert (it->t9.b.c == i*9 + j);
    }
  }
  maybe_write (t, fout);
}


void test33 (TFile& f, TFile* fout)
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
      assert (it->second.bb == i*3 + j);
      assert (it->second.c == i*4 + j);
    }
  }
  maybe_write (t, fout);
}


void test34 (TFile& f, TFile* fout)
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
      assert (ll->t1.bb == i*4 + j);
      assert (ll->t1.c == i*5 + j);
    }
  }
  maybe_write (t, fout);
}


void test35 (TFile& f, TFile* fout)
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
      assert (t13->bb == i*4 + j);
      assert (t13->c == i*5 + j);
    }
  }
  maybe_write (t, fout);
}


void test36 (TFile& f, TFile* fout)
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
      assert (oo->e.ff == i*3 + j);
      assert (oo->e.h == i*4 + j);
      assert (oo->t9.ii9 == i*5 + j);
      assert (oo->t9.b.d == i*6 + j);
      assert (oo->t9.b.a == i*7 + j);
      assert (oo->t9.b.bb == i*8 + j);
      assert (oo->t9.b.c == i*9 + j);
    }
  }
  maybe_write (t, fout);
}


void test37 (TFile& f, TFile* fout)
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
    assert (c->t.bb == i*3);
    assert (c->t.c == i*4);
    assert (c->ee == i*5);
    assert (c->t3.g == i*6);
    assert (c->t3.ff == i*7);
    for (int j=0; j<3; j++) {
      const T1& t1 = (*c)[j];
      assert (t1.a == i*8 + j);
      assert (t1.bb == i*9 + j);
      assert (t1.c == i*10 + j);
    }
  }
  maybe_write (t, fout);
}


void test38 (TFile& f, TFile* fout)
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
    assert (c->t1->bb == i*3);
    assert (c->t1->c == i*4);
  }
  maybe_write (t, fout);
}


void test39 (TFile& f, TFile* fout)
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
      assert (c->vt1[j]->bb == i*3 + j);
      assert (c->vt1[j]->c == i*4 + j);
    }
    c->clear();
  }
  maybe_write (t, fout);
}


void test40 (TFile& f, TFile* fout)
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
  maybe_write (t, fout);
}


void test41 (TFile& f, TFile* fout)
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
  maybe_write (t, fout);
}


void runtest (TFile* fin, TFile* fout)
{
  test1 (*fin, fout);
  test2 (*fin, fout);
  test3 (*fin, fout);
  test4 (*fin, fout);
  test5 (*fin, fout);
  test6 (*fin, fout);
  test7 (*fin, fout);
  test8 (*fin, fout);
  test9 (*fin, fout);
  test10 (*fin, fout);
  test11 (*fin, fout);
  test12 (*fin, fout);
  test13 (*fin, fout);
  test14 (*fin, fout);
  test15 (*fin, fout);
  test16 (*fin, fout);
  test17 (*fin, fout);
  test18 (*fin, fout);
  test19 (*fin, fout);
  test20 (*fin, fout);
  test21 (*fin, fout);
  test22 (*fin, fout);
  test23 (*fin, fout);
  test24 (*fin, fout);
  test25 (*fin, fout);
  test26 (*fin, fout);
  test27 (*fin, fout);
  test28 (*fin, fout);
  test29 (*fin, fout);
  test30 (*fin, fout);
  test31 (*fin, fout);
  test32 (*fin, fout);
  test33 (*fin, fout);
  test34 (*fin, fout);
  test35 (*fin, fout);
  test36 (*fin, fout);
  test37 (*fin, fout);
  test38 (*fin, fout);
  test39 (*fin, fout);
  test40 (*fin, fout);
  test41 (*fin, fout);
}


int main()
{
  TROOT root ("root", "root");
  gSystem->Load ("libSTLRflx");
  gSystem->Load ("libSTLAddRflx");
  gSystem->Load ("libAtlasSTLAddReflexDict");
  TConvertingStreamerInfo::Initialize();
  RootConversions::VectorConverters::initialize();
  TConverterRegistry::Instance()->AddConverter (new T1_conv);
  TConverterRegistry::Instance()->AddConverter (new T2_conv);
  TConverterRegistry::Instance()->AddConverter (new T3_conv);
  TConverterRegistry::Instance()->AddConverter (new T4_conv);
  TConverterRegistry::Instance()->AddConverter (new T5_conv);
  TConverterRegistry::Instance()->AddConverter (new T6_conv);
  TConverterRegistry::Instance()->AddConverter (new T7_conv);
  TConverterRegistry::Instance()->AddConverter (new T8_conv);
  TConverterRegistry::Instance()->AddConverter (new T9_conv);
  TConverterRegistry::Instance()->AddConverter (new T10_conv);
  TConverterRegistry::Instance()->AddConverter (new T11_conv);
  TConverterRegistry::Instance()->AddConverter (new T12_conv);
  TConverterRegistry::Instance()->AddConverter (new T13_conv);
  TConverterRegistry::Instance()->AddConverter (new vec_conv);
  {
    TFile fout ("tcls2.root", "RECREATE");
    TFile f ("tcls.root");
    runtest (&f, &fout);
    fout.Close();
  }
#if 0
  {
    TFile f2 ("tcls2.root");
    runtest (&f2, 0);
  }
#endif
}
