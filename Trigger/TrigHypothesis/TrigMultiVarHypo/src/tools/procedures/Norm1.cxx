/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///Local include(s)
#include "TrigMultiVarHypo/tools/procedures/Norm1.h"
///std library(s)
#include <cmath>
using namespace std;
using namespace Ringer;




Norm1::Norm1( double etmin, double etmax, double etamin, double etamax, double mumin, double mumax, bool usetilecal):
  INormalization(etmin,etmax,etamin,etamax,mumin,mumax),
  m_useTileCal(usetilecal)
{;}
 


void Norm1::execute( std::vector<float> &rings ) const {
  
  if(!m_useTileCal){
    if(rings.size() == 100){
      // remove the last 12 rings (tilecal)
      rings.erase(rings.begin()+88,rings.end());   
    }
  }
  float energy=0.0;
  for(unsigned i=0; i<rings.size(); ++i)  energy+=rings.at(i);
  for(unsigned i=0; i<rings.size(); ++i)  rings.at(i)=rings.at(i)/std::fabs(energy);
}



