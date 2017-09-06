/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: twrite_test.cxx,v 1.7 2009-01-21 04:34:40 ssnyder Exp $
/**
 * @file twrite_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief TConvertingBranchElement regression tests.
 *
 * This is the writing side of the regression test.
 * This writes a bunch of classes which we'll then try to read
 * after schema evolution.
 */


#undef NDEBUG
#include "TTree.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "tcls1.h"

void test1(TFile& f)
{
  TTree tree ("tree", "tree");
  T1* c = new T1;
  tree.Branch ("T1", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->a = i;
    c->b = i*2;
    c->c = i*3;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}

void test2(TFile& f)
{
  TTree tree ("tree2", "tree2");
  A* a = new A;
  tree.Branch ("A", &a, 16000, 99);
  for (int i=0; i < 10; i++) {
    a->a = i;
    a->t.a = i*2;
    a->t.b = i*3;
    a->t.c = i*4;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}

void test3(TFile& f)
{
  TTree tree ("tree3", "tree3");
  B* b = new B;
  tree.Branch ("B", &b, 16000, 99);
  for (int i=0; i < 10; i++) {
    b->d = i;
    b->a = i*2;
    b->b = i*3;
    b->c = i*4;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test4(TFile& f)
{
  TTree tree ("tree4", "tree4");
  C* c = new C;
  tree.Branch ("C", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->cc = i;
    c->u.e = i*2;
    c->u.t.a = i*3;
    c->u.t.b = i*4;
    c->u.t.c = i*5;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test5(TFile& f)
{
  TTree tree ("tree5", "tree5");
  E* c = new E;
  tree.Branch ("E", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->g = i;
    c->f = i*2;
    c->h = i*3;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test6(TFile& f)
{
  TTree tree ("tree6", "tree6");
  FX* c = new FX;
  tree.Branch ("FX", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->g = i;
    c->a = i*2;
    c->b = i*3;
    c->c = i*4;
    c->j = i*5;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test7(TFile& f)
{
  TTree tree ("tree7", "tree7");
  G* c = new G;
  tree.Branch ("G", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->a = i;
    c->b = i*2;
    c->c = i*3;
    c->k = i*4;
    c->t.a = i*5;
    c->t.b = i*6;
    c->t.c = i*7;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test8(TFile& f)
{
  TTree tree ("tree8", "tree8");
  I* c = new I;
  tree.Branch ("I", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->n = i;
    c->i.m = i*2;
    c->i.t4.l = i*3;
    c->i.t4.d.g = i*4;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test9(TFile& f)
{
  TTree tree ("tree9", "tree9");
  K* c = new K;
  tree.Branch ("K", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->ik = i;
    c->j.ij = i*2;
    c->j.a.a = i*3;
    c->j.a.t.a = i*4;
    c->j.a.t.b = i*5;
    c->j.a.t.c = i*6;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test10(TFile& f)
{
  TTree tree ("tree10", "tree10");
  L* c = new L;
  tree.Branch ("L", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->il = i;
    c->t5.i5 = i*2;
    c->t5.a.a = i*3;
    c->t5.a.t.a = i*4;
    c->t5.a.t.b = i*5;
    c->t5.a.t.c = i*6;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test11 (TFile& f)
{
  TTree tree ("tree11", "tree11");
  M* c = new M;
  tree.Branch ("M", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->im = i;
    c->h = i*2;
    c->f = i*3;
    c->g = i*4;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test12 (TFile& f)
{
  TTree tree ("tree12", "tree12");
  O* c = new O;
  tree.Branch ("O", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->io = i;
    c->in = i*2;
    c->d = i*3;
    c->a = i*4;
    c->b = i*5;
    c->c = i*6;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test13 (TFile& f)
{
  TTree tree ("tree13", "tree13");
  P* c = new P;
  tree.Branch ("P", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->ip = i;
    c->i6 = i*2;
    c->d = i*3;
    c->a = i*4;
    c->b = i*5;
    c->c = i*6;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test14 (TFile& f)
{
  TTree tree ("tree14", "tree14");
  R* c = new R;
  tree.Branch ("R", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->a = i;
    c->t.a = i*2;
    c->t.b = i*3;
    c->t.c = i*4;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test15 (TFile& f)
{
  TTree tree ("tree15", "tree15");
  T8* c = new T8;
  tree.Branch ("T8", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->a.a = i;
    c->a.t.a = i*2;
    c->a.t.b = i*3;
    c->a.t.c = i*4;
    c->h.m = i*5;
    c->h.t4.l = i*6;
    c->h.t4.d.g = i*7;
    c->i8 = i*8;
    c->g = i*9;
    c->f = i*10;
    c->i7 = i*11;
    c->gg = i*12;
    c->t1.a = i*13;
    c->t1.b = i*14;
    c->t1.c = i*15;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test16 (TFile& f)
{
  TTree tree ("tree16", "tree16");
  SX* c = new SX;
  tree.Branch ("SX", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->is = i;
    c->t3.f = i*2;
    c->t3.g = i*3;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test17 (TFile& f)
{
  TTree tree ("tree17", "tree17");
  U* c = new U;
  tree.Branch ("U", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->iu = i;
    c->b.d = i*2;
    c->b.a = i*3;
    c->b.b = i*4;
    c->b.c = i*5;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test18 (TFile& f)
{
  TTree tree ("tree18", "tree18");
  V* c = new V;
  tree.Branch ("V", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->iv = i;
    c->e.g = i*2;
    c->e.f = i*3;
    c->e.h = i*4;
    c->t9.i9 = i*5;
    c->t9.b.d = i*6;
    c->t9.b.a = i*7;
    c->t9.b.b = i*8;
    c->t9.b.c = i*9;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test19 (TFile& f)
{
  TTree tree ("tree19", "tree19");
  T10* c = new T10;
  tree.Branch ("T10", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->a = i;
    c->b = i*2;
    c->c = i*3;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test20 (TFile& f)
{
  TTree tree ("tree20", "tree20");
  T11* c = new T11;
  tree.Branch ("T11", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->a = i;
    c->b = i*2;
    c->c = i*3;
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test21 (TFile& f)
{
  TTree tree ("tree21", "tree21");
  W* c = new W;
  tree.Branch ("W", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->iw= i;
    for (int j=0; j<3; j++) {
      c->t1[j].a = i*2 + j;
      c->t1[j].b = i*3 + j;
      c->t1[j].c = i*4 + j;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test22 (TFile& f)
{
  TTree tree ("tree22", "tree22");
  XX* c = new XX;
  tree.Branch ("XX", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->ix= i;
    c->t1.resize (i);
    for (int j=0; j<i; j++) {
      c->t1[j].a = i*2 + j;
      c->t1[j].b = i*3 + j;
      c->t1[j].c = i*4 + j;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test23 (TFile& f)
{
  TTree tree ("tree23", "tree23");
  YY* c = new YY;
  tree.Branch ("YY", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->iy = i;
    c->vi.resize (3);
    for (int j=0; j<3; j++) {
      c->vi[j].n = i*2 + j;
      c->vi[j].i.m = i*3 + j;
      c->vi[j].i.t4.l = i*4 + j;
      c->vi[j].i.t4.d.g = i*5 + j;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test24 (TFile& f)
{
  TTree tree ("tree24", "tree24");
  ZZ* c = new ZZ;
  tree.Branch ("ZZ", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->iz = i;
    c->yy.iy = i*10;
    c->yy.vi.resize (3);
    for (int j=0; j<3; j++) {
      c->yy.vi[j].n = i*2 + j;
      c->yy.vi[j].i.m = i*3 + j;
      c->yy.vi[j].i.t4.l = i*4 + j;
      c->yy.vi[j].i.t4.d.g = i*5 + j;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test25 (TFile& f)
{
  TTree tree ("tree25", "tree25");
  AA* c = new AA;
  tree.Branch ("AA", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->iaa = i;
    c->vy.resize (2);
    for (int k=0; k < 2; k++) {
      c->vy[k].iy = i*10;
      c->vy[k].vi.resize (3);
      for (int j=0; j<3; j++) {
        c->vy[k].vi[j].n = i*2 + j + k*10;
        c->vy[k].vi[j].i.m = i*3 + j + k*10;
        c->vy[k].vi[j].i.t4.l = i*4 + j + k*10;
        c->vy[k].vi[j].i.t4.d.g = i*5 + j + k*10;
      }
    }
    tree.Fill();

  }
  tree.Print();
  f.Write();
}


void test26 (TFile& f)
{
  TTree tree ("tree26", "tree26");
  DD* c = new DD;
  tree.Branch ("DD", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->idd = i;
    c->vcc.resize (3);
    for (int j=0; j < 3; j++) {
      c->vcc[j].t9.i9 = i*5 + j;
      c->vcc[j].t9.b.d = i*6 + j;
      c->vcc[j].t9.b.a = i*7 + j;
      c->vcc[j].t9.b.b = i*8 + j;
      c->vcc[j].t9.b.c = i*9 + j;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test27 (TFile& f)
{
  TTree tree ("tree27", "tree27");
  FF* c = new FF;
  tree.Branch ("FF", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->iff = i;
    c->vee.resize (3);
    for (int j=0; j < 3; j++) {
      c->vee[j].t12.i12 = i*5 + j;
      c->vee[j].t12.d.g = i*6 + j;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test28 (TFile& f)
{
  TTree tree ("tree28", "tree28");
  BB* c = new BB;
  tree.Branch ("BB", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->ibb = i;
    c->vv.resize (3);
    for (int j=0; j < 3; j++) {
      c->vv[j].iv = i + j;
      c->vv[j].e.g = i*2 + j;
      c->vv[j].e.f = i*3 + j;
      c->vv[j].e.h = i*4 + j;
      c->vv[j].t9.i9 = i*5 + j;
      c->vv[j].t9.b.d = i*6 + j;
      c->vv[j].t9.b.a = i*7 + j;
      c->vv[j].t9.b.b = i*8 + j;
      c->vv[j].t9.b.c = i*9 + j;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test29 (TFile& f)
{
  TTree tree ("tree29", "tree29");
  GG* c = new GG;
  tree.Branch ("GG", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->igg = i;
    c->vt8.resize (3);
    for (int j=0; j < 3; j++) {
      c->vt8[j].a.a = i + j;
      c->vt8[j].a.t.a = i*2 + j;
      c->vt8[j].a.t.b = i*3 + j;
      c->vt8[j].a.t.c = i*4 + j;
      c->vt8[j].h.m = i*5 + j;
      c->vt8[j].h.t4.l = i*6 + j;
      c->vt8[j].h.t4.d.g = i*7 + j;
      c->vt8[j].i8 = i*8 + j;
      c->vt8[j].g = i*9 + j;
      c->vt8[j].f = i*10 + j;
      c->vt8[j].i7 = i*11 + j;
      c->vt8[j].gg = i*12 + j;
      c->vt8[j].t1.a = i*13 + j;
      c->vt8[j].t1.b = i*14 + j;
      c->vt8[j].t1.c = i*15 + j;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test30 (TFile& f)
{
  TTree tree ("tree30", "tree30");
  HH* c = new HH;
  tree.Branch ("HH", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->ihh = i;
    c->lt8.resize (3);
    int j = 0;
    for (std::list<T8>::iterator it = c->lt8.begin();
         it != c->lt8.end();
         ++it, ++j)
    {
      it->a.a = i + j;
      it->a.t.a = i*2 + j;
      it->a.t.b = i*3 + j;
      it->a.t.c = i*4 + j;
      it->h.m = i*5 + j;
      it->h.t4.l = i*6 + j;
      it->h.t4.d.g = i*7 + j;
      it->i8 = i*8 + j;
      it->g = i*9 + j;
      it->f = i*10 + j;
      it->i7 = i*11 + j;
      it->gg = i*12 + j;
      it->t1.a = i*13 + j;
      it->t1.b = i*14 + j;
      it->t1.c = i*15 + j;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test31 (TFile& f)
{
  TTree tree ("tree31", "tree31");
  II* c = new II;
  tree.Branch ("II", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->st1.clear();
    c->iii = i;
    for (int j=0; j<3; j++) {
      T1 t1;
      t1.a = i*2 + j;
      t1.b = i*3 + j;
      t1.c = i*4 + j;
      c->st1.insert (t1);
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test32 (TFile& f)
{
  TTree tree ("tree32", "tree32");
  JJ* c = new JJ;
  tree.Branch ("JJ", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->sv.clear();
    c->ijj = i;
    for (int j=0; j<3; j++) {
      V v;
      v.iv = i + j;
      v.e.g = i*2 + j;
      v.e.f = i*3 + j;
      v.e.h = i*4 + j;
      v.t9.i9 = i*5 + j;
      v.t9.b.d = i*6 + j;
      v.t9.b.a = i*7 + j;
      v.t9.b.b = i*8 + j;
      v.t9.b.c = i*9 + j;
      c->sv.insert (v);
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test33 (TFile& f)
{
  TTree tree ("tree33", "tree33");
  KK* c = new KK;
  tree.Branch ("KK", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->mt1.clear();
    c->ikk = i;
    for (int j=0; j<3; j++) {
      T1 t1;
      t1.a = i*2 + j;
      t1.b = i*3 + j;
      t1.c = i*4 + j;
      c->mt1[j] = t1;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test34 (TFile& f)
{
  TTree tree ("tree34", "tree34");
  MM* c = new MM (3);
  tree.Branch ("MM", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->imm = i;
    for (int j=0; j<3; j++) {
      LL* ll = new (c->mo[j]) LL;
      ll->ill = i*2 + j;
      ll->t1.a = i*3 + j;
      ll->t1.b = i*4 + j;
      ll->t1.c = i*5 + j;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test35 (TFile& f)
{
  TTree tree ("tree35", "tree35");
  NN* c = new NN (3);
  tree.Branch ("NN", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->inn = i;
    for (int j=0; j<3; j++) {
      T13* t13 = new (c->no[j]) T13;
      t13->a = i*3 + j;
      t13->b = i*4 + j;
      t13->c = i*5 + j;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test36 (TFile& f)
{
  TTree tree ("tree36", "tree36");
  PP* c = new PP (3);
  tree.Branch ("PP", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->ipp = i;
    for (int j=0; j<3; j++) {
      OO* oo = new (c->po[j]) OO;
      oo->ioo = i*10 + j;
      oo->iv = i + j;
      oo->e.g = i*2 + j;
      oo->e.f = i*3 + j;
      oo->e.h = i*4 + j;
      oo->t9.i9 = i*5 + j;
      oo->t9.b.d = i*6 + j;
      oo->t9.b.a = i*7 + j;
      oo->t9.b.b = i*8 + j;
      oo->t9.b.c = i*9 + j;
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test37 (TFile& f)
{
  TTree tree ("tree37", "tree37");
  QQ* c = new QQ;
  tree.Branch ("QQ", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->iqq = i;
    c->t.a = i*2;
    c->t.b = i*3;
    c->t.c = i*4;
    c->e = i*5;
    c->t3.g = i*6;
    c->t3.f = i*7;
    c->clear();
    for (int j=0; j<3; j++) {
      T1 t1;
      t1.a = i*8 + j;
      t1.b = i*9 + j;
      t1.c = i*10 + j;
      c->push_back (t1);
    }
    tree.Fill();
  }
  tree.Print();
  f.Write();
}


void test38 (TFile& f)
{
  TTree tree ("tree38", "tree38");
  RR* c = new RR;
  tree.Branch ("RR", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->irr = i;
    c->t1 = new T1;
    c->t1->a = i*2;
    c->t1->b = i*3;
    c->t1->c = i*4;
    tree.Fill();
    delete c->t1;
  }
  tree.Print();
  f.Write();
}


void test39 (TFile& f)
{
  TTree tree ("tree39", "tree39");
  SS* c = new SS;
  tree.Branch ("SS", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    c->iss = i;
    for (int j=0; j < 3; j++) {
      T1* t1 = new T1;
      t1->a = i*2 + j;
      t1->b = i*3 + j;
      t1->c = i*4 + j;
      c->vt1.push_back (t1);
    }
    tree.Fill();
    c->clear();
  }
  tree.Print();
  f.Write();
}


void test40 (TFile& f)
{
  TTree tree ("tree40", "tree40");
  T14* c = new T14;
  tree.Branch ("T14", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    for (int j=0; j < 3; j++) {
      c->a.push_back (i + (double)j/8);
      c->b.push_back (i + (double)j/8 + 0.5);
      c->c.push_back (i + 1000*j);
      c->d.push_back (i + 1000*j + 10);
    }
    tree.Fill();
    c->a.clear();
    c->b.clear();
    c->c.clear();
    c->d.clear();
  }
  tree.Print();
  f.Write();
}


void test41 (TFile& f)
{
  TTree tree ("tree41", "tree41");
  T15* c = new T15;
  tree.Branch ("T15", &c, 16000, 99);
  for (int i=0; i < 10; i++) {
    T14* cc = new T14;
    for (int j=0; j < 3; j++) {
      cc->a.push_back (i + (double)j/8);
      cc->b.push_back (i + (double)j/8 + 0.5);
      cc->c.push_back (i + 1000*j);
      cc->d.push_back (i + 1000*j + 10);
    }
    c->a = cc;
    tree.Fill();
    delete cc;
  }
  tree.Print();
  f.Write();
}


int main()
{
  TROOT root ("root", "root");
  gSystem->Load ("libSTLRflx");
  gSystem->Load ("libSTLAddRflx");
  gSystem->Load ("libAtlasSTLAddReflexDict");
  TFile f ("tcls.root", "RECREATE");
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
  return 0;
}
