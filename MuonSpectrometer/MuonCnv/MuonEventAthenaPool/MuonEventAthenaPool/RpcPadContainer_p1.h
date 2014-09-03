/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcPadContainer_p1.h

#ifndef MUONEVENTATHENAPOOL_RPCPADCONAINTER_P1_H
#define MUONEVENTATHENAPOOL_RPCPADCONAINTER_P1_H

/** 
	Persistent version of RpcPadContainer
@author edward.moyse@cern.ch
*/ 

#include <vector>
#include "MuonEventAthenaPool/RpcPad_p1.h"

class RpcPadContainer_p1 
:public std::vector<RpcPad_p1> {

public:  
    RpcPadContainer_p1() {}; 
    //friend class RpcPadContainerCnv_p1;

};
#endif


