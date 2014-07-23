/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/CaloJetInput.h"

#include <iostream>

using namespace std;

TrigConf::CaloJetInput::CaloJetInput( const std::string& name,
                                      unsigned int version,
                                      const std::string& type,
                                      int ptcut,
                                      int phimin,
                                      int phimax,
                                      int etamin,
                                      int etamax) : 
   L1DataBaseclass(),
   m_Type( type ),
   m_Ptcut( ptcut ),
   m_PhiMin(phimin),
   m_PhiMax(phimax),
   m_EtaMin(etamin),
   m_EtaMax(etamax)
{
   setName(name);
   setVersion(version);
}

TrigConf::CaloJetInput::CaloJetInput() :
   L1DataBaseclass()
{}

TrigConf::CaloJetInput::~CaloJetInput()
{}  
 

void
TrigConf::CaloJetInput::print(const std::string& indent, unsigned int /*detail*/) const {

   cout << indent << "-----------------------" << endl; 
   cout << indent << "id: " << id() << " Name: " << name() << " Version: " << version() << endl;
   cout << indent <<  "Type: "<< m_Type << " ptcut: " << m_Ptcut << " phi_min: " << m_PhiMin 
        << " phi_max: " << m_PhiMax << " eta_min: " << m_EtaMin 
        << " eta_max: " << m_EtaMax << endl;
}
