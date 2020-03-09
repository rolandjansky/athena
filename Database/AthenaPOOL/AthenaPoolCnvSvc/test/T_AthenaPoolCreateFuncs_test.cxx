/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaPoolCnvSvc/test/T_AthenaPoolCreateFuncs_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Tests for T_AthenaPoolCreateFuncs_test.
 */


#undef NDEBUG
#include "AthenaPoolCnvSvc/T_AthenaPoolCreateFuncs.h"
#include "GaudiKernel/MsgStream.h"
#include "TestConverterBase.h"
#include <iostream>
#include <cassert>


struct X
{
  X() {}
  X (int x, const std::string& key) : m_x(x), m_key(key) {}
  int m_x = 0;
  std::string m_key;
};
struct X_p1
{
  X_p1() {}
  X_p1 (int x, const std::string& key) : m_x(x), m_key(key) {}
  int m_x = 0;
  std::string m_key;
};


class TPCnv1
  : public TestConverterBase
{
public:
  typedef X Trans_t;
  typedef X_p1 Pers_t;

  X_p1* createPersistentWithKey (const X* x, const std::string& key, MsgStream&)
  {
    return new X_p1 (x->m_x, key);
  }


  X* createTransientWithKey (const X_p1* xp, const std::string& key, MsgStream&)
  {
    return new X (xp->m_x, key);
  }
};


class TPCnv2
{
public:
  typedef X Trans_t;
  typedef X_p1 Pers_t;

  void transToPers (const X* x, X_p1* xp, MsgStream&)
  {
    xp->m_x   = x->m_x;
    xp->m_key = x->m_key;
  }

  void persToTrans (const X_p1* xp, X* x, MsgStream&)
  {
    x->m_x   = xp->m_x;
    x->m_key = xp->m_key;
  }
};


void test1()
{
  std::cout << "test1\n";
  MsgStream msg (nullptr, "");
  X x (1, "x");
  X_p1 xp (2, "xp");
  TPCnv1 cnv1;
  {
    std::unique_ptr<X_p1> xp_ptr =
      AthenaPoolCnvSvc::createPersistent (cnv1, &x, "key", msg);
    assert (xp_ptr->m_x == 1);
    assert (xp_ptr->m_key == "key");
  }

  {
    std::unique_ptr<X> x_ptr =
      AthenaPoolCnvSvc::createTransient (cnv1, &xp, "key", msg);
    assert (x_ptr->m_x == 2);
    assert (x_ptr->m_key == "key");
  }

  TPCnv2 cnv2;
  {
    std::unique_ptr<X_p1> xp_ptr =
      AthenaPoolCnvSvc::createPersistent (cnv2, &x, "key", msg);
    assert (xp_ptr->m_x == 1);
    assert (xp_ptr->m_key == "x");
  }

  {
    std::unique_ptr<X> x_ptr =
      AthenaPoolCnvSvc::createTransient (cnv2, &xp, "key", msg);
    assert (x_ptr->m_x == 2);
    assert (x_ptr->m_key == "xp");
  }
}


int main()
{
  std::cout << "AthenaPoolCnvSvc/T_AthenaPoolCreateFuncs_test\n";
  test1();
  return 0;
}

