///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef THINNINGUTILS_ThinTrkTrackAlg_H
#define THINNINGUTILS_ThinTrkTrackAlg_H 
/**
 @class ThinTrkTrackAlg
*/
// STL includes
#include <string>


// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ThinningHandleKey.h"


class ThinTrkTrackAlg : public ::AthAlgorithm {
public:
  
  /// Constructor with parameters:
  ThinTrkTrackAlg( const std::string& name, ISvcLocator* pSvcLocator );
    
  /// Destructor:
  virtual ~ThinTrkTrackAlg();
  
  /// Athena algorithm's initalize hook
  virtual StatusCode  initialize() final;
  
  /// Athena algorithm's execute hook
  virtual StatusCode  execute() final;
  
  /// Athena algorithm's finalize hook
  virtual StatusCode  finalize() final;
  
 private:
  /// Inline method
  StatusCode doEGamma() ;
  StatusCode doMuons() ;
  
  StringProperty m_streamName
  { this, "StreamName", "", "Name of the stream being thinned" };

  // For P->T converters of ID tracks with Pixel
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  // For P->T converters of ID tracks with SCT
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  
  /// Should the thinning run?
  bool m_doElectrons;
  bool m_doPhotons;
  bool m_doMuons;
  /// The containers of Objects of interest Electron/Muon
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonsKey
  { this, "MuonsKey", "Muons", "StoreGate key for muons container" };

  SG::ReadHandleKey<xAOD::ElectronContainer> m_electronsKey
  { this, "ElectronsKey", "Electrons", "StoreGate key for electrons container" };

  SG::ReadHandleKey<xAOD::PhotonContainer> m_photonsKey
  { this, "PhotonsKey", "Photons", "StoreGate key for photon container" };


  /// Containers to thin
  SG::ThinningHandleKey<TrackCollection> m_CombinedMuonsTracksKey
  { this, "CombinedMuonsTrackKey", "CombinedMuonTracks", "StoreGate key for combined muons Trk::Track container" };

  SG::ThinningHandleKey<TrackCollection> m_GSFTracksKey
  { this, "GSFTrackKey", "GSFTracks", "StoreGate key for GSF Trk::Track container" };

  //pT cuts for the objects
  double m_minptElectrons;
  double m_minptPhotons;
  double m_minptMuons;
  //Should we only keep the best match?
  bool m_bestonlyElectrons;
  bool m_bestonlyPhotons;

};


#endif 
