/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONRECOVALIDATIONTOOL_H
#define MUON_IMUONRECOVALIDATIONTOOL_H

/** 
    @class IMuonRecoValidationTool

    @author Niels.Van.Eldik@cern.ch
*/

#include "GaudiKernel/IAlgTool.h"

#include "xAODTracking/TrackParticle.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonCombinedEvent/CandidateSummary.h"

namespace Trk {
  class PrepRawData;
  class Track;
}

namespace Muon {
  
  class MuonSegment;
  struct MuonCandidate;

  /** Interface ID*/  
  static const InterfaceID IID_IMuonRecoValidationTool("Muon::IMuonRecoValidationTool", 1, 0);

  class IMuonRecoValidationTool : virtual public IAlgTool {
  public:
     /**Virtual destructor*/
    virtual ~IMuonRecoValidationTool(){};
  
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IMuonRecoValidationTool; }
    
    /** add a new TrackParticle with it's muon system extension */
    virtual bool addTrackParticle(  const xAOD::TrackParticle& indetTrackParticle, const MuonSystemExtension& muonSystemExtention ) const = 0;
    
    /** add a new segment, indicate the stage of the reco */
    virtual bool add( const MuonSystemExtension::Intersection& intersection, const MuonSegment& segment, int stage ) const = 0;

    /** add a new hough maximum */
    virtual bool add( const MuonSystemExtension::Intersection& intersection,const MuonHough::MuonLayerHough::Maximum& maximum ) const = 0;

    /** add a new prd */
    virtual bool add( const MuonSystemExtension::Intersection& intersection, const Trk::PrepRawData& prd, float expos, float expos_err ) const = 0;

    /** add a new time measurement */
    virtual bool addTimeMeasurement( const MuonSystemExtension::Intersection& intersection, const Trk::MeasurementBase& meas ) const = 0;

    /** add a new time measurement */
    virtual bool addTimeMeasurement( const MuonSystemExtension::Intersection& intersection, const Identifier& id,
                                     const Amg::Vector3D& gpos, float time, float errorTime ) const = 0;

    /** add new MuonCandidate */
    virtual bool addMuonCandidate( const xAOD::TrackParticle& indetTrackParticle, const MuonCandidate* candidate, 
                                   Trk::Track* combinedTrack, int ntimes, float beta, float chi2ndof, int stage ) const = 0;

    /** add StauHits to ntuple */
    virtual bool addTimeMeasurements( const xAOD::TrackParticle& indetTrackParticle, const MuGirlNS::StauHits& stauHits ) const = 0;

  };

}

#endif
