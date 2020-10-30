/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONCANDIDATETRACKBUILDER_H
#define MUON_MUONCANDIDATETRACKBUILDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCombinedToolInterfaces/IMuonCandidateTrackBuilderTool.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonSegmentTrackBuilder.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"

#include <vector>


namespace Muon {

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
	if(m_idHelperSvc->isMdt(id1) && m_idHelperSvc->isMdt(id2)) return mst1->globalPosition().perp() < mst2->globalPosition().perp();
	else if(m_idHelperSvc->isRpc(id1) && m_idHelperSvc->isMdt(id2)){
	  if(m_idHelperSvc->rpcIdHelper().doubletR(id1)==1){
	    if(m_idHelperSvc->stationIndex(id2)==MuonStationIndex::StIndex::BM || m_idHelperSvc->isSmallChamber(id2)) return true;
	    else return false;
	  }
	  else return false;
	}else if(m_idHelperSvc->isRpc(id2) && m_idHelperSvc->isMdt(id1)){
	  if(m_idHelperSvc->rpcIdHelper().doubletR(id2)==1){
	    if(m_idHelperSvc->stationIndex(id1)==MuonStationIndex::StIndex::BM || m_idHelperSvc->isSmallChamber(id1)) return false;
	    else return true;
	  }
	  else return true;
	}
	else{
	  if(m_idHelperSvc->rpcIdHelper().doubletR(id1)!=m_idHelperSvc->rpcIdHelper().doubletR(id2)){
	    return m_idHelperSvc->rpcIdHelper().doubletR(id1)<m_idHelperSvc->rpcIdHelper().doubletR(id2);
	  }
	  else return mst1->globalPosition().perp() < mst2->globalPosition().perp();
	}
      }
    }
  SortMeas(const IMuonEDMHelperSvc* h, const IMuonIdHelperSvc* idh, bool end ) : m_edmHelperSvc(h),m_idHelperSvc(idh),isEndcap(end) {}

    const IMuonEDMHelperSvc* m_edmHelperSvc;
    const IMuonIdHelperSvc*  m_idHelperSvc;
    bool isEndcap;
  };
      
  class MuonCandidateTrackBuilderTool : virtual public IMuonCandidateTrackBuilderTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonCandidateTrackBuilderTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonCandidateTrackBuilderTool()=default;
    StatusCode initialize();

    /**IMuonCandidateTrackBuilderTool interface: buildCombinedTrack */   
    Trk::Track* buildCombinedTrack( const Trk::Track& idTrack, const MuonCandidate& candidate ) const;

    void cleanUp() const;

  private:
    ToolHandle<IMuonSegmentTrackBuilder> m_muonTrackBuilder{this,"MuonSegmentTrackBuilder","Muon::MooTrackBuilder/MooMuonTrackBuilder"}; 
    ToolHandle<MuonEDMPrinterTool> m_printer{this,"MuonEDMPrinterTool","Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"}; 
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };
    ToolHandle<Rec::ICombinedMuonTrackBuilder> m_trackFitter{this,"MuonTrackBuilder","Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder"}; 

    Gaudi::Property<bool> m_reOrderMeasurements{this, "ReOrderMeasurements", true};
  };
}



#endif
 

