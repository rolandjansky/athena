/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTRACKEXTRAPOLATIONTOOL_H
#define MUON_MUONTRACKEXTRAPOLATIONTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"


class MsgStream;


namespace Muon {
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
}

namespace MuonGM {
  class MuonDetectorManager;
}



namespace Trk {
  class ITrackingGeometrySvc;
  class IExtrapolator;
  class IMagneticFieldTool;
  class TrackingGeometry;
  class TrackingVolume;
  class Track;
}

namespace Muon {

  /**
     @brief tool to select tracks

  */
  class MuonTrackExtrapolationTool : virtual public IMuonTrackExtrapolationTool, public AthAlgTool {
  public:
    /** @brief constructor */
    MuonTrackExtrapolationTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonTrackExtrapolationTool ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** extrapolates track parameters to muon entry record, will return a zero pointer if the extrapolation fails. The caller gets ownership of the new parameters */
    const Trk::TrackParameters* extrapolateToMuonEntryRecord( const Trk::TrackParameters& pars, Trk::ParticleHypothesis particleHypo=Trk::muon ) const;

    /** extrapolates track parameters to muon entry record, will return a zero pointer if the extrapolation fails. The caller gets ownership of the new parameters */
    const Trk::TrackParameters* extrapolateToIP( const Trk::TrackParameters& pars, Trk::ParticleHypothesis particleHypo=Trk::muon ) const;
    
    /** @brief extrapolates a muon track to the muon entry record and returns a new track expressed at the destination.
	@param track input track
	@return a pointer to the extrapolated track, zero if extrapolation failed.
	The ownership of the track is passed to the client calling the tool.
    */
    Trk::Track* extrapolate( const Trk::Track& track ) const;

    /** @brief extrapolates a muon track collection to the muon entry record and returns a new track expressed at the destination.
	@param tracks input track collection
	@return a pointer to the extrapolated track collection, zero if extrapolation failed.
	The ownership of the collection is passed to the client calling the tool.
    */
    TrackCollection* extrapolate( const TrackCollection& tracks ) const;


  private:

    bool getMuonEntrance() const;
    bool retrieveTrackingGeometry() const;

    double estimateDistanceToEntryRecord( const Trk::TrackParameters& pars ) const;
    const Trk::TrackParameters* checkForSecondCrossing( const Trk::TrackParameters& firstCrossing, const Trk::Track& track) const;
    const Trk::TrackParameters* findClosestParametersToMuonEntry( const Trk::Track& track ) const;

    ToolHandle<Trk::IExtrapolator>      m_atlasExtrapolator;
    ToolHandle<Trk::IExtrapolator>      m_muonExtrapolator;

    ServiceHandle<MagField::IMagFieldSvc>    m_magFieldSvc; 
    ServiceHandle<Trk::ITrackingGeometrySvc> m_trackingGeometrySvc;

    ToolHandle<Muon::MuonEDMHelperTool>              m_helper;
    ToolHandle<Muon::MuonEDMPrinterTool>             m_printer;
    ToolHandle<Muon::MuonIdHelperTool>               m_idHelper;

    bool m_cosmics;
    bool m_keepOldPerigee;

    std::string m_msEntranceName;
  };

}

#endif
