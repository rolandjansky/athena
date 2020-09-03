/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  AthenaKernel/test/BaseInfo_test.cxx
 * @author scott snyder
 * @date A while ago.
 * @brief Regression test for BaseInfo.
 */


#undef NDEBUG

#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "boost/assign/list_of.hpp"
#include <iostream>
#include <algorithm>
#include <cassert>

using std::cout;
using boost::assign::list_of;

struct AA
{
  AA (int the_x=0): x(the_x) {}
  ~AA() { }
  int x;
};
struct BB : public AA
{
  BB (int the_x=0) : AA(the_x) {}
};
struct CC : public BB
{
  CC (int the_x=0) : BB(the_x) {}
};

SG_BASES(BB, AA);
SG_BASES(CC, BB);

CLASS_DEF (AA, 1111, 2)
CLASS_DEF (BB, 1112, 2)
CLASS_DEF (CC, 1113, 2)

struct M
{
  M (int the_x=0) : x(the_x) {}
  virtual ~M() { }
  int x;
};
struct N : virtual public M 
{
  N (int the_x=0) : M(the_x) {}
};
struct O : virtual public M
{
  O (int the_x=0) : M(the_x) {}
};
struct P : virtual public N, virtual public O
{
  P (int the_x=0) : M(the_x) {}
};

CLASS_DEF (M, 1114, 2)
CLASS_DEF (N, 1115, 2)
CLASS_DEF (O, 1116, 2)
CLASS_DEF (P, 1117, 2)

SG_BASES (N, SG_VIRTUAL (M));
SG_BASES (O, SG_VIRTUAL (M));
SG_BASES (P, SG_VIRTUAL (N), SG_VIRTUAL (O));

struct X1 {};
struct X2 : public X1 {};
SG_BASES (X2, X1);

struct Q : virtual public M
{
  Q (int the_x=0) : M(the_x) {}
};
struct R : virtual public N, virtual public O, virtual public Q
{
  R (int the_x=0) : M(the_x) {}
};

CLASS_DEF (Q, 1118, 2)
CLASS_DEF (R, 1119, 2)

SG_BASES (Q, SG_VIRTUAL (M));
SG_BASES (R, SG_VIRTUAL (N),
          SG_VIRTUAL (O),
          SG_VIRTUAL (Q));


struct I1 : public AA {};
struct I2 : public AA {};
SG_BASES (I1, AA);
SG_BASES (I2, AA);
CLASS_DEF (I2, 1120, 2)

struct J : public AA, virtual public X1, public M {};
SG_BASES(J, AA);
SG_ADD_BASE(J, SG_VIRTUAL(X1));
SG_ADD_BASE(J, M);

struct K : public AA {};
SG_ADD_BASE(K, AA);


