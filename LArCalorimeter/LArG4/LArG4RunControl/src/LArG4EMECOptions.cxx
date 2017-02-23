/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4RunControl/LArG4EMECOptions.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

void LArG4EMECOptions::saveMe()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore",
                                        "LArG4EMECOptions::saveMe");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->record(this,"LArG4EMECOptions").isFailure())
      std::cout << "Can not record LArG4EMECOptions" << std::endl;
  }
}

void LArG4EMECOptions::printMe()
{
  std::cout << " *** *** This is the object of type LArG4EMECOptions *** *** \n";
  std::string x;
  if(m_EMECECorrType == NONE)   x="NONE(=0)";
  if(m_EMECECorrType == CHCOLL) x="CHCOLL(=1)";
  if(m_EMECECorrType == GAPADJ) x="GAPADJ(=3)";
  if(m_EMECECorrType == GAPOLD) x="GAPOLD(=2";
  if(m_EMECECorrType == GAP_E)  x="GAP_E(=4)";
  if(m_EMECECorrType == GAP_S)  x="GAP_S(=5)";
  if(m_EMECECorrType == GAP_SE) x="GAP_SE(6)";
  if(m_EMECECorrType == CHCOLL1) x="CHCOLL1(7)"; 
  std::cout << " ** EMECECorrType       = " << x                     << "\n";
  std::cout << " ** EMECGapPower        = " << m_EMECGapPower        << "\n";
  std::cout << " ** EMECChMap           = " << m_EMECChMap           << "\n";
  std::cout << " ** EMECHVMap           = " << m_EMECHVMap           << "\n";
  std::cout << " ** EMECEsr             = " << m_EMECEsr             << "\n";
#ifdef LARWHEELCALCULATOR_PSA_DEVELOPMENT
  std::cout << " ** EMECFoldA           = " << m_EMECFoldA           << "\n";
#endif
  std::cout << " ** EMECBirksLaw        = " << (m_EMECBirksLaw ? "true" : "false") << "\n";
   std::cout << "** EMECBirksk          = " << m_EMECBirksk << "\n *** *** \n";
   std::cout << "** EMECHVEnable        = " << m_EMECHVEnable << "\n *** *** \n";


  //  std::cout << " ** EMECPhiRotation     = " << m_EMECPhiRotation     << "\n";
  //std::cout << " ** EMECSagging         = " << m_EMECSagging         << "\n";
  //std::cout << " ** EMECInnerSlantParam = " << m_EMECInnerSlantParam << "\n";
  //std::cout << " ** EMECOuterSlantParam = " << m_EMECOuterSlantParam << "\n *** *** \n";
}
