/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONCLUSTERSEGMENTFINDER_H
#define MUON_IMUONCLUSTERSEGMENTFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "TrkSegment/SegmentCollection.h"

static const InterfaceID IID_IMuonClusterSegmentFinder("Muon::IMuonClusterSegmentFinder",1,0);


namespace Muon {

  class MuonSegment;

  /** Interface for tools calculating hit count summaries for track */
  class IMuonClusterSegmentFinder : virtual public IAlgTool {     
  public:
    /** IAlgTool interface */
    static const InterfaceID& interfaceID();

    virtual void getClusterSegments(const Muon::MdtPrepDataContainer* mdtPrdCont, 
				    const Muon::RpcPrepDataContainer* rpcPrdCont, const Muon::TgcPrepDataContainer* tgcPrdCont,
				    const PRD_MultiTruthCollection* tgcTruthColl, const PRD_MultiTruthCollection* rpcTruthColl,
				    Trk::SegmentCollection* segColl) const = 0;

    virtual void getClusterSegments(const Muon::MdtPrepDataContainer* mdtPrdCont,
				    std::vector<const Muon::TgcPrepDataCollection*>* tgcCols, std::vector<const Muon::RpcPrepDataCollection*>* rpcCols,
				    const PRD_MultiTruthCollection* tgcTruthColl, const PRD_MultiTruthCollection* rpcTruthColl,
				    Trk::SegmentCollection* segColl) const = 0;

    /** @brief Find tgc + mdt segments for a given set of TGC hit collections */
    virtual void findSegments( std::vector<const TgcPrepDataCollection*>& tgcCols, const Muon::MdtPrepDataContainer* mdtPrdCont, Trk::SegmentCollection* segColl,
			       const PRD_MultiTruthCollection* tgcTruthColl) const = 0;

    /** @brief Find rpc + mdt segments for a given set of RPC hit collections */
    virtual void findSegments( std::vector<const RpcPrepDataCollection*>& rpcCols, const Muon::MdtPrepDataContainer* mdtPrdCont, Trk::SegmentCollection* segColl,
			       const PRD_MultiTruthCollection* rpcTruthColl) const = 0;

  };
 
  inline const InterfaceID& IMuonClusterSegmentFinder::interfaceID()
  {
    return IID_IMuonClusterSegmentFinder;
  }


} // end of name space

#endif // IMuonClusterSegmentFinder_H