int test1()
{
  std::cout << "test1\n";

  typedef SG::BaseInfo<CC> CC_C;
  typedef SG::BaseInfo<R>  R_C;

  std::vector<CLID> clids;
  clids = CC_C::get_bases();
  std::vector<CLID> exp1 = list_of
    (ClassID_traits<CC>::ID())
    (ClassID_traits<BB>::ID())
    (ClassID_traits<AA>::ID());
  std::sort (clids.begin(), clids.end());
  std::sort (exp1.begin(), exp1.end());
  assert (clids == exp1);

  std::vector<const std::type_info*> tinfos;
  tinfos = CC_C::get_ti_bases();
  std::vector<const std::type_info*> exp1ti = list_of
    (&typeid(CC))
    (&typeid(BB))
    (&typeid(AA));
  std::sort (tinfos.begin(), tinfos.end());
  std::sort (exp1ti.begin(), exp1ti.end());
  assert (tinfos == exp1ti);

  clids = R_C::get_bases();
  std::vector<CLID> exp2 = list_of
    (ClassID_traits<M>::ID())
    (ClassID_traits<N>::ID())
    (ClassID_traits<O>::ID())
    (ClassID_traits<Q>::ID())
    (ClassID_traits<R>::ID());
  std::sort (clids.begin(), clids.end());
  std::sort (exp2.begin(), exp2.end());
  assert (clids == exp2);

  tinfos = R_C::get_ti_bases();
  std::vector<const std::type_info*> exp2ti = list_of
    (&typeid(M))
    (&typeid(N))
    (&typeid(O))
    (&typeid(Q))
    (&typeid(R));
  std::sort (tinfos.begin(), tinfos.end());
  std::sort (exp2ti.begin(), exp2ti.end());
  assert (tinfos == exp2ti);

  SG::BaseInfoBase::castfn_t* castfn;

  M* m0 = new M;
  R* r1 = new R;
  M* m1 = r1;
  assert ((long)m1 != (long)r1);
  M* m2 = (M*)R_C::cast (r1, ClassID_traits<M>::ID());
  assert ((long)m1 == (long)m2);
  m2 = (M*)R_C::cast (r1, typeid (M));
  assert ((long)m1 == (long)m2);
  R* r2 = (R*)R_C::castTo (m1, ClassID_traits<M>::ID());
  assert ((long)r1 == (long)r2);
  r2 = (R*)R_C::castTo (m1, typeid (M));
  assert ((long)r1 == (long)r2);
  r2 = (R*)R_C::castTo (m0, typeid (M));
  assert (r2 == 0);

  castfn = R_C::castfn (ClassID_traits<M>::ID());
  assert ((long)m1 == (long)castfn(r1));
  castfn = R_C::castfn (typeid (M));
  assert ((long)m1 == (long)castfn(r1));
  castfn = R_C::castfnTo (ClassID_traits<M>::ID());
  assert ((long)r1 == (long)castfn(m1));
  castfn = R_C::castfnTo (typeid (M));
  assert ((long)r1 == (long)castfn(m1));
  assert (0 == (long)castfn(m0));

  assert (R_C::cast (r1, ClassID_traits<AA>::ID()) == 0);
  assert (R_C::cast (r1, typeid (AA)) == 0);
  assert (R_C::castTo (r1, ClassID_traits<AA>::ID()) == 0);
  assert (R_C::castTo (r1, typeid (AA)) == 0);
  assert (R_C::castfn (ClassID_traits<AA>::ID()) == 0);
  assert (R_C::castfn (typeid (AA)) == 0);
  assert (R_C::castfnTo (ClassID_traits<AA>::ID()) == 0);
  assert (R_C::castfnTo (typeid (AA)) == 0);

  assert (R_C::is_base (ClassID_traits<M>::ID()));
  assert (R_C::is_base (typeid (M)));

  assert (!R_C::is_base (ClassID_traits<AA>::ID()));
  assert (!R_C::is_base (typeid (AA)));

  assert (CC_C::is_base (ClassID_traits<AA>::ID()));
  assert (CC_C::is_base (typeid (AA)));

  assert (R_C::is_virtual (ClassID_traits<M>::ID()));
  assert (R_C::is_virtual (typeid (M)));

  assert (!CC_C::is_virtual (ClassID_traits<AA>::ID()));
  assert (!CC_C::is_virtual (typeid (AA)));

  typedef SG::BaseInfo<X2> X_C;
  assert (X_C::is_base (typeid (X1)));
  X2* x2 = new X2;
  X1* x1 = (X1*)X_C::cast (x2, typeid (X1));
  assert (x1 == x2);
  X2* x2a = (X2*)X_C::castTo (x1, typeid (X1));
  assert (x2a == 0); // castTo doesn't work now for non-polymorphic classes.

  clids = SG::BaseInfoBase::find (ClassID_traits<CC>::ID())->get_bases();
  std::sort (clids.begin(), clids.end());
  assert (clids == exp1);
  tinfos = SG::BaseInfoBase::find (ClassID_traits<CC>::ID())->get_ti_bases();
  std::sort (tinfos.begin(), tinfos.end());
  assert (tinfos == exp1ti);

  clids = CC_C::baseinfo().get_bases();
  std::sort (clids.begin(), clids.end());
  assert (clids == exp1);
  tinfos = CC_C::baseinfo().get_ti_bases();
  std::sort (tinfos.begin(), tinfos.end());
  assert (tinfos == exp1ti);

  clids = SG::BaseInfoBase::find (typeid (CC))->get_bases();
  std::sort (clids.begin(), clids.end());
  assert (clids == exp1);
  tinfos = SG::BaseInfoBase::find (typeid (CC))->get_ti_bases();
  std::sort (tinfos.begin(), tinfos.end());
  assert (tinfos == exp1ti);

  assert (SG::BaseInfoBase::find (typeid(CC))->clid() ==
          ClassID_traits<CC>::ID());
  assert (SG::BaseInfoBase::find (typeid(CC))->typeinfo() == typeid(CC));

  assert (SG::BaseInfoBase::find (99999) == 0);
  assert (SG::BaseInfoBase::find (typeid(int)) == 0);

  assert (typeid(SG::BaseType<SG::Bases<BB>::bases::Base1>::type) == typeid(AA));
  assert (typeid(SG::BaseType<SG::Bases<BB>::bases::Base1>::is_virtual) ==
          typeid(std::false_type));

  assert (typeid(SG::BaseType<SG::Bases<N>::bases::Base1>::type) == typeid(M));
  assert (typeid(SG::BaseType<SG::Bases<N>::bases::Base1>::is_virtual) ==
          typeid(std::true_type));

  std::vector<CLID> exp3 = list_of
    (ClassID_traits<AA>::ID());
  clids = SG::BaseInfoBase::find (typeid (I1))->get_bases();
  assert (clids == exp3);

  std::vector<const std::type_info*> exp3ti = list_of
    (&typeid (AA))
    (&typeid (I1));
  std::sort (exp3ti.begin(), exp3ti.end());
  tinfos = SG::BaseInfoBase::find (typeid (I1))->get_ti_bases();
  std::sort (tinfos.begin(), tinfos.end());
  assert (tinfos == exp3ti);

  std::vector<CLID> exp4 = list_of
    (ClassID_traits<AA>::ID())
    (ClassID_traits<I2>::ID());
  clids = SG::BaseInfoBase::find (ClassID_traits<I2>::ID())->get_bases();
  std::sort (clids.begin(), clids.end());
  assert (clids == exp4);

  std::vector<const std::type_info*> exp4ti = list_of
    (&typeid (AA))
    (&typeid (I2));
  std::sort (exp4ti.begin(), exp4ti.end());
  tinfos = SG::BaseInfoBase::find (typeid (I2))->get_ti_bases();
  std::sort (tinfos.begin(), tinfos.end());
  assert (tinfos == exp4ti);

  const SG::BaseInfoBase* jbib = SG::BaseInfoBase::find (typeid (J));
  tinfos = jbib->get_ti_bases();
  std::vector<const std::type_info*> exp5ti = list_of
    (&typeid (AA))
    (&typeid (X1))
    (&typeid (M))
    (&typeid (J));
  std::sort (exp5ti.begin(), exp5ti.end());
  std::sort (tinfos.begin(), tinfos.end());
  assert (tinfos == exp5ti);
  assert (!jbib->is_virtual (typeid (AA)));
  assert ( jbib->is_virtual (typeid (X1)));
  assert (!jbib->is_virtual (typeid (M)));

  const SG::BaseInfoBase* kbib = SG::BaseInfoBase::find (typeid (K));
  tinfos = kbib->get_ti_bases();
  std::vector<const std::type_info*> exp6ti = list_of
    (&typeid (AA))
    (&typeid (K));
  std::sort (exp6ti.begin(), exp6ti.end());
  std::sort (tinfos.begin(), tinfos.end());
  assert (tinfos == exp6ti);
    
  return 0;
}


class KCopyConversion
  : public SG::CopyConversion<K, M>
{
public:
  void convert (const K& src, M& dst) const
  { dst.x = src.x; }
};

SG_ADD_COPY_CONVERSION(K, KCopyConversion);


// Test copying conversions.
void test2()
{
  std::cout << "test2\n";
  const SG::BaseInfoBase& bib = SG::BaseInfo<K>::baseinfo();
  std::vector<CLID> clids = bib.get_copy_conversions();
  std::vector<CLID> exp1 = list_of
    (ClassID_traits<M>::ID());
  assert (clids == exp1);

  const SG::CopyConversionBase* b = bib.copy_conversion (clids[0]);
  assert (b == bib.copy_conversion (typeid (M)));
  assert (0 == bib.copy_conversion (typeid (X1)));
  assert (0 == bib.copy_conversion (typeid (Q)));
  assert (0 == bib.copy_conversion (ClassID_traits<Q>::ID()));

  K k;
  k.x = 10;
  M m;
  m.x = 0;
  b->convertUntyped (&k, &m);
  assert (m.x == 10);
}


int main()
{
  test1();
  test2();
  return 0;
}

