/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINCONTAINERS_H
#define THINCONTAINERS_H

/**
   @class ThinContainers
   @brief Thinning algorithm to thin the ContainerContainer

   This algorithm can be used to only keep calorimeter cells near interesting
object (level 1 RoI, electron, photon, muon, tau, jet).

   @author Karsten Koeneke

*/

/******************************************************************************
Name:        ThinContainers

Author:      Karsten Koeneke (DESY)
Created:     August 2008

Description: This is a short algorithm to select calorimeter cells that are
             near interesting objects (electron, muons, Level 1 RoIs,...).
******************************************************************************/


#include <string>
#include <vector>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventKernel/INavigable4Momentum.h"
#include "AthenaKernel/IThinningSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthContainers/DataVector.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"

// Needed for TrackParticles
#include "Particle/TrackParticleContainer.h"
// Needed for Tracks
#include "TrkTrack/TrackCollection.h" 
#include "TrkTrack/Track.h" 

// Needed for the cell IDs
#include "Identifier/Identifier.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterContainer.h"

// Needed for the pixel clusters
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetIdentifier/PixelID.h"

// Needed for RegSelSvc
#include "GaudiKernel/ToolHandle.h"
#include "IRegionSelector/RegSelEnums.h"

typedef InDet::PixelClusterContainer PixelClusterContainer;
typedef InDet::PixelClusterCollection PixelClusterCollection;
typedef InDet::PixelCluster PixelCluster;

typedef InDet::SCT_ClusterContainer SCT_ClusterContainer;
typedef InDet::SCT_ClusterCollection SCT_ClusterCollection;
typedef InDet::SCT_Cluster SCT_Cluster;

typedef InDet::TRT_DriftCircleContainer TRT_DriftCircleContainer;
typedef InDet::TRT_DriftCircleCollection TRT_DriftCircleCollection;
typedef InDet::TRT_DriftCircle TRT_DriftCircle;

//class CaloClusterContainer;
//Needed for RegSelSvc
class IRegSelSvc;
class I4Momentum;



class ThinContainers : public AthAlgorithm {
  
public: // Constructor and Destructor
  // Standard Service Constructor
  ThinContainers(const std::string& name, ISvcLocator* pSvcLocator);

  // Destructor
  virtual ~ThinContainers();
  
public:
  // Gaudi Service Interface method implementations:
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


private:

  // The method to find to good cells near interesting objects
  StatusCode findGoodCells(const CaloCellContainer* cellCont,
                           const I4Momentum& candHepLorentz, 
                           double maxDeltaR, bool useDeltaEtaPhi=false,
                           double maxDeltaEta=0.0, double maxDeltaPhi=0.0);

  // The method to find good Inner Detector Objects near interesting objects
  // using the region selector service                                                               
  StatusCode findGoodIDObjects(DETID detectorID,
                               CLHEP::HepLorentzVector candHepLorentz,
                               CLHEP::Hep3Vector primaryVertex,
                               double maxDeltaEta,
                               double maxDeltaPhi,
                               double maxDeltaZ);
  

  // The method to find to good TrackParticles near interesting objects
  StatusCode findGoodTrackParticles(const Rec::TrackParticleContainer* trackParticleCont,
                                    const I4Momentum& candHepLorentz, 
                                    double maxDeltaR, bool useDeltaEtaPhi=false,
                                    double maxDeltaEta=0.0, double maxDeltaPhi=0.0);
  
  // The method to find to good Tracks near interesting objects
  StatusCode findGoodTracks(const TrackCollection* trackCont,
			    CLHEP::HepLorentzVector candHepLorentz, 
			    double maxDeltaR, bool useDeltaEtaPhi=false,
			    double maxDeltaEta=0.0, double maxDeltaPhi=0.0);
  
  // The method to thin the CaloClusters
  StatusCode thinCaloClusters(const CaloClusterContainer* clusCont);
  
  // The method to thin the CaloCells
  StatusCode thinCells(const CaloCellContainer* cellCont);

  // The method to thin the PixelClusters 
  StatusCode thinPixels(const InDet::PixelClusterContainer* pixelCont);

  // The method to thin the SCT_Clusters 
  StatusCode thinSCTClusters(const InDet::SCT_ClusterContainer* sctCont);

  // The method to thin the TRT_DriftCircles 
  StatusCode thinTRTDriftCircles(const InDet::TRT_DriftCircleContainer* trtCont);

