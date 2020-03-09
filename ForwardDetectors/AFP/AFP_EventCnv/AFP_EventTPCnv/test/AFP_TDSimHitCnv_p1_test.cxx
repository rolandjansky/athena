/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AFP_EventTPCnv/test/AFP_TDSimHitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for AFP_TDSimHitCnv_p1.
 */


#undef NDEBUG
#include "AFP_EventTPCnv/AFP_TDSimHitCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const AFP_TDSimHit& p1,
              const AFP_TDSimHit& p2)
{
  assert ( p1.m_nHitID              == p2.m_nHitID );
  assert ( p1.m_nTrackID            == p2.m_nTrackID );
  assert ( p1.m_nParticleEncoding   == p2.m_nParticleEncoding );
  assert ( p1.m_fKineticEnergy      == p2.m_fKineticEnergy );
  assert ( p1.m_fEnergyDeposit      == p2.m_fEnergyDeposit );
  assert ( p1.m_fWaveLength         == p2.m_fWaveLength );
  assert ( p1.m_fPreStepX           == p2.m_fPreStepX );
  assert ( p1.m_fPreStepY           == p2.m_fPreStepY );
  assert ( p1.m_fPreStepZ           == p2.m_fPreStepZ );
  assert ( p1.m_fPostStepX          == p2.m_fPostStepX );
  assert ( p1.m_fPostStepY          == p2.m_fPostStepY );
  assert ( p1.m_fPostStepZ          == p2.m_fPostStepZ );
  assert ( p1.m_fGlobalTime         == p2.m_fGlobalTime );
  assert ( p1.m_nStationID          == p2.m_nStationID );
  assert ( p1.m_nDetectorID         == p2.m_nDetectorID );
  assert ( p1.m_nSensitiveElementID == p2.m_nSensitiveElementID );
}


void testit (const AFP_TDSimHit& trans1)
{
  MsgStream log (0, "test");
  AFP_TDSimHitCnv_p1 cnv;
  AFP_TDSimHit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  AFP_TDSimHit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  AFP_TDSimHit trans1;
  trans1.m_nHitID = 1;
  trans1.m_nTrackID = 2;
  trans1.m_nParticleEncoding = 3;
  trans1.m_fKineticEnergy = 4.5;
  trans1.m_fEnergyDeposit = 5.5;
  trans1.m_fWaveLength = 6.5;
  trans1.m_fPreStepX = 7.5;
  trans1.m_fPreStepY = 8.5;
  trans1.m_fPreStepZ = 9.5;
  trans1.m_fPostStepX = 10.5;
  trans1.m_fPostStepY = 11.5;
  trans1.m_fPostStepZ = 12.5;
  trans1.m_fGlobalTime = 13.5;
  trans1.m_nStationID = 14;
  trans1.m_nDetectorID = 15;
  trans1.m_nSensitiveElementID = 16;

  testit (trans1);
}


int main()
{
  std::cout << "AFP_EventTPCnv/AFP_TDSimHitCnv_p1_test\n";
  test1();
  return 0;
}
