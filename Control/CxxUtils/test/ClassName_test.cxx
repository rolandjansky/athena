/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/test/ClassName_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Regression tests for ClassName.
 */


#undef NDEBUG
#include "CxxUtils/ClassName.h"
#include <iostream>
#include <cassert>


#include "expect_exception.icc"


using CxxUtils::ClassName;


void test1()
{
  std::cout << "test1\n";
  
  {
    ClassName cn ("Foo");
    assert (cn.name() == "Foo");
    assert (cn.qualifiedName() == "Foo");
    assert (cn.fullName() == "Foo");
    assert (cn.isConst() == false);
  }

  {
    ClassName cn ("const Foo");
    assert (cn.name() == "Foo");
    assert (cn.qualifiedName() == "Foo");
    assert (cn.fullName() == "const Foo");
    assert (cn.isConst() == true);
  }

  {
    ClassName cn ("Foo const");
    assert (cn.name() == "Foo");
    assert (cn.qualifiedName() == "Foo");
    assert (cn.fullName() == "const Foo");
    assert (cn.isConst() == true);
  }

  {
    ClassName cn ("Foo::Bar::Fee");
    assert (cn.name() == "Fee");
    assert (cn.qualifiedName() == "Foo::Bar::Fee");
    assert (cn.fullName() == "Foo::Bar::Fee");
    assert (cn.isConst() == false);
  }


  {
    ClassName cn ("Foo<Bar>");
    assert (cn.name() == "Foo");
    assert (cn.qualifiedName() == "Foo");
    assert (cn.fullName() == "Foo<Bar>");
    assert (cn.isConst() == false);
  }

  {
    ClassName cn ("Foo<Bar,Fee>");
    assert (cn.name() == "Foo");
    assert (cn.qualifiedName() == "Foo");
    assert (cn.fullName() == "Foo<Bar,Fee>");
    assert (cn.isConst() == false);
  }

  {
    ClassName cn ("Foo<Bar const,Fee>");
    assert (cn.name() == "Foo");
    assert (cn.qualifiedName() == "Foo");
    assert (cn.fullName() == "Foo<const Bar,Fee>");
    assert (cn.isConst() == false);
  }

  {
    ClassName cn ("Foo<Bar const,Fee> const");
    assert (cn.name() == "Foo");
    assert (cn.qualifiedName() == "Foo");
    assert (cn.fullName() == "const Foo<const Bar,Fee>");
    assert (cn.isConst() == true);
  }

  {
    ClassName cn ("A::B<C>::Foo<Bar,D<E> >");
    assert (cn.name() == "Foo");
    assert (cn.qualifiedName() == "A::B<C>::Foo");
    assert (cn.fullName() == "A::B<C>::Foo<Bar,D<E> >");
    assert (cn.isConst() == false);
  }

  {
    ClassName cn ("A<S<T> const>");
    assert (cn.name() == "A");
    assert (cn.qualifiedName() == "A");
    assert (cn.fullName() == "A<const S<T> >");
    assert (cn.isConst() == false);
  }

  {
    ClassName cn ("const std::foo");
    assert (cn.name() == "foo");
    assert (cn.qualifiedName() == "std::foo");
  }

  EXPECT_EXCEPTION (ClassName::ExcBadClassName, ClassName cn ("A>B"));
}


void test_eq()
{
  std::cout << "test_eq\n";

  assert (ClassName ("A::B<int>") == ClassName ("A::B<int>"));
  assert (ClassName ("A::B<int>") != ClassName ("X::B<int>"));
  assert (ClassName ("A::B<int>") != ClassName ("A::B<i,nt>"));
  assert (ClassName ("A::B<int>") != ClassName ("A::B<int>::C"));

  assert (ClassName ("A::B<int>") != ClassName ("A::B<const int>"));
  assert (ClassName ("A::B<const int>") == ClassName ("A::B<const int>"));
}


