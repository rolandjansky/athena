/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKSUMMARYHELPERTOOL_H
#define MUONTRACKSUMMARYHELPERTOOL_H

#include "TrkToolInterfaces/IExtendedTrackSummaryHelperTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkGeometry/TrackingGeometry.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <vector>
#include <string>
#include <bitset>

namespace Trk {
  class RIO_OnTrack;
  class TrackStateOnSurface;
  class CompetingRIOsOnTrack;
}

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {

  class MuonTrackSummaryHelperTool :  public extends<AthAlgTool,Trk::IExtendedTrackSummaryHelperTool>    {
  public:
    MuonTrackSummaryHelperTool(const std::string&,const std::string&,const IInterface*);
        
    virtual ~MuonTrackSummaryHelperTool()=default;
        
    virtual StatusCode initialize() override;
        
    virtual void analyse(
                         const Trk::Track& trk,
                         const Trk::RIO_OnTrack* rot,
                         const Trk::TrackStateOnSurface* tsos,
                         std::vector<int>& information,
                         std::bitset<Trk::numberOfDetectorTypes>& hitPattern  ) const override;

    virtual void analyse( 
                         const Trk::Track& trk,
                         const Trk::CompetingRIOsOnTrack* crot,
                         const Trk::TrackStateOnSurface* tsos,
                         std::vector<int>& information,
                         std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const override;

    virtual void analyse(
                         const Trk::Track& trk,
                         const Trk::PRDtoTrackMap *prd_to_track_map,
                         const Trk::RIO_OnTrack* rot,
                         const Trk::TrackStateOnSurface* tsos,
                         std::vector<int>& information,
                         std::bitset<Trk::numberOfDetectorTypes>& hitPattern  ) const override {
      (void) prd_to_track_map;
      analyse(trk,rot,tsos,information,hitPattern);
    }

    virtual void analyse(
                         const Trk::Track& trk,
                         const Trk::PRDtoTrackMap *prd_to_track_map,
                         const Trk::CompetingRIOsOnTrack* crot,
                         const Trk::TrackStateOnSurface* tsos,
                         std::vector<int>& information,
                         std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const override {
      (void) prd_to_track_map;
      analyse(trk,crot,tsos,information, hitPattern);
    }

    virtual
    void searchForHoles(
                        const Trk::Track& track,
                        std::vector<int>& information, Trk::ParticleHypothesis hyp) const override;

    virtual
    void updateSharedHitCount(const Trk::Track&,
                              const Trk::PRDtoTrackMap *,
                              Trk::TrackSummary&) const override  {};

    virtual
    void addDetailedTrackSummary( const Trk::Track& track, Trk::TrackSummary& summary ) const override;

private:

    const MdtPrepDataCollection* findMdtPrdCollection( const Identifier& chId ) const;
    void calculateRoadHits(Trk::MuonTrackSummary::ChamberHitSummary& chamberHitSummary, const Trk::TrackParameters& pars) const;
    bool isFirstProjection( const Identifier& id ) const;
    void updateHoleContent( Trk::MuonTrackSummary::ChamberHitSummary& chamberHitSummary ) const;

    /**increment the 'type'*/
    void increment(int& type) const;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    /* used to do hits-in-road search for straight tracks */
    ToolHandle<Trk::IExtrapolator> m_slExtrapolator{"Trk::Extrapolator/MuonStraightLineExtrapolator"};

    /* used to do hits-in-road search */
    ToolHandle<Trk::IExtrapolator> m_extrapolator{this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator"};
        
    /** allow us to block the calculation of close hits */
    Gaudi::Property<bool> m_calculateCloseHits{this, "CalculateCloseHits", false};

    /** width road use to associate close hits  */
    Gaudi::Property<double> m_roadWidth{this, "RoadWidth", 135., "width used to calculate hits within the road (mm)"};

    /** storegate key of MdtPrepDataContainer */
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_mdtKey{this,"MdtPrepDataContainer","MDT_DriftCircles","MDT PRDs"};

    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
	"MuonDetectorManager", 
	"Key of input MuonDetectorManager condition data"};    
  };
}
#endif 