  // The method to thin the TrackParticles
  StatusCode thinTrackParticles(const Rec::TrackParticleContainer* trackParticleCont);

  // The method to thin the TrackParticles
  StatusCode thinTracks(const TrackCollection* trackCont );

  // Pointer to IThinningSvc
  ServiceHandle<IThinningSvc> m_thinningSvc;

  // Get the TRT_ID helper
  const TRT_ID* m_trtID;

  // Create an empty list of IDs for the various objects (to be thinned)
  // that will be kept.
  std::set<Identifier> m_goodCellIDs; // need to re-code this with hash tables
  std::set<int> m_goodClusterIDs; // need to re-code this with hash tables
  std::set<int> m_goodTrackParticleIDs; // need to re-code this with hash tables
  std::set<int> m_goodTrackIDs; // need to re-code this with hash tables

  ServiceHandle<IRegSelSvc> m_regionSelector;
  std::vector<IdentifierHash> m_goodPixelIDs; // need to re-code this with hash tables
  std::vector<IdentifierHash> m_goodSCTClusterIDs; // need to re-code this with hash tables
  std::vector<IdentifierHash> m_goodTRTDriftCircleIDs; // need to re-code this with hash tables
  
  // jobOption parameters
  bool   m_thinCaloCells;
  bool   m_thinPixelClusters;
  bool   m_thinSCTClusters;
  bool   m_thinTRTDriftCircles;
  bool   m_thinTrackParticles;
  bool   m_thinTracks;
  bool   m_keepForwardElectronCells;
  bool   m_keepOnlyForwardElectronClusters;

  std::string m_caloCellKey;
  std::string m_caloClusterKey;
  std::string m_pixelClusterKey;
  std::string m_sctClusterKey;
  std::string m_trtDriftCircleKey;
  std::string m_trackParticleKey;
  std::string m_tracksKey;
  std::vector<std::string> m_Lvl1Items;

  bool   m_keepNearLvl1Roi;
  double m_maxDeltaRLvl1Roi;
  bool   m_useDeltaEtaPhiLvl1Roi;
  double m_maxDeltaEtaLvl1Roi;
  double m_maxDeltaPhiLvl1Roi;
  std::string m_electronKey;
  std::string m_photonKey;
  std::vector<std::string> m_muonKeys;
  std::string m_tauKey;
  std::vector<std::string> m_jetKeys;
  bool   m_keepNearElectron;
  bool   m_keepNearPhoton;
  bool   m_keepNearMuon;
  bool   m_keepNearTau;
  bool   m_keepNearJet;
  double m_maxDeltaRElectron;
  bool   m_useDeltaEtaPhiElectron;
  double m_maxDeltaEtaElectron;
  double m_maxDeltaPhiElectron;
  double m_maxDeltaRPhoton;
  bool   m_useDeltaEtaPhiPhoton;
  double m_maxDeltaEtaPhoton;
  double m_maxDeltaPhiPhoton;
  double m_maxDeltaRMuon;
  bool   m_useDeltaEtaPhiMuon;
  double m_maxDeltaEtaMuon;
  double m_maxDeltaPhiMuon;
  double m_maxDeltaRTau;
  bool   m_useDeltaEtaPhiTau;
  double m_maxDeltaEtaTau;
  double m_maxDeltaPhiTau;
  double m_maxDeltaRJet;
  bool   m_useDeltaEtaPhiJet;
  double m_maxDeltaEtaJet;
  double m_maxDeltaPhiJet;
  double m_minEtElectron;
  double m_minEtPhoton;
  double m_minEtMuon;
  double m_minEtTau;
  double m_minEtJet;
  bool   m_keepPRDFromTracks;
  std::vector<std::string> m_trackForPRDKeys;
  bool   m_keepCellsFromLinkContainers;
  std::vector<std::string> m_cellLinkKeys;
  bool   m_useEGammaClusterPosition;
  bool   m_treatBadTracks;
  double m_wideDeltaEtaForBadTracks;
  bool   m_useCaloCellEnergyCut;
  double m_minCaloCellEnergy;

