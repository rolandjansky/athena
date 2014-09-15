/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENTOOLS_MUONALIGNREFITTOOL_H
#define MUONALIGNGENTOOLS_MUONALIGNREFITTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Identifier/Identifier.h"
#include "TrkTrack/Track.h"

#include "MuonRecToolInterfaces/IMuonRefitTool.h"
//#include "MuonAlignGenTools/myMuonRefitTool.h"

#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"

#include <math.h>

/**
   @file MuonAlignRefitTool.h
   @class MuonAlignRefitTool

   @brief Tool containing methods to refit tracks for muon alignment 

   @author Robert Harrington <roberth@bu.edu>
   @date 11/25/2010
*/

class AtlasDetectorID;

namespace Trk {
  class Track;
  class IPropagator;
  //class IMagneticFieldTool;
  class MagneticFieldProperties;
  class MeasurementTypeID;
  class Volume;
}

class IMessageSvc;

namespace Muon {

  class IMdtDriftCircleOnTrackCreator;
  class IMuonClusterOnTrackCreator;
  class IMuonCompetingClustersOnTrackCreator;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
  class MuonIdHelperTool;
  class IMuonSegmentTrackBuilder;
  //class IMuonRefitTool;
  class MuonAlignHelperTool;

  static const InterfaceID ID_MuonAlignRefitTool ( "Muon::MuonAlignRefitTool",1,0);

  class MuonAlignRefitTool : public AthAlgTool {
    
  public:
    MuonAlignRefitTool(const std::string& type,
			const std::string& name,
			const IInterface* parent);
    virtual ~MuonAlignRefitTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** sets qOverP for first TSOS on track */
    Trk::Track* pTCorrectTrack(const Trk::Track& track, double qOverP, bool removePerigee=false) const;

    const Trk::Track* prepareHitsOnTrack(const Trk::Track& track);

    const Trk::Track* refitForAlignment(const Trk::Track& track);

    const Trk::MaterialEffectsBase* materialEffects(const Trk::TrackStateOnSurface& tsos) const;
    
    static const InterfaceID& interfaceID() { return ID_MuonAlignRefitTool; }
    
  private:
    
    //ToolHandle<Trk::IPropagator>         m_propagator;
    //ToolHandle<Trk::IMagneticFieldTool>  m_magFieldTool;    
    ToolHandle<IMuonSegmentTrackBuilder> m_trackBuilder; //<! track builder
    ToolHandle<MuonEDMPrinterTool>       m_printer;      //<! helper to nicely print out tracks
    ToolHandle<MuonEDMHelperTool>        m_helper;       //<! muon EDM helper
    ToolHandle<MuonIdHelperTool>         m_muonIdHelper;     //<! muon id helper
    ToolHandle<IMuonRefitTool>           m_refitTool;    //<! muon refit tool
    //ToolHandle<myMuonRefitTool>            m_refitTool;    //<! muon refit tool

    ToolHandle<MuonAlignHelperTool>      m_muonAlignHelperTool;

    //Trk::MagneticFieldProperties*        m_magFieldProperties; //!< magnetic field properties
    const AtlasDetectorID*  m_idHelper;
    Trk::MeasurementTypeID* m_measTypeIdHelper;

    ServiceHandle<Trk::ITrackingVolumesSvc> m_trackingVolumesSvc;

    const Trk::Volume*      m_calVolume;
    const Trk::Volume*      m_indetVolume;
    
    bool m_removeBIS8Hits;
    bool m_removeBEEHits;
    bool m_removeEEHits;
    bool m_removeTGCHits;
    bool m_removeTGCEtaHits;
    bool m_removeRPCEtaHits;
    bool m_removeCSCHits;
    bool m_removeSpoiledCSCHits;
    bool m_removeCSCPhiHits;
    bool m_removeSmallBarrelHits;
    bool m_removeBarrelHits;
    bool m_removeEndcapHits;
    bool m_removeTRTHits;
    bool m_removeScattering;

    bool m_declusterTGCHits;
    
    bool m_recreateStartingParameters;
    bool m_extrapolateToMuonEntry;

    bool m_redoErrorScaling;

    IMuonRefitTool::Settings m_defaultRefitSettings;

  }; // end class
  
} // end namespace

#endif // MUONALIGNGENTOOLS_MUONALIGNHELPERTOOL_H
