/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/MyVec.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Test class for D3PD maker.
 */


#include "D3PDMakerTest/MyVec.h"
#include "TClass.h"


namespace D3PDTest {


//***********************************************************************
//  Dummy methods required by root.
//


TClass* MyVec::Class()
{
  if (!fgIsA)
    fgIsA = TClass::GetClass ("D3PDTest::MyVec");
  return fgIsA;
}


#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
void MyVec::ShowMembers (TMemberInspector& R__insp)
{
  TLorentzVector::ShowMembers (R__insp);
}
#endif


void MyVec::Streamer (TBuffer& b)
{
  TLorentzVector::Streamer (b);
}


#if ROOT_VERSION_CODE >= ROOT_VERSION(6,1,0) || (ROOT_VERSION_CODE>=ROOT_VERSION(5,34,22) && ROOT_VERSION_CODE<ROOT_VERSION(6,0,0))
atomic_TClass_ptr MyVec::fgIsA;
#else
TClass* MyVec::fgIsA = 0;
#endif


} // namespace D3PDTest
