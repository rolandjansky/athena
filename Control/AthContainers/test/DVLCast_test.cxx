/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DVLCast_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file DVLCast_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2010
 * @brief Regression tests for DVLCast.
 */


#undef NDEBUG
#include "AthContainers/tools/DVLCast.h"
#include <cassert>


struct X1
{
  X1(int the_x) : x (the_x) {}
  int x;
};

struct X2 : public X1
{
  X2(int the_y) : X1(the_y+1), y(the_y) {}
  int y;
};

struct X3 : public X2
{
  X3(int the_z) : X2(the_z+1), z(the_z) {}
  int z;
};


struct Y1
{
  Y1(int the_x) : x (the_x) {}
  int x;
};

struct Y2 : virtual public Y1
{
  Y2(int the_y) : Y1(the_y+1), y(the_y) {}
  int y;
};

struct Y3 : virtual public Y2
{
  Y3(int the_z) : Y1(the_z+2), Y2(the_z+1), z(the_z) {}
  int z;
};



struct Test1
{
  typedef ::X1 B;
  typedef ::X2 D;
  typedef ::X3 DD;
  typedef D base_value_type;
  static const bool has_virtual = false;
};

  
struct Test2
{
  typedef ::Y1 B;
  typedef ::Y2 D;
  typedef ::Y3 DD;
  typedef D base_value_type;
  static const bool has_virtual = true;
};

  
template <class T>
void tester()
{
  typedef typename T::B B;
  typedef typename T::D D;
  typedef typename T::DD DD;
  typedef typename DataModel_detail::DVLCast<T> Cast;

  D d(1);
  DD dd(2);
  B* b = &d;
  assert (Cast::cast (b) == &d);
  const B* cb = &d;
  assert (Cast::cast (cb) == &d);

  b = &dd;
  D* dd2 = &dd;
  assert (Cast::cast (b) == dd2);
  cb = &dd;
  assert (Cast::cast (cb) == dd2);
}


int main()
{
  tester<Test1>();
  return 0;
}
