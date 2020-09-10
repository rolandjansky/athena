/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaPoolCnvSvc/test/TPCnvElt_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TPCnvElt.
 */


#undef NDEBUG
#include "AthenaPoolCnvSvc/TPCnvElt.h"
#include "AthenaPoolCnvSvcTestDict.h"
#include "GaudiKernel/MsgStream.h"
#include "TSystem.h"
#include "TestConverterBase.h"
#include <iostream>
#include <typeinfo>
#include <cassert>


using namespace AthenaPoolCnvSvcTest;


class XCnv_p1
  : public TestConverterBase
{
public:
  typedef X Trans_t;
  typedef X_p1 Pers_t;
  
  X* createTransientWithKey (const X_p1* pers, const std::string&, MsgStream& log)
  {
    return createTransient (pers, log);
  }
  X* createTransient (const X_p1* pers, MsgStream&)
  { return new X(pers->m_a*2); }

  void persToTrans (const X_p1* pers, X* trans, MsgStream&)
  {
    trans->m_a = pers->m_a*2;
  }
};


const std::string X_guid = "CAE53A87-64AD-4576-A203-1A4142E1E10F";
const std::string X_p1_guid = "6AD63B61-BE75-40FC-B0C6-DD3C7801D871";


class TestConverter
{
public:
  TestConverter (const std::string& guid) : m_guid (guid) {}
  
  bool compareClassGuid (const Guid& guid)
  { return guid == m_guid; }

  template <class T>
  T* poolReadObject() { return new T (10); }
  

private:
  Guid m_guid;
};


void test1()
{
  std::cout << "test1\n";

  assert (AthenaPoolCnvSvc::guidFromTypeinfo (typeid (X_p1)) == Guid (X_p1_guid));
}


void test2()
{
  std::cout << "test2\n";
  MsgStream msg (nullptr, "");

  AthenaPoolCnvSvc::TPCnvElt<TestConverter, XCnv_p1> tpcnv;

  TestConverter cnv1 (X_p1_guid);
  {
    std::unique_ptr<X> xptr = tpcnv.createTransient (cnv1, "key", msg);
    assert (xptr->m_a == 20);
  }

  X x2 (0);
  assert (tpcnv.persToTrans (cnv1, &x2, "key", msg));
  assert (x2.m_a == 20);
  
  TestConverter cnv2 ("8ADE9DD7-ED1F-447E-A096-0F8F786291CD");
  assert (tpcnv.createTransient (cnv2, "key", msg) == nullptr);
  assert (!tpcnv.persToTrans (cnv2, &x2, "key", msg));

  AthenaPoolCnvSvc::TPCnvElt<TestConverter, T_TPCnvNull<X> > tpcnv_null;
  TestConverter cnv3 (X_guid);
  {
    std::unique_ptr<X> xptr = tpcnv_null.createTransient (cnv3, "key", msg);
    assert (xptr->m_a == 10);
  }
  assert (tpcnv_null.createTransient (cnv1, "key", msg) == nullptr);

  assert (tpcnv_null.persToTrans (cnv3, &x2, "key", msg));
  assert (x2.m_a == 10);
  assert (!tpcnv_null.persToTrans (cnv1, &x2, "key", msg));
}


int main()
{
  gSystem->Load("libAthenaPoolCnvSvcTestDict");
  test1();
  test2();
  return 0;
}


