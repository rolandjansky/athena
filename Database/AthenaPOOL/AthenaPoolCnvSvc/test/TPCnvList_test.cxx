/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaPoolCnvSvc/test/TPCnvList_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TPCnvList.
 */


#undef NDEBUG
#include "AthenaPoolCnvSvc/TPCnvList.h"
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


class XCnv_p2
  : public TestConverterBase
{
public:
  typedef X Trans_t;
  typedef X_p2 Pers_t;
  
  X* createTransientWithKey (const X_p2* pers, const std::string&, MsgStream& log)
  {
    return createTransient (pers, log);
  }
  X* createTransient (const X_p2* pers, MsgStream&)
  { return new X(pers->m_a*3); }

  void persToTrans (const X_p2* pers, X* trans, MsgStream&)
  {
    trans->m_a = pers->m_a*3;
  }
};


const std::string X_guid = "CAE53A87-64AD-4576-A203-1A4142E1E10F";
const std::string X_p1_guid = "6AD63B61-BE75-40FC-B0C6-DD3C7801D871";
const std::string X_p2_guid = "0AAC9C99-726D-4CF4-B9F9-00B6674C57DD";


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
  MsgStream msg (nullptr, "");

  AthenaPoolCnvSvc::TPCnvList<TestConverter, X,
                              XCnv_p2,
                              XCnv_p1,
                              T_TPCnvNull<X> > tpcnv;

  X x2(0);

  TestConverter cnv1 (X_p1_guid);
  {
    std::unique_ptr<X> xptr = tpcnv.createTransient (cnv1, "key", msg);
    assert (xptr->m_a == 20);
  }

  assert (tpcnv.persToTrans (cnv1, &x2, "key", msg));
  assert (x2.m_a == 20);

  TestConverter cnv2 (X_p2_guid);
  {
    std::unique_ptr<X> xptr = tpcnv.createTransient (cnv2, "key", msg);
    assert (xptr->m_a == 30);
  }

  assert (tpcnv.persToTrans (cnv2, &x2, "key", msg));
  assert (x2.m_a == 30);

  TestConverter cnv0 (X_guid);
  {
    std::unique_ptr<X> xptr = tpcnv.createTransient (cnv0, "key", msg);
    assert (xptr->m_a == 10);
  }

  assert (tpcnv.persToTrans (cnv0, &x2, "key", msg));
  assert (x2.m_a == 10);

  TestConverter cnvx ("B3FCEE90-66FC-4AE1-A81B-5257A0306EF8");
  assert (tpcnv.createTransient (cnvx, "key", msg) == nullptr);
  assert (!tpcnv.persToTrans (cnvx, &x2, "key", msg));
}


int main()
{
  gSystem->Load("libAthenaPoolCnvSvcTestDict");
  test1();
  return 0;
}
