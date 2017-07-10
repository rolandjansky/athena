/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SimHelpers/ProcessSubTypeMap.h"

ProcessSubTypeMap::ProcessSubTypeMap()
{;}

ProcessSubTypeMap::~ProcessSubTypeMap()
{;}

G4int ProcessSubTypeMap::GetProcessSubType(const G4VProcess* pProc)
{
  std::map<const G4VProcess*,G4int>::iterator itr = m_map.find(pProc);

  G4int subType = -1;
  if(itr!=m_map.end())
  { subType = itr->second; }
  else
  {
    G4String procName = pProc->GetProcessName();
    if(procName=="Decay") // in DecayStrategy
    { subType = DECAY; }
    else if(procName=="eBrem" || procName=="muBrems") // in BremsstrahlungStrategy
    { subType = fBremsstrahlung; }
    else if(procName=="compt") // in ComptonStrategy
    { subType = fComptonScattering; }
    else if(procName=="conv") // in ConversionStrategy
    { subType = fGammaConversion; }
    else if(procName=="eIoni" || procName=="muIoni" || procName=="hIoni") // in IonizationStrategy
    { subType = fIonisation; }
    else if(procName=="Cerenkov") // in LUCID_SensitiveDetector
    { subType = fCerenkov; }

    m_map[pProc] = subType;
  }

  return subType;
}