void test_match()
{
  std::cout << "test_match\n";

  ClassName pat1 ("A::B<int>");
  ClassName pat2 ("std::vector<$T, std::allocator<$T> >");

  ClassName::match_t matches;
  assert (pat1.match (pat1, matches));
  assert (matches.size() == 0);
  assert (!ClassName ("A::B<float>").match (pat1, matches));

  assert (ClassName ("std::vector<int, std::allocator<int> >").match
          (pat2,  matches));
  assert (matches.size() == 1);
  assert (matches["T"].fullName() == "int");

  assert (ClassName ("std::vector<A::B<C>, std::allocator<A::B<C> > >").match
          (pat2,  matches));
  assert (matches.size() == 1);
  assert (matches["T"].fullName() == "A::B<C>");

  ClassName pat3 ("A::B<$T>");
  ClassName pat4 ("A::B<const $T>");

  assert (ClassName ("A::B<int>").match (pat3, matches));
  assert (matches["T"].fullName() == "int");
  assert (ClassName ("A::B<const int>").match (pat3, matches));
  assert (matches["T"].fullName() == "const int");
  assert (!ClassName ("A::B<int>").match (pat4, matches));
  assert (ClassName ("A::B<const int>").match (pat4, matches));
  assert (matches["T"].fullName() == "int");

  ClassName pat5 ("A<$T>");
  assert (ClassName ("const A<int>").match (pat5, matches));
}


void test_subst()
{
  std::cout << "test_subst\n";

  ClassName pat1 ("std::vector<$T, std::allocator<$T> >");
  ClassName rep1 ("std::vector<$T>");

  ClassName cn1 ("std::vector<C::A<B>, std::allocator<C::A<B> > >");

  ClassName::match_t matches;
  assert (cn1.match (pat1, matches));
  ClassName cn1rep = rep1.substCopy (matches);
  assert (cn1rep.fullName() == "std::vector<C::A<B> >");

  matches.clear();
  matches["T"] = ClassName ("const int");
  ClassName cn2 ("Foo<$T>");
  cn2.subst (matches);
  assert (cn2.fullName() == "Foo<const int>");
  ClassName cn3 ("Foo<const $T>");
  cn3.subst (matches);
  assert (cn3.fullName() == "Foo<const int>");
  ClassName cn4 ("Foo<const $T>");
  matches["T"] = ClassName ("int");
  cn4.subst (matches);
  assert (cn4.fullName() == "Foo<const int>");

  ClassName cn5 ("Foo<$U>");
  EXPECT_EXCEPTION (ClassName::ExcMissingVariable, cn5.subst (matches));
}


void test_rules()
{
  std::cout << "test_rules\n";

  ClassName::Rules rules;

  rules.add ("std::vector<$T, std::allocator<$T> >",
             "std::vector<$T>");
  rules.add ("std::vector<Foo>", "std::vector<Bar>");
  rules.add ("std::map<$K,$V, std::less<$K>, std::allocator<std::pair<const $K,$V> > >",
             "std::map<$K,$V>");
  rules.add ("DataVector<$T, $B>", "DataVector<$T>");
  rules.add ("std::__1", "std");

  assert (rules.size() == 5);
  assert (rules.apply ("std::__1::foo") ==
          "std::foo");

  assert (rules.apply ("std::__1::vector<std::__1::vector<int, std::__1::allocator<int> >, std::__1::allocator<std::__1::vector<int, std::__1::allocator<int> > > >") ==
          "std::vector<std::vector<int> >");

  assert (rules.apply ("std::map<int, float, std::less<int>, std::allocator<std::pair<const int, float> > >") ==
          "std::map<int,float>");

  assert (rules.apply ("DataVector<Foo, DataModel_detail::NoBase>") ==
          "DataVector<Foo>");

  assert (rules.apply ("std::vector<Foo, std::allocator<Foo> >") ==
          "std::vector<Bar>");

  assert (rules.apply ("const std::vector<Foo, std::allocator<Foo> >") ==
          "const std::vector<Bar>");
}


int main()
{
  test1();
  test_eq();
  test_match();
  test_subst();
  test_rules();
  return 0;
}
