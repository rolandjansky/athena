/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MuonRecoValidationTool_H
#define MUON_MuonRecoValidationTool_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonRecToolInterfaces/IMuonRecoValidationTool.h"
#include "Identifier/Identifier.h"

#include "xAODTracking/TrackParticle.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonLayerHough/MuonLayerHough.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "MuonRecValidationNtuples/MuonInsideOutValidationNtuple.h"
#include "MuonCombinedEvent/CandidateSummary.h"

#include <vector>
class TTree;
class TFile;
class IIncidentSvc;
class IMuTagMatchingTool;

namespace Trk {
  class IExtrapolator;
  class Track;
}

namespace Muon {

  class IMuonTruthSummaryTool;
  class IMuonSegmentHitSummaryTool;
  class IMuonHitSummaryTool;
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class IMuonHitTimingTool;
  class MuonSegment;
  class MuonClusterOnTrack;
  struct MuonCandidate;

  class MuonRecoValidationTool : public IMuonRecoValidationTool, virtual public IIncidentListener, public AthAlgTool  {
  public:
    /** @brief default AlgTool constructor */
    MuonRecoValidationTool(const std::string&, const std::string&, const IInterface*);
    
    /** @brief destructor */
    ~MuonRecoValidationTool();
    
    /** @brief initialize method, method taken from bass-class AlgTool */
    virtual StatusCode initialize() override;

    /** @brief finialize method, method taken from bass-class AlgTool */
    virtual StatusCode finalize() override;
    
    /** add a new TrackParticle with it's muon system extension */
    bool addTrackParticle(  const xAOD::TrackParticle& indetTrackParticle, const MuonSystemExtension& muonSystemExtention ) override;

    /** add a new segment */
    bool add( const MuonSystemExtension::Intersection& intersection, const MuonSegment& segment, int stage ) override;

    /** add a new hough maximum */
    bool add( const MuonSystemExtension::Intersection& intersection, const MuonHough::MuonLayerHough::Maximum& maximum ) override;

    /** add a new prd */
    bool add( const MuonSystemExtension::Intersection& intersection, const Trk::PrepRawData& prd, float expos, float expos_err ) override;

    /** add a new time measurement */
    bool addTimeMeasurement( const MuonSystemExtension::Intersection& intersection, const Trk::MeasurementBase& meas ) override;

    /** add a new time measurement */
    bool addTimeMeasurement( const MuonSystemExtension::Intersection& intersection, const Identifier& id,
                             const Amg::Vector3D& gpos, float time, float errorTime ) override;

    /** add StauHits to ntuple */
    bool addTimeMeasurements( const xAOD::TrackParticle& indetTrackParticle, const MuGirlNS::StauHits& stauHits ) override;

    /** add a new muon candidate */
    bool addMuonCandidate( const xAOD::TrackParticle& indetTrackParticle, const MuonCandidate* candidate, 
                           Trk::Track* combinedTrack, int ntimes, float beta, float chi2ndof, int stage ) override;

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

    ToolHandle<MuonIdHelperTool>           m_idHelper;
    ToolHandle<MuonEDMHelperTool>          m_edmHelper;
    ToolHandle<IMuonSegmentHitSummaryTool> m_segmentHitSummaryTool;
    ToolHandle<IMuonHitSummaryTool>        m_hitSummaryTool;
    ToolHandle<IMuonTruthSummaryTool>      m_truthSummaryTool;
    ToolHandle<Trk::IExtrapolator>         m_extrapolator; 
    ToolHandle<IMuTagMatchingTool>         m_matchingTool; 
    ToolHandle<IMuonHitTimingTool>         m_hitTimingTool; 
    ServiceHandle< IIncidentSvc >          m_incidentSvc;

    std::vector<const xAOD::TrackParticle*> m_trackParticles;
    std::map< const Trk::TrackParameters*, unsigned int > m_trackParticleIndexLookup;

    MuonInsideOutValidationNtuple m_ntuple;
    TTree* m_tree;

    unsigned int m_candidateCounter;

    bool m_isMC;
  };
  
}

#endif
