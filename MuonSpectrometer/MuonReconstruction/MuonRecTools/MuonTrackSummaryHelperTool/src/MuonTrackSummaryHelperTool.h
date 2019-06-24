/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKSUMMARYHELPERTOOL_H
#define MUONTRACKSUMMARYHELPERTOOL_H

#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"
//
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "StoreGate/ReadHandleKey.h"

#include <vector>
#include <bitset>

class RpcIdHelper;
class TgcIdHelper;
class CscIdHelper;
class MdtIdHelper;
// New Small Wheel
class MmIdHelper;
class sTgcIdHelper;

class Identifier;

namespace Trk {
  class RIO_OnTrack;
  class TrackStateOnSurface;
  class CompetingRIOsOnTrack;
}

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {

  class MuonTrackSummaryHelperTool :  virtual public Trk::ITrackSummaryHelperTool, public AthAlgTool   {
  public:
    MuonTrackSummaryHelperTool(const std::string&,const std::string&,const IInterface*);
        
    virtual ~MuonTrackSummaryHelperTool ();
        
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

    virtual
    void searchForHoles(
                        const Trk::Track& track,
                        std::vector<int>& information, Trk::ParticleHypothesis hyp) const override;

    virtual
    void addDetailedTrackSummary( const Trk::Track& track, Trk::TrackSummary& summary ) const override;

private:

    const MdtPrepDataCollection* findMdtPrdCollection( const Identifier& chId ) const;
    void calculateRoadHits(Trk::MuonTrackSummary::ChamberHitSummary& chamberHitSummary, const Trk::TrackParameters& pars) const;
    bool isFirstProjection( const Identifier& id ) const;
    void updateHoleContent( Trk::MuonTrackSummary::ChamberHitSummary& chamberHitSummary ) const;

    /**increment the 'type'*/
    void increment(int& type) const;

    // muon IdHelpers
    const RpcIdHelper* m_rpcId{nullptr};
    const TgcIdHelper* m_tgcId{nullptr};
    const CscIdHelper* m_cscId{nullptr};
    const MdtIdHelper* m_mdtId{nullptr};
    //New Small Wheel
    const sTgcIdHelper* m_stgcId{nullptr};
    const MmIdHelper* m_mmId{nullptr};

    /* used to work out layer ids etc*/
    ToolHandle<MuonIdHelperTool> m_idHelperTool{"Muon::MuonIdHelperTool/MuonIdHelperTool"};

    /* used to work out if track has momentum */
    ToolHandle<MuonEDMHelperTool> m_edmHelperTool{"Muon::MuonEDMHelperTool/MuonEDMHelperTool"};

    /* used to do hits-in-road search for straight tracks */
    ToolHandle<Trk::IExtrapolator> m_slExtrapolator{"Trk::Extrapolator/MuonStraightLineExtrapolator"};

    /** tool used to do hole search */
    ToolHandle<Trk::ITrackHoleSearchTool> m_muonTgTool{this, "HoleOnTrackTool", "MuonHolesOnTrack"};

    /* used to do hits-in-road search */
    ToolHandle<Trk::IExtrapolator> m_extrapolator{this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator"};
        
    /**Allows us to block the hole search whilst the Muon tracking geometry etc is being debugged*/
    Gaudi::Property<bool> m_doHoles{this, "DoHolesOnTrack", false};
        
    /** allow us to block the calculation of close hits */
    Gaudi::Property<bool> m_calculateCloseHits{this, "CalculateCloseHits", false};

    /** width road use to associate close hits  */
    Gaudi::Property<double> m_roadWidth{this, "RoadWidth", 135., "width used to calculate hits within the road (mm)"};

    /** storegate key of MdtPrepDataContainer */
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_mdtKey{this,"MdtPrepDataContainer","MDT_DriftCircles","MDT PRDs"};

    /** name of the tracking geometry */
    Gaudi::Property<std::string> m_trackingGeometryName{this, "TrackingGeometryName", "MuonStandaloneTrackingGeometry"};

    mutable const Trk::TrackingGeometry* m_trackingGeometry{nullptr};
    const MuonGM::MuonDetectorManager*  m_detMgr{nullptr};
  };
}
#endif 

