/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MuonRecoValidationTool_H
#define MUON_MuonRecoValidationTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "MuonRecToolInterfaces/IMuonRecoValidationTool.h"

#include "xAODTracking/TrackParticle.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonLayerHough/MuonLayerHough.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "MuonRecValidationNtuples/MuonInsideOutValidationNtuple.h"
#include "MuonCombinedEvent/CandidateSummary.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagMatchingTool.h"
#include "MuonRecToolInterfaces/IMuonTruthSummaryTool.h"
#include "MuonRecToolInterfaces/IMuonHitTimingTool.h"
#include "MuonRecToolInterfaces/IMuonHitSummaryTool.h"

#include <vector>
#include <atomic>

class TTree;
class TFile;
class IIncidentSvc;

namespace Trk {
  class Track;
}

namespace Muon {
  class MuonClusterOnTrack;

  struct MuonCandidate;

  class MuonRecoValidationTool : public IMuonRecoValidationTool, virtual public IIncidentListener, public AthAlgTool  {
  public:
    /** @brief default AlgTool constructor */
    MuonRecoValidationTool(const std::string&, const std::string&, const IInterface*);
    
    /** @brief destructor */
    ~MuonRecoValidationTool()=default;
    
    /** @brief initialize method, method taken from bass-class AlgTool */
    virtual StatusCode initialize() override;
    
    /** add a new TrackParticle with it's muon system extension */
    bool addTrackParticle(  const xAOD::TrackParticle& indetTrackParticle, const MuonSystemExtension& muonSystemExtention ) const override;

    /** add a new segment */
    bool add( const MuonSystemExtension::Intersection& intersection, const MuonSegment& segment, int stage ) const override;

    /** add a new hough maximum */
    bool add( const MuonSystemExtension::Intersection& intersection, const MuonHough::MuonLayerHough::Maximum& maximum ) const override;

    /** add a new prd */
    bool add( const MuonSystemExtension::Intersection& intersection, const Trk::PrepRawData& prd, float expos, float expos_err ) const override;

    /** add a new time measurement */
    bool addTimeMeasurement( const MuonSystemExtension::Intersection& intersection, const Trk::MeasurementBase& meas ) const override;

    /** add a new time measurement */
    bool addTimeMeasurement( const MuonSystemExtension::Intersection& intersection, const Identifier& id,
                             const Amg::Vector3D& gpos, float time, float errorTime ) const override;

    /** add StauHits to ntuple */
    bool addTimeMeasurements( const xAOD::TrackParticle& indetTrackParticle, const MuGirlNS::StauHits& stauHits ) const override;

    /** add a new muon candidate */
    bool addMuonCandidate( const xAOD::TrackParticle& indetTrackParticle, const MuonCandidate* candidate, 
                           Trk::Track* combinedTrack, int ntimes, float beta, float chi2ndof, int stage ) const override;

    /**  incident service handle for EndEvent */
    virtual void handle(const Incident& inc) override;

  private:
    /** clear ntuples */
    void clear();

    void extract( const MuonSegment& segment, std::set<Identifier>& ids, std::vector<const MuonClusterOnTrack*>& clusters ) const;

    int getIndex( const MuonSystemExtension::Intersection& intersection ) const {
      int index = -1;
      auto result = m_trackParticleIndexLookup.find(intersection.trackParameters.get());
      if( result != m_trackParticleIndexLookup.end() ) index = result->second;
      return index;
    }
    
    // extract majority barcode out of a set of identifiers
    int getBarcode( const std::set<Identifier>& ids ) const;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<IMuonEDMHelperSvc>       m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };
    ToolHandle<IMuonSegmentHitSummaryTool> m_segmentHitSummaryTool;
    ToolHandle<IMuonHitSummaryTool>        m_hitSummaryTool;
    ToolHandle<IMuonTruthSummaryTool>      m_truthSummaryTool;
    ToolHandle<Trk::IExtrapolator>         m_extrapolator; 
    ToolHandle<IMuTagMatchingTool>         m_matchingTool; 
    ToolHandle<IMuonHitTimingTool>         m_hitTimingTool; 
    ServiceHandle< IIncidentSvc >          m_incidentSvc;

    // the following members are supposed to be used in 1-thread debugging, thus, not in production and not with mutliple threads
    mutable std::vector<const xAOD::TrackParticle*> m_trackParticles;
    mutable std::map< const Trk::TrackParameters*, unsigned int > m_trackParticleIndexLookup;

    mutable MuonInsideOutValidationNtuple m_ntuple;
    TTree* m_tree;

    mutable std::atomic<unsigned int> m_candidateCounter;

    bool m_isMC;
  };
  
}

#endif
