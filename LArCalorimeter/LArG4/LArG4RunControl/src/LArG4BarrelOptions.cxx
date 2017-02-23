/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4RunControl/LArG4BarrelOptions.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"


void LArG4BarrelOptions::saveMe()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore",
                                        "LArG4BarrelOptions::saveMe");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->record(this,"LArG4BarrelOptions").isFailure())
      std::cout << "Can not record LArG4BarrelOptions" << std::endl;
  }
}

void LArG4BarrelOptions::printMe()
{
  std::cout << " *** *** This is the object of type LArG4BarrelOptions *** *** \n";
  std::cout << " ** EMBCurr = " << (m_EMBCurr ? "true" : "false") << "\n";
  std::cout << " ** EMBEtaTrans = " << (m_EMBEtaTrans ? "true" : "false") << "\n";
  std::cout << " ** EMBXtalk = " << (m_EMBXtalk ? "true" : "false") << "\n";
  std::cout << " ** EMBTimeCurr = " << (m_EMBTimeCurr ? "true" : "false") << "\n";
  std::cout << " ** EMBTimeShower = " << (m_EMBTimeShower ? "true" : "false") << "\n";
  std::cout << " ** EMBTimeProp = " << (m_EMBTimeProp ? "true" : "false") << "\n";
  std::cout << " ** EMBdstep  = " << m_EMBdstep << "\n *** *** \n";
  std::cout << " ** EMBBirksLaw = " << (m_EMBBirksLaw ? "true" : "false") << "\n";
  std::cout << "** EMBBirksk  = " << m_EMBBirksk << "\n *** *** \n";
  std::cout << "** EMBHVEnable  = " << m_EMBHVEnable << "\n *** *** \n";
}
