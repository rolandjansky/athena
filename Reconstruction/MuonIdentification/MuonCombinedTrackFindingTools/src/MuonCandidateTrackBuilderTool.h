/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONCANDIDATETRACKBUILDER_H
#define MUON_MUONCANDIDATETRACKBUILDER_H

#include "MuonCombinedToolInterfaces/IMuonCandidateTrackBuilderTool.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Rec {
  class ICombinedMuonTrackBuilder;
}

namespace Muon {

  class IMuonSegmentTrackBuilder;
  class MuonEDMPrinterTool;
  struct MuonCandidate;

  class SortMeas{
  public:
    bool operator()(const Trk::MeasurementBase* mst1,const Trk::MeasurementBase* mst2) const{
      if(isEndcap) {
	return fabs(mst1->globalPosition().z()) < fabs(mst2->globalPosition().z());
      }
      else {
	Identifier id1=m_edmHelperSvc->getIdentifier(*mst1);
	Identifier id2=m_edmHelperSvc->getIdentifier(*mst2);
	if(m_idHelper->isMdt(id1) && m_idHelper->isMdt(id2)) return mst1->globalPosition().perp() < mst2->globalPosition().perp();
	else if(m_idHelper->isRpc(id1) && m_idHelper->isMdt(id2)){
	  if(m_idHelper->rpcIdHelper().doubletR(id1)==1){
	    if(m_idHelper->stationIndex(id2)==MuonStationIndex::StIndex::BM || m_idHelper->isSmallChamber(id2)) return true;
	    else return false;
	  }
	  else return false;
	}else if(m_idHelper->isRpc(id2) && m_idHelper->isMdt(id1)){
	  if(m_idHelper->rpcIdHelper().doubletR(id2)==1){
	    if(m_idHelper->stationIndex(id1)==MuonStationIndex::StIndex::BM || m_idHelper->isSmallChamber(id1)) return false;
	    else return true;
	  }
	  else return true;
	}
	else{
	  if(m_idHelper->rpcIdHelper().doubletR(id1)!=m_idHelper->rpcIdHelper().doubletR(id2)){
	    return m_idHelper->rpcIdHelper().doubletR(id1)<m_idHelper->rpcIdHelper().doubletR(id2);
	  }
	  else return mst1->globalPosition().perp() < mst2->globalPosition().perp();
	}
      }
    }
  SortMeas(const IMuonEDMHelperSvc* h, const MuonIdHelperTool* idh, bool end ) : m_edmHelperSvc(h),m_idHelper(idh),isEndcap(end) {}

    const IMuonEDMHelperSvc* m_edmHelperSvc;
    const MuonIdHelperTool*  m_idHelper;
    bool isEndcap;
  };
      
  class MuonCandidateTrackBuilderTool : virtual public IMuonCandidateTrackBuilderTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonCandidateTrackBuilderTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonCandidateTrackBuilderTool();
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonCandidateTrackBuilderTool interface: buildCombinedTrack */   
    Trk::Track* buildCombinedTrack( const Trk::Track& idTrack, const MuonCandidate& candidate ) const;

  private:

    ToolHandle<IMuonSegmentTrackBuilder>       m_muonTrackBuilder; 
    ToolHandle<MuonEDMPrinterTool>             m_printer;
    ToolHandle<MuonIdHelperTool>               m_idHelper;
    ServiceHandle<IMuonEDMHelperSvc>           m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };
    bool                                       m_reOrderMeasurements;
    ToolHandle<Rec::ICombinedMuonTrackBuilder> m_trackFitter;
  };
}



#endif
 

