/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "LArCabling/LArLATOMEMapping.h"

#define N_LATOMES 116

LArLATOMEMapping::LArLATOMEMapping()
{
 m_sourceID.resize(N_LATOMES);
 m_onlineID.resize(N_LATOMES);
}


