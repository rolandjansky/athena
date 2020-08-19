/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  AthenaKernel/test/Bases_test.cxx
 * @author scott snyder
 * @date Aug, 2020
 * @brief Regression test for Bases.
 */


#undef NDEBUG

#include "AthenaKernel/Bases.h"
#include <type_traits>
#include <typeinfo>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>


class A
{
public:
  static const char* name() { return "A"; }
};
class B
{
public:
  static const char* name() { return "B"; }
};
class C : public A, public B
{
public:
  static const char* name() { return "C"; }
};


class D : public C
{
public:
  static const char* name() { return "C"; }
};

class F
{
public:
  static const char* name() { return "F"; }
};

class E : virtual public C, public F
{
public:
  static const char* name() { return "E"; }
};


template <class T>
struct name
{
  static const char* value() { return T::name(); }
};
template <>
struct name<void>
{
  static const char* value() { return "void"; }
};


namespace SG {

template <>
class Bases<C>
{
public:
  using bases = BaseList<A, B>;
};


template <>
class Bases<D>
{
public:
  using bases = BaseList<C>;
};

template <>
class Bases<E>
{
public:
  using bases = BaseList<Virtual<C>, F>;
};

} // namespace SG


void test1()
{
  std::cout << "test1\n";

  std::vector<std::string> names;

  auto collect_names = [&] (auto* p, bool is_virtual)
  {
    using base_t = std::remove_pointer_t<decltype(p)>;
    names.push_back (name<base_t>::value());
    if (is_virtual) names.push_back ("(virtual)");
    return false;
  };

  assert (SG::Bases<C>::bases::foreach_ (collect_names) == false);
  assert (names == (std::vector<std::string> { "A", "B" }));

  names.clear();
  assert (SG::Bases<D>::bases::foreach_ (collect_names) == false);
  assert (names == (std::vector<std::string> { "C", "A", "B" }));

  names.clear();
  assert (SG::Bases<E>::bases::foreach_ (collect_names) == false);
  assert (names == (std::vector<std::string> { "C", "(virtual)",
                                                "A", "(virtual)",
                                                "B", "(virtual)",
                                                "F"}));

  auto collect_names2 = [&] (auto* p, bool is_virtual)
  {
    using base_t = std::remove_pointer_t<decltype(p)>;
    const char* nm = name<base_t>::value();
    names.push_back (nm);
    if (is_virtual) names.push_back ("(virtual)");
    if (std::string(nm) == "A") return nm;
    return static_cast<const char*> (nullptr);
  };

  names.clear();
  const char* nm = SG::Bases<D>::bases::foreach_ (collect_names2);
  assert (std::string(nm) == "A");
  assert (names == (std::vector<std::string> { "C", "A" }));
}


template <class T>
bool derivesFrom (const std::type_info& ti)
{
  // Check if T itself is ti.
  if (typeid(T) == ti) return true;

  auto search = [&] (auto* p, bool is_virtual [[maybe_unused]])
  {
    using base_t = std::remove_pointer_t<decltype(p)>;
    if (typeid(base_t) == ti) return true;
    return false;
  };
  return SG::Bases<T>::bases::foreach_ (search);
}


void test2()
{
  std::cout << "test2\n";
  std::cout << derivesFrom<C> (typeid(A)) << "\n";
  std::cout << derivesFrom<C> (typeid(D)) << "\n";
}


int main()
{
  std::cout << "AthenaKernel/Bases_test\n";
  test1();
  test2();
  return 0;
}
