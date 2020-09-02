/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTRACKEXTRAPOLATIONTOOL_H
#define MUON_MUONTRACKEXTRAPOLATIONTOOL_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkExInterfaces/IExtrapolator.h"

// For magneticfield
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

#include "TrkParameters/TrackParameters.h"

namespace Trk {
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
    virtual ~MuonTrackExtrapolationTool()=default;
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
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

    const Trk::Perigee* createPerigee( const Trk::TrackParameters& pars ) const;
    
    // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                               "Name of the Magnetic Field conditions object key"};

    ServiceHandle<Trk::ITrackingGeometrySvc> m_trackingGeometrySvc {this, "TrackingGeometrySvc", "TrackingGeometrySvc/AtlasTrackingGeometrySvc"};
    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", "Handle to the service providing the IMuonEDMHelperSvc interface"};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    ToolHandle<Muon::MuonEDMPrinterTool> m_printer {this, "EDMPrinter", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool", "helper to nicely print out tracks"};
    ToolHandle<Trk::IExtrapolator> m_atlasExtrapolator {this, "AtlasExtrapolator", "Trk::Extrapolator/AtlasExtrapolator"};
    ToolHandle<Trk::IExtrapolator> m_muonExtrapolator {this, "MuonExtrapolator", "Trk::Extrapolator/MuonExtrapolator"};
    ToolHandle<Trk::IExtrapolator> m_muonExtrapolator2 {this, "MuonExtrapolator2", "Trk::Extrapolator/MuonExtrapolator"};

    Gaudi::Property<bool> m_cosmics {this, "Cosmics", false};
    Gaudi::Property<bool> m_keepOldPerigee {this, "KeepInitialPerigee", true};
    Gaudi::Property<std::string> m_msEntranceName {this, "MuonSystemEntranceName", "MuonSpectrometerEntrance"};
  };

}

#endif
