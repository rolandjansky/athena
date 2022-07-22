/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "LArCabling/LArLATOMEMapping.h"

#define N_LATOMES 116

LArLATOMEMapping::LArLATOMEMapping(unsigned nLatomes)
{
   if(nLatomes == 0) {
    m_sourceID.resize(N_LATOMES);
    m_onlineID.resize(N_LATOMES);
   } else {
    m_sourceID.resize(nLatomes);
    m_onlineID.resize(nLatomes);
   }
}


