/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONCLUSTERSEGMENTFINDER_H
#define MUON_IMUONCLUSTERSEGMENTFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

static const InterfaceID IID_IMuonClusterSegmentFinder("Muon::IMuonClusterSegmentFinder",1,0);


namespace Muon {

  class MuonSegment;

  /** Interface for tools calculating hit count summaries for track */
  class IMuonClusterSegmentFinder : virtual public IAlgTool {     
  public:
    /** IAlgTool interface */
    static const InterfaceID& interfaceID();

    virtual std::vector<const Muon::MuonSegment*>* getClusterSegments(bool doTGCClust, bool doRPCClust) const = 0;

    /** @brief Find tgc + mdt segments for a given set of TGC hit collections */
    virtual void findSegments( std::vector<const TgcPrepDataCollection*>& tgcCols, std::vector<const Muon::MuonSegment*>* segments ) const = 0;

    /** @brief Find rpc + mdt segments for a given set of RPC hit collections */
    virtual void findSegments( std::vector<const RpcPrepDataCollection*>& rpcCols, std::vector<const Muon::MuonSegment*>* segments ) const = 0;

  };
 
  inline const InterfaceID& IMuonClusterSegmentFinder::interfaceID()
  {
    return IID_IMuonClusterSegmentFinder;
  }


} // end of name space

#endif // IMuonClusterSegmentFinder_H
