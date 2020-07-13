/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IRECMUONCOMBINEDINDETEXTENSIONTOOL_H
#define IRECMUONCOMBINEDINDETEXTENSIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkSegment/SegmentCollection.h"

namespace MuonCombined {

  static const InterfaceID IID_IMuonCombinedInDetExtensionTool("MuonCombined::IMuonCombinedInDetExtensionTool", 1, 0);

  /** @class IMuonCombinedInDetExtensionTool
      @brief interface for tools buildingmuons from ID candidates
 
      @author Niels van Eldik
   */

  class IMuonCombinedInDetExtensionTool : virtual public IAlgTool {
  public:
    struct MuonPrdData{
    MuonPrdData() : mdtPrds(0), rpcPrds(0), tgcPrds(0), cscPrds(0), stgcPrds(0), mmPrds(0) {}
      const Muon::MdtPrepDataContainer* mdtPrds;
      const Muon::RpcPrepDataContainer* rpcPrds;
      const Muon::TgcPrepDataContainer* tgcPrds;
      const Muon::CscPrepDataContainer* cscPrds;
      const Muon::sTgcPrepDataContainer* stgcPrds;
      const Muon::MMPrepDataContainer* mmPrds;
    };

    static const InterfaceID& interfaceID( ) ;


    /**IMuonCombinedInDetExtensionTool interface: build combined muons from ID candidates */    
    virtual void extend( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, TrackCollection* combTracks, TrackCollection* meTracks, 
			 Trk::SegmentCollection* segments) const = 0;
    
    /*New interface including PRDs for MuGirl segment-finding*/
    virtual void extendWithPRDs( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, IMuonCombinedInDetExtensionTool::MuonPrdData prdData,
				 TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments) const = 0;

  };

  inline const InterfaceID& IMuonCombinedInDetExtensionTool::interfaceID()
    { 
      return IID_IMuonCombinedInDetExtensionTool; 
    }

} // end of namespace

#endif 
