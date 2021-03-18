/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Collect RPC readout elements belonging to the same DoubletR
 -------------------------------------------------------
 ***************************************************************************/

#ifndef MUONREADOUTGEOMETRY_RPCREADOUTSET_H
#define MUONREADOUTGEOMETRY_RPCREADOUTSET_H

#include "Identifier/Identifier.h"

class RpcIdHelper;

namespace MuonGM {

class MuonDetectorManager;
class RpcReadoutElement;

class RpcReadoutSet
{
public:

   RpcReadoutSet();
   RpcReadoutSet(const MuonDetectorManager* mgr, Identifier id);
   // this is a detectorId

   void  setMgr(const MuonDetectorManager* mgr);
   void  setId(Identifier id);
   const RpcReadoutElement* readoutElement(Identifier id) const;
   // this is a channelId
 
   const RpcReadoutElement* readoutElement(int dbz, int dbp) const;
   unsigned int NreadoutElements() const;
   unsigned int NdoubletZ() const;
   unsigned int NsegmentedDoubletZ() const;
   unsigned int NPhimodules(int dbz) const;  
    
private:

   Identifier m_id; // this is a detectorId
   const RpcIdHelper*  m_helper;
   const MuonDetectorManager* m_mgr;
};

} // namespace MuonGM

#endif // MUONREADOUTGEOMETRY_RPCREADOUTSET_H