  // Declare some counters and initialize them to zero
  unsigned long m_nEventsProcessed;
  unsigned long m_nCellsProcessed;
  unsigned long m_nCellsKept;
  unsigned long m_nCellsRemoved;
  unsigned long m_nClustersProcessed;
  unsigned long m_nClustersKept;
  unsigned long m_nClustersRemoved;
  unsigned long m_nPixelCollectionsProcessed;
  unsigned long m_nPixelCollectionsKept;
  unsigned long m_nPixelCollectionsRemoved;
  unsigned long m_nPixelClustersProcessed;
  unsigned long m_nPixelClustersKept;
  unsigned long m_nPixelClustersRemoved;
  unsigned long m_nSCTClusterCollectionsProcessed;
  unsigned long m_nSCTClusterCollectionsKept;
  unsigned long m_nSCTClusterCollectionsRemoved;
  unsigned long m_nSCTClustersProcessed;
  unsigned long m_nSCTClustersKept;
  unsigned long m_nSCTClustersRemoved;
  unsigned long m_nTRTDriftCircleCollectionsProcessed;
  unsigned long m_nTRTDriftCircleCollectionsKept;
  unsigned long m_nTRTDriftCircleCollectionsRemoved;
  unsigned long m_nTRTDriftCirclesProcessed;
  unsigned long m_nTRTDriftCirclesKept;
  unsigned long m_nTRTDriftCirclesRemoved;
  unsigned long m_nTrackParticlesProcessed;
  unsigned long m_nTrackParticlesKept;
  unsigned long m_nTrackParticlesRemoved;
  unsigned long m_nTracksProcessed;
  unsigned long m_nTracksKept;
  unsigned long m_nTracksRemoved;

//Variables to specify region selector boundaries for the Inner Detector
//sub-detectors 
  
  double m_maxDeltaEtaPixelLvl1Roi;
  double m_maxDeltaPhiPixelLvl1Roi;
  double m_maxDeltaZPixelLvl1Roi;
  double m_maxDeltaEtaSCTLvl1Roi;
  double m_maxDeltaPhiSCTLvl1Roi;
  double m_maxDeltaZSCTLvl1Roi;
  double m_maxDeltaEtaTRTLvl1Roi;
  double m_maxDeltaPhiTRTLvl1Roi;
  double m_maxDeltaZTRTLvl1Roi;

  double m_maxDeltaEtaPixelElectron;
  double m_maxDeltaPhiPixelElectron;
  double m_maxDeltaZPixelElectron;
  double m_maxDeltaEtaSCTElectron;
  double m_maxDeltaPhiSCTElectron;
  double m_maxDeltaZSCTElectron;
  double m_maxDeltaEtaTRTElectron;
  double m_maxDeltaPhiTRTElectron;
  double m_maxDeltaZTRTElectron;

  double m_maxDeltaEtaPixelPhoton;
  double m_maxDeltaPhiPixelPhoton;
  double m_maxDeltaZPixelPhoton;
  double m_maxDeltaEtaSCTPhoton;
  double m_maxDeltaPhiSCTPhoton;
  double m_maxDeltaZSCTPhoton;
  double m_maxDeltaEtaTRTPhoton;
  double m_maxDeltaPhiTRTPhoton;
  double m_maxDeltaZTRTPhoton;

  double m_maxDeltaEtaPixelMuon;
  double m_maxDeltaPhiPixelMuon;
  double m_maxDeltaZPixelMuon;
  double m_maxDeltaEtaSCTMuon;
  double m_maxDeltaPhiSCTMuon;
  double m_maxDeltaZSCTMuon;
  double m_maxDeltaEtaTRTMuon;
  double m_maxDeltaPhiTRTMuon;
  double m_maxDeltaZTRTMuon;

  double m_maxDeltaEtaPixelTau;
  double m_maxDeltaPhiPixelTau;
  double m_maxDeltaZPixelTau;
  double m_maxDeltaEtaSCTTau;
  double m_maxDeltaPhiSCTTau;
  double m_maxDeltaZSCTTau;
  double m_maxDeltaEtaTRTTau;
  double m_maxDeltaPhiTRTTau;
  double m_maxDeltaZTRTTau;

  double m_maxDeltaEtaPixelJet;
  double m_maxDeltaPhiPixelJet;
  double m_maxDeltaZPixelJet;
  double m_maxDeltaEtaSCTJet;
  double m_maxDeltaPhiSCTJet;
  double m_maxDeltaZSCTJet;
  double m_maxDeltaEtaTRTJet;
  double m_maxDeltaPhiTRTJet;
  double m_maxDeltaZTRTJet;

};


#endif
