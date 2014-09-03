/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcPadContainer_p2.h

#ifndef MUONEVENTATHENAPOOL_RPCPADCONAINTER_P2_H
#define MUONEVENTATHENAPOOL_RPCPADCONAINTER_P2_H

/** 
	Persistent version of RpcPadContainer
@author edward.moyse@cern.ch
*/ 

#include <vector>
#include "MuonEventAthenaPool/RpcPad_p1.h"

class RpcPadContainer_p2  {
public:  
    RpcPadContainer_p2() {}; 
    std::vector<RpcPad_p1> m_pads;
    unsigned int size() { return m_pads.size(); } // needed for compat with MuonRDO cnv icc
};
#endif


