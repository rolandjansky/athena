/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaPoolCnvSvc/test/T_AuxContainerCopyTPCnv_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2019
 * @brief Test for T_AuxContainerCopyTPCnv.
 */


#undef NDEBUG
#include "AthenaPoolCnvSvc/T_AuxContainerCopyTPCnv.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "TestTools/expect_exception.h"
#include <iostream>
#include <cassert>


class NewData
  : public SG::AuxStoreInternal
{
public:
  NewData();


  SG::auxid_t m_i1_auxid;
  SG::auxid_t m_i2_auxid;
  SG::auxid_t m_i3_auxid;
};


NewData::NewData()
  : m_i1_auxid (SG::AuxTypeRegistry::instance().getAuxID<int> ("i1")),
    m_i2_auxid (SG::AuxTypeRegistry::instance().getAuxID<int> ("i2")),
    m_i3_auxid (SG::AuxTypeRegistry::instance().getAuxID<int> ("i3"))
{
  getData (m_i1_auxid, 0, 0);
  getData (m_i2_auxid, 0, 0);
  getData (m_i3_auxid, 0, 0);
}


class OldData
  : public SG::AuxStoreInternal
{
public:
  OldData();
  OldData (const OldData&) = default;
  OldData& operator= (const OldData&) { return *this; }


  SG::auxid_t m_i1_auxid;
  SG::auxid_t m_i2_auxid;
};


OldData::OldData()
  : m_i1_auxid (SG::AuxTypeRegistry::instance().getAuxID<int> ("i1")),
    m_i2_auxid (SG::AuxTypeRegistry::instance().getAuxID<int> ("i2"))
{
  getData (m_i1_auxid, 0, 0);
  getData (m_i2_auxid, 0, 0);
}




void test1()
{
  std::cout << "test1\n";

  OldData olddata;
  olddata.resize (3);
  int* old_i1 = reinterpret_cast<int*> (olddata.getData (olddata.m_i1_auxid, 3, 3));
  int* old_i2 = reinterpret_cast<int*> (olddata.getData (olddata.m_i2_auxid, 3, 3));
  for (int i=0; i < 3; i++) {
    old_i1[i] = i+10;
    old_i2[i] = i+20;
  }

  MsgStream log (nullptr, "test");
  NewData newdata;
  T_AuxContainerCopyTPCnv<NewData, OldData> cnv;
  cnv.persToTrans (&olddata, &newdata, log);

  assert (newdata.size() == 3);
  int* new_i1 = reinterpret_cast<int*> (newdata.getData (newdata.m_i1_auxid, 3, 3));
  int* new_i2 = reinterpret_cast<int*> (newdata.getData (newdata.m_i2_auxid, 3, 3));
  int* new_i3 = reinterpret_cast<int*> (newdata.getData (newdata.m_i3_auxid, 3, 3));
  for (int i=0; i < 3; i++) {
    assert (new_i1[i] == i+10);
    assert (new_i2[i] == i+20);
    assert (new_i3[i] == 0);
  }

  EXPECT_EXCEPTION( std::runtime_error,
                    cnv.transToPers (&newdata, &olddata, log) );
}


int main()
{
  std::cout << "AthenaPoolCnvSvc/T_AuxContainerCopyTPCnv_test\n";
  test1();
  return 0;
}
