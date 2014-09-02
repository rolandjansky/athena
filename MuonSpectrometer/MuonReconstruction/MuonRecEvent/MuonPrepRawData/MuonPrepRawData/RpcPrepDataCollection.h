/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONPREPRAWDATA_RPCPREPDATACOLLECTION_H
#define MUONPREPRAWDATA_RPCPREPDATACOLLECTION_H

#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {
    
    /** @brief Muon::MuonPrepDataCollection to hold the Muon::RpcPrepData*/
    typedef MuonPrepDataCollection< RpcPrepData > RpcPrepDataCollection;
   
    /**Overload of << operator for MsgStream for debug output*/
    MsgStream& operator << ( MsgStream& sl, const RpcPrepDataCollection& coll);
    
    /**Overload of << operator for std::ostream for debug output*/ 
    std::ostream& operator << ( std::ostream& sl, const RpcPrepDataCollection& coll);
    
}

CLASS_DEF(Muon::RpcPrepDataCollection, 2041800033, 1)

#endif
