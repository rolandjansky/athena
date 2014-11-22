/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/CaloSinCos.h"

#include <iostream>

using namespace std;

TrigConf::CaloSinCos::CaloSinCos( const std::string& n, const int v,
                                  int val1, int val2, int val3, int val4, 
                                  int val5, int val6, int val7, int val8,
                                  int phimin, int phimax,int etamin,int etamax)
   : L1DataBaseclass(), 
     m_Val1( val1 ), 
     m_Val2( val2 ), 
     m_Val3( val3 ), 
     m_Val4( val4 ), 
     m_Val5( val5 ), 
     m_Val6( val6 ), 
     m_Val7( val7 ), 
     m_Val8( val8 ), 
     m_PhiMin(phimin), 
      m_PhiMax(phimax), 
     m_EtaMin(etamin), 
     m_EtaMax(etamax)
{
   setName(n);
   setVersion(v);
}

TrigConf::CaloSinCos::CaloSinCos()
{}

TrigConf::CaloSinCos::~CaloSinCos()
{}  
 
void
TrigConf::CaloSinCos::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << " \t-----------------------" << endl; 
   cout << indent << " \t CaloSinCos" << endl;
   printNameIdV(indent);
   cout << indent << " \t phi_min: " << m_PhiMin 
        << " \t phi_max: " << m_PhiMax << " eta_min: " << m_EtaMin 
        << " \t eta_max: " << m_EtaMax << endl;
   cout << indent << " \t values: \t" << m_Val1 << " " << m_Val2 << " " << m_Val3 << " " << m_Val4
        << " " << m_Val5 << " " << m_Val6 << " " << m_Val7 << " " << m_Val8 << endl;
}
