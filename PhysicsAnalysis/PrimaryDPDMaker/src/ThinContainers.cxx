/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//=============================================================================
// Name:        ThinContainers
//
// Author:      Karsten Koeneke (DESY)
// Created:     August 2008
//
// Description: This is a short algorithm to select objects from containers that are
//              near interesting objects (electron, muons, Level 1 RoIs,...).
//=============================================================================

// Preprocessor magic for debugging
#define XXX std::cout << "I am here: " << __FILE__ << ":" << __LINE__ << std::endl;

// This class' header
#include "PrimaryDPDMaker/ThinContainers.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AthLinks/ElementLink.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

// The different particles
#include "EventKernel/INavigable4Momentum.h"
#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/Photon.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "JetEvent/JetCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkTrack/TrackCollection.h" 
#include "TrkTrack/Track.h" 
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h" 
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloEvent/CaloCellLink.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"

#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "Identifier/IdentifierHash.h"


// Needed for deltaR calculation (really?)
#include "FourMomUtils/P4Helpers.h"
#include "FourMom/P4EEtaPhiMBase.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include <math.h>
#include <set>

//--- Needed for the use of RegSelSvc
#include "IRegionSelector/IRegSelSvc.h"
#include "RoiDescriptor/RoiDescriptor.h"
//---


using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;
using CLHEP::pi;


//=============================================================================
// Constructor
//=============================================================================
ThinContainers::ThinContainers(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_thinningSvc( "ThinningSvc",  name ),
  m_trtID(0),
  m_regionSelector("RegSelSvc", name),
  m_nClustersKept(0),
  m_nClustersRemoved(0),
  m_nPixelCollectionsProcessed(0),
  m_nPixelCollectionsKept(0),
  m_nPixelCollectionsRemoved(0),
  m_nPixelClustersProcessed(0),
  m_nPixelClustersKept(0),
  m_nPixelClustersRemoved(0),
  m_nSCTClusterCollectionsProcessed(0),
  m_nSCTClusterCollectionsKept(0),
  m_nSCTClusterCollectionsRemoved(0),
  m_nSCTClustersProcessed(0),
  m_nSCTClustersKept(0),
  m_nSCTClustersRemoved(0),
  m_nTRTDriftCircleCollectionsProcessed(0),
  m_nTRTDriftCircleCollectionsKept(0),
  m_nTRTDriftCircleCollectionsRemoved(0),
  m_nTRTDriftCirclesProcessed(0),
  m_nTRTDriftCirclesKept(0),
  m_nTRTDriftCirclesRemoved(0),
  m_nTrackParticlesProcessed(0),
  m_nTrackParticlesKept(0),
  m_nTrackParticlesRemoved(0),
  m_nTracksProcessed(0),
  m_nTracksKept(0),
  m_nTracksRemoved(0)
{
  // Properties go here
  declareProperty("RegionSelectorTool",      m_regionSelector);		
  declareProperty("thinSvc",                         m_thinningSvc );
  declareProperty("thinCaloCells",                   m_thinCaloCells=false );
  declareProperty("thinPixelClusters",               m_thinPixelClusters=false );
  declareProperty("thinSCTClusters",                 m_thinSCTClusters=false );
  declareProperty("thinTRTDriftCircles",             m_thinTRTDriftCircles=false );
  declareProperty("thinTrackParticles",              m_thinTrackParticles=false );
  declareProperty("thinTracks",                      m_thinTracks=false );
  declareProperty("caloCellCollectionName",          m_caloCellKey="AllCalo" );
  declareProperty("pixelClusterCollectionName",      m_pixelClusterKey="PixelClusters" );
  declareProperty("sctClusterCollectionName",        m_sctClusterKey="SCT_Clusters" );
  declareProperty("trtDriftCirclesCollectionName",   m_trtDriftCircleKey="TRT_DriftCircles" );
  declareProperty("trackParticleCollectionName",     m_trackParticleKey="TrackParticleCandidate" );
  declareProperty("tracksCollectionName",            m_tracksKey="Tracks" );
  declareProperty("Lvl1RoiNames",                    m_Lvl1Items );
  declareProperty("keepObjectsNearLvl1Rois",         m_keepNearLvl1Roi=false );
  declareProperty("maxDeltaRLvl1RoiObject",          m_maxDeltaRLvl1Roi=0.5 );
  declareProperty("useDeltaEtaPhiLvl1Roi",           m_useDeltaEtaPhiLvl1Roi=false );
  declareProperty("maxDeltaEtaLvl1RoiObject",        m_maxDeltaEtaLvl1Roi=0.5 );
  declareProperty("maxDeltaPhiLvl1RoiObject",        m_maxDeltaPhiLvl1Roi=0.5 );
  declareProperty("electronCollectionName",          m_electronKey="ElectronAODCollection" );
  declareProperty("photonCollectionName",            m_photonKey="PhotonAODCollection" );
  declareProperty("muonCollectionNames",             m_muonKeys );
  declareProperty("tauCollectionName",               m_tauKey="TauRecContainer" );
  declareProperty("jetCollectionNames",              m_jetKeys );
  declareProperty("keepObjectsNearElectrons",        m_keepNearElectron=false );
  declareProperty("keepObjectsNearPhotons",          m_keepNearPhoton=false );
  declareProperty("keepObjectsNearMuons",            m_keepNearMuon=false );
  declareProperty("keepObjectsNearTaus",             m_keepNearTau=false );
  declareProperty("keepObjectsNearJets",             m_keepNearJet=false );
  declareProperty("maxDeltaRElectronObject",         m_maxDeltaRElectron=0.5 );
  declareProperty("useDeltaEtaPhiElectron",          m_useDeltaEtaPhiElectron=false );
  declareProperty("maxDeltaEtaElectronObject",       m_maxDeltaEtaElectron=0.5 );
  declareProperty("maxDeltaPhiElectronObject",       m_maxDeltaPhiElectron=0.5 );
  declareProperty("maxDeltaRPhotonObject",           m_maxDeltaRPhoton=0.5 );
  declareProperty("useDeltaEtaPhiPhoton",            m_useDeltaEtaPhiPhoton=false );
  declareProperty("maxDeltaEtaPhotonObject",         m_maxDeltaEtaPhoton=0.5 );
  declareProperty("maxDeltaPhiPhotonObject",         m_maxDeltaPhiPhoton=0.5 );
  declareProperty("maxDeltaRMuonObject",             m_maxDeltaRMuon=0.5 );
  declareProperty("useDeltaEtaPhiMuon",              m_useDeltaEtaPhiMuon=false );
  declareProperty("maxDeltaEtaMuonObject",           m_maxDeltaEtaMuon=0.5 );
  declareProperty("maxDeltaPhiMuonObject",           m_maxDeltaPhiMuon=0.5 );
  declareProperty("maxDeltaRTauObject",              m_maxDeltaRTau=0.5 );
  declareProperty("useDeltaEtaPhiTau",               m_useDeltaEtaPhiTau=false );
  declareProperty("maxDeltaEtaTauObject",            m_maxDeltaEtaTau=0.5 );
  declareProperty("maxDeltaPhiTauObject",            m_maxDeltaPhiTau=0.5 );
  declareProperty("maxDeltaRJetObject",              m_maxDeltaRJet=0.5 );
  declareProperty("useDeltaEtaPhiJet",               m_useDeltaEtaPhiJet=false );
  declareProperty("maxDeltaEtaJetObject",            m_maxDeltaEtaJet=0.5 );
  declareProperty("maxDeltaPhiJetObject",            m_maxDeltaPhiJet=0.5 );
  declareProperty("minEtElectron",                   m_minEtElectron=0.0 );
  declareProperty("minEtPhoton",                     m_minEtPhoton=0.0 );
  declareProperty("minEtMuon",                       m_minEtMuon=0.0 );
  declareProperty("minEtTau",                        m_minEtTau=0.0 );
  declareProperty("minEtJet",                        m_minEtJet=0.0 );
  declareProperty("keepPRDFromTracks",               m_keepPRDFromTracks=false );
  declareProperty("trackContainerNamesForPRD",       m_trackForPRDKeys );
  declareProperty("keepCellsFromLinkContainers",     m_keepCellsFromLinkContainers=false );
  declareProperty("cellLinkContainerNames",          m_cellLinkKeys );
  declareProperty("useEGammaClusterPosition",        m_useEGammaClusterPosition=false );
  declareProperty("treatBadTracks",                  m_treatBadTracks=true );
  declareProperty("wideDeltaEtaForBadTracks",        m_wideDeltaEtaForBadTracks=1.5 );
  declareProperty("keepForwardElectronCells",        m_keepForwardElectronCells=false);
  declareProperty("keepOnlyForwardElectronClusters", m_keepOnlyForwardElectronClusters=false);
  declareProperty("caloClusterContainerKey",         m_caloClusterKey="CaloCalTopoCluster");

  //---Properties to make use of RegSelSvc
  declareProperty("maxDeltaEtaPixelLvl1RoiObject", m_maxDeltaEtaPixelLvl1Roi=0.5 );
  declareProperty("maxDeltaPhiPixelLvl1RoiObject", m_maxDeltaPhiPixelLvl1Roi=0.5 );
  declareProperty("maxDeltaZPixelLvl1RoiObject",   m_maxDeltaZPixelLvl1Roi=400. );
  declareProperty("maxDeltaEtaSCTLvl1RoiObject", m_maxDeltaEtaSCTLvl1Roi=0.5 );
  declareProperty("maxDeltaPhiSCTLvl1RoiObject", m_maxDeltaPhiSCTLvl1Roi=0.5 );
  declareProperty("maxDeltaZSCTLvl1RoiObject",   m_maxDeltaZSCTLvl1Roi=400. );
  declareProperty("maxDeltaEtaTRTLvl1RoiObject", m_maxDeltaEtaTRTLvl1Roi=0.5 );
  declareProperty("maxDeltaPhiTRTLvl1RoiObject", m_maxDeltaPhiTRTLvl1Roi=0.5 );
  declareProperty("maxDeltaZTRTLvl1RoiObject",   m_maxDeltaZTRTLvl1Roi=400. );
   
  declareProperty("maxDeltaEtaPixelElectronObject", m_maxDeltaEtaPixelElectron=0.5 );
  declareProperty("maxDeltaPhiPixelElectronObject", m_maxDeltaPhiPixelElectron=0.5 );
  declareProperty("maxDeltaZPixelElectronObject",   m_maxDeltaZPixelElectron=400. );
  declareProperty("maxDeltaEtaSCTElectronObject", m_maxDeltaEtaSCTElectron=0.5 );
  declareProperty("maxDeltaPhiSCTElectronObject", m_maxDeltaPhiSCTElectron=0.5 );
  declareProperty("maxDeltaZSCTElectronObject",   m_maxDeltaZSCTElectron=400. );
  declareProperty("maxDeltaEtaTRTElectronObject", m_maxDeltaEtaTRTElectron=0.5 );
  declareProperty("maxDeltaPhiTRTElectronObject", m_maxDeltaPhiTRTElectron=0.5 );
  declareProperty("maxDeltaZTRTElectronObject",   m_maxDeltaZTRTElectron=400. );

  declareProperty("maxDeltaEtaPixelMuonObject", m_maxDeltaEtaPixelMuon=0.5 );
  declareProperty("maxDeltaPhiPixelMuonObject", m_maxDeltaPhiPixelMuon=0.5 );
  declareProperty("maxDeltaZPixelMuonObject",   m_maxDeltaZPixelMuon=400. );
  declareProperty("maxDeltaEtaSCTMuonObject", m_maxDeltaEtaSCTMuon=0.5 );
  declareProperty("maxDeltaPhiSCTMuonObject", m_maxDeltaPhiSCTMuon=0.5 );
  declareProperty("maxDeltaZSCTMuonObject",   m_maxDeltaZSCTMuon=400. );
  declareProperty("maxDeltaEtaTRTMuonObject", m_maxDeltaEtaTRTMuon=0.5 );
  declareProperty("maxDeltaPhiTRTMuonObject", m_maxDeltaPhiTRTMuon=0.5 );
  declareProperty("maxDeltaZTRTMuonObject",   m_maxDeltaZTRTMuon=400. );

  declareProperty("maxDeltaEtaPixelPhotonObject", m_maxDeltaEtaPixelPhoton=0.5 );
  declareProperty("maxDeltaPhiPixelPhotonObject", m_maxDeltaPhiPixelPhoton=0.5 );
  declareProperty("maxDeltaZPixelPhotonObject",   m_maxDeltaZPixelPhoton=400. );
  declareProperty("maxDeltaEtaSCTPhotonObject", m_maxDeltaEtaSCTPhoton=0.5 );
  declareProperty("maxDeltaPhiSCTPhotonObject", m_maxDeltaPhiSCTPhoton=0.5 );
  declareProperty("maxDeltaZSCTPhotonObject",   m_maxDeltaZSCTPhoton=400. );
  declareProperty("maxDeltaEtaTRTPhotonObject", m_maxDeltaEtaTRTPhoton=0.5 );
  declareProperty("maxDeltaPhiTRTPhotonObject", m_maxDeltaPhiTRTPhoton=0.5 );
  declareProperty("maxDeltaZTRTPhotonObject",   m_maxDeltaZTRTPhoton=400. );

  declareProperty("maxDeltaEtaPixelTauObject", m_maxDeltaEtaPixelTau=0.5 );
  declareProperty("maxDeltaPhiPixelTauObject", m_maxDeltaPhiPixelTau=0.5 );
  declareProperty("maxDeltaZPixelTauObject",   m_maxDeltaZPixelTau=400. );
  declareProperty("maxDeltaEtaSCTTauObject", m_maxDeltaEtaSCTTau=0.5 );
  declareProperty("maxDeltaPhiSCTTauObject", m_maxDeltaPhiSCTTau=0.5 );
  declareProperty("maxDeltaZSCTTauObject",   m_maxDeltaZSCTTau=400. );
  declareProperty("maxDeltaEtaTRTTauObject", m_maxDeltaEtaTRTTau=0.5 );
  declareProperty("maxDeltaPhiTRTTauObject", m_maxDeltaPhiTRTTau=0.5 );
  declareProperty("maxDeltaZTRTTauObject",   m_maxDeltaZTRTTau=400. );
   
  declareProperty("maxDeltaEtaPixelJetObject", m_maxDeltaEtaPixelJet=0.5 );
  declareProperty("maxDeltaPhiPixelJetObject", m_maxDeltaPhiPixelJet=0.5 );
  declareProperty("maxDeltaZPixelJetObject",   m_maxDeltaZPixelJet=400. );
  declareProperty("maxDeltaEtaSCTJetObject", m_maxDeltaEtaSCTJet=0.5 );
  declareProperty("maxDeltaPhiSCTJetObject", m_maxDeltaPhiSCTJet=0.5 );
  declareProperty("maxDeltaZSCTJetObject",   m_maxDeltaZSCTJet=400. );
  declareProperty("maxDeltaEtaTRTJetObject", m_maxDeltaEtaTRTJet=0.5 );
  declareProperty("maxDeltaPhiTRTJetObject", m_maxDeltaPhiTRTJet=0.5 );
  declareProperty("maxDeltaZTRTJetObject",   m_maxDeltaZTRTJet=400. );

  declareProperty("useCaloCellEnergyCut",    m_useCaloCellEnergyCut=false);
  declareProperty("minCaloCellEnergy",       m_minCaloCellEnergy = 100.0);
}



//=============================================================================
// Destructor
//=============================================================================
ThinContainers::~ThinContainers()
{
}

StatusCode ThinContainers::initialize()
{
  // Print the used configuration
  if (msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG) 
        << "==> initialize " << name() << "..."
        << endmsg;
      
      // Print out the used configuration
      msg(MSG::DEBUG) << " using thinCaloCells                   = " << m_thinCaloCells << endmsg;
      msg(MSG::DEBUG) << " using thinPixelClusters               = " << m_thinPixelClusters << endmsg;
      msg(MSG::DEBUG) << " using thinSCTClusters                 = " << m_thinSCTClusters << endmsg;
      msg(MSG::DEBUG) << " using thinTRTDriftCircles             = " << m_thinTRTDriftCircles << endmsg;
      msg(MSG::DEBUG) << " using thinTrackParticles              = " << m_thinTrackParticles << endmsg;
      msg(MSG::DEBUG) << " using thinTracks                      = " << m_thinTracks << endmsg;
      msg(MSG::DEBUG) << " using caloCellCollectionName          = " << m_caloCellKey << endmsg;
      msg(MSG::DEBUG) << " using pixelClusterCollectionName      = " << m_pixelClusterKey << endmsg;
      msg(MSG::DEBUG) << " using sctClusterCollectionName        = " << m_sctClusterKey << endmsg;
      msg(MSG::DEBUG) << " using trtDriftCirclesCollectionName   = " << m_trtDriftCircleKey << endmsg;
      msg(MSG::DEBUG) << " using trackParticleCollectionName     = " << m_trackParticleKey << endmsg;
      msg(MSG::DEBUG) << " using Lvl1RoiNames                    = " << m_Lvl1Items << endmsg;
      msg(MSG::DEBUG) << " using keepObjectsNearLvl1Rois         = " << m_keepNearLvl1Roi << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaRLvl1RoiObject          = " << m_maxDeltaRLvl1Roi << endmsg;
      msg(MSG::DEBUG) << " using useDeltaEtaPhiLvl1Roi           = " << m_useDeltaEtaPhiLvl1Roi << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaEtaLvl1RoiObject        = " << m_maxDeltaEtaLvl1Roi << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaPhiLvl1RoiObject        = " << m_maxDeltaPhiLvl1Roi << endmsg;
      msg(MSG::DEBUG) << " using electronCollectionName          = " << m_electronKey << endmsg;
      msg(MSG::DEBUG) << " using photonCollectionName            = " << m_photonKey << endmsg;
      msg(MSG::DEBUG) << " using muonCollectionNames             = " << m_muonKeys << endmsg;
      msg(MSG::DEBUG) << " using tauCollectionName               = " << m_tauKey << endmsg;
      msg(MSG::DEBUG) << " using jetCollectionNames              = " << m_jetKeys << endmsg;
      msg(MSG::DEBUG) << " using keepObjectsNearElectrons        = " << m_keepNearElectron << endmsg;
      msg(MSG::DEBUG) << " using keepObjectsNearPhotons          = " << m_keepNearPhoton << endmsg;
      msg(MSG::DEBUG) << " using keepObjectsNearMuons            = " << m_keepNearMuon << endmsg;
      msg(MSG::DEBUG) << " using keepObjectsNearTaus             = " << m_keepNearTau << endmsg;
      msg(MSG::DEBUG) << " using keepObjectsNearJets             = " << m_keepNearJet << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaRElectronObject         = " << m_maxDeltaRElectron << endmsg;
      msg(MSG::DEBUG) << " using useDeltaEtaPhiElectron          = " << m_useDeltaEtaPhiElectron << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaEtaElectronObject       = " << m_maxDeltaEtaElectron << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaPhiElectronObject       = " << m_maxDeltaPhiElectron << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaRPhotonObject           = " << m_maxDeltaRPhoton << endmsg;
      msg(MSG::DEBUG) << " using useDeltaEtaPhiPhoton            = " << m_useDeltaEtaPhiPhoton << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaEtaPhotonObject         = " << m_maxDeltaEtaPhoton << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaPhiPhotonObject         = " << m_maxDeltaPhiPhoton << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaRMuonObject             = " << m_maxDeltaRMuon << endmsg;
      msg(MSG::DEBUG) << " using useDeltaEtaPhiMuon              = " << m_useDeltaEtaPhiMuon << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaEtaMuonObject           = " << m_maxDeltaEtaMuon << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaPhiMuonObject           = " << m_maxDeltaPhiMuon << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaRTauObject              = " << m_maxDeltaRTau << endmsg;
      msg(MSG::DEBUG) << " using useDeltaEtaPhiTau               = " << m_useDeltaEtaPhiTau << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaEtaTauObject            = " << m_maxDeltaEtaTau << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaPhiTauObject            = " << m_maxDeltaPhiTau << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaRJetObject              = " << m_maxDeltaRJet << endmsg;
      msg(MSG::DEBUG) << " using useDeltaEtaPhiJet               = " << m_useDeltaEtaPhiJet << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaEtaJetObject            = " << m_maxDeltaEtaJet << endmsg;
      msg(MSG::DEBUG) << " using maxDeltaPhiJetObject            = " << m_maxDeltaPhiJet << endmsg;
      msg(MSG::DEBUG) << " using minEtElectron                   = " << m_minEtElectron << endmsg;
      msg(MSG::DEBUG) << " using minEtPhoton                     = " << m_minEtPhoton << endmsg;
      msg(MSG::DEBUG) << " using minEtMuon                       = " << m_minEtMuon << endmsg;
      msg(MSG::DEBUG) << " using minEtTau                        = " << m_minEtTau << endmsg;
      msg(MSG::DEBUG) << " using minEtJet                        = " << m_minEtJet << endmsg;
      msg(MSG::DEBUG) << " using keepPRDFromTracks               = " << m_keepPRDFromTracks << endmsg;
      msg(MSG::DEBUG) << " using trackContainerNamesForPRD       = " << m_trackForPRDKeys << endmsg;
      msg(MSG::DEBUG) << " using keepCellsFromLinkContainers     = " << m_keepCellsFromLinkContainers << endmsg;
      msg(MSG::DEBUG) << " using cellLinkContainerNames          = " << m_cellLinkKeys << endmsg;
      msg(MSG::DEBUG) << " using useEGammaClusterPosition        = " << m_useEGammaClusterPosition << endmsg;
      msg(MSG::DEBUG) << " using treatBadTracks                  = " << m_treatBadTracks << endmsg;
      msg(MSG::DEBUG) << " using wideDeltaEtaForBadTracks        = " << m_wideDeltaEtaForBadTracks << endmsg;
      msg(MSG::DEBUG) << " using keepForwardElectronCells        = " << m_keepForwardElectronCells << endmsg;
      msg(MSG::DEBUG) << " using keepOnlyForwardElectronClusters = " << m_keepOnlyForwardElectronClusters << endmsg;
      msg(MSG::DEBUG) << " using caloClusterContainerKey         = " << m_caloClusterKey << endmsg;
      msg(MSG::DEBUG) << " using useCaloCellEnergyCut            = " << m_useCaloCellEnergyCut << endmsg;
      msg(MSG::DEBUG) << " using minCaloCellEnergy               = " << m_minCaloCellEnergy << endmsg;
    }

  // The standard status code
  StatusCode sc(StatusCode::SUCCESS);
 
  // Get pointer to Region Selector service                                                       
  // m_regionSelector is of type IRegSelSvc                                                       
  ATH_CHECK( m_regionSelector.retrieve() );

  
  // Get pointer to ThinningSvc and cache it :
  // m_thinningSvc is of type IThinningSvc
  ATH_CHECK( m_thinningSvc.retrieve() );


  // Get TRT_DetectorManager and identifier
  if ( m_thinTRTDriftCircles ) 
    {
      ATH_CHECK( detStore()->retrieve( m_trtID, "TRT_ID" ) );
    }


  // Initialize the counters to zero
  m_nEventsProcessed = 0;
  m_nCellsProcessed  = 0;
  m_nCellsKept       = 0;
  m_nCellsRemoved    = 0;
  m_nClustersProcessed  = 0;
  m_nClustersKept       = 0;
  m_nClustersRemoved    = 0;
  m_nPixelCollectionsProcessed = 0;
  m_nPixelCollectionsKept      = 0;
  m_nPixelCollectionsRemoved   = 0;
  m_nPixelClustersProcessed = 0;
  m_nPixelClustersKept      = 0;
  m_nPixelClustersRemoved   = 0;
  m_nSCTClusterCollectionsProcessed = 0;
  m_nSCTClusterCollectionsKept      = 0;
  m_nSCTClusterCollectionsRemoved   = 0;
  m_nSCTClustersProcessed = 0;
  m_nSCTClustersKept      = 0;
  m_nSCTClustersRemoved   = 0;
  m_nTRTDriftCircleCollectionsProcessed = 0;
  m_nTRTDriftCircleCollectionsKept      = 0;
  m_nTRTDriftCircleCollectionsRemoved   = 0;
  m_nTRTDriftCirclesProcessed = 0;
  m_nTRTDriftCirclesKept      = 0;
  m_nTRTDriftCirclesRemoved   = 0;
  m_nTrackParticlesProcessed = 0;
  m_nTrackParticlesKept      = 0;
  m_nTrackParticlesRemoved   = 0;
  m_nTracksProcessed = 0;
  m_nTracksKept      = 0;
  m_nTracksRemoved   = 0;

  return sc;
}





//=============================================================================
// Athena execute event method
//=============================================================================
StatusCode ThinContainers::execute() 
{
  ++m_nEventsProcessed;
  
  // Simple status message at the beginning of each event execute
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );
  


  // Clear the list of good containers from the previous event
  m_goodCellIDs.clear();
  m_goodPixelIDs.clear();
  m_goodSCTClusterIDs.clear();
  m_goodTRTDriftCircleIDs.clear();
  m_goodTrackParticleIDs.clear();
  m_goodTrackIDs.clear();

  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);


  // retrive the cell container
  const CaloCellContainer* cellContainer = 0;
  if ( m_thinCaloCells || m_keepForwardElectronCells ) 
    {
      sc = evtStore()->retrieve(cellContainer, m_caloCellKey);
      if ( sc.isFailure() || !cellContainer ) 
        {
          ATH_MSG_WARNING( "Container '" << m_caloCellKey 
                           << "' could not be retrieved from StoreGate!" 
                           << " Will NOT use this container for thinning in all subsequent processing!!!"
                           << " Please fix your job options..." );
          sc = StatusCode::SUCCESS;
          m_thinCaloCells = false;
          m_keepForwardElectronCells = false;
        } 
      else
        {
          ATH_MSG_DEBUG( "Container '" << m_caloCellKey 
                         << "' retrieved from StoreGate" );
        }
    }


  // retrive the cluster container
  const CaloClusterContainer* clusterContainer = 0;
  if ( m_keepOnlyForwardElectronClusters ) 
    {
      sc = evtStore()->retrieve(clusterContainer, m_caloClusterKey);
      if ( sc.isFailure() || !clusterContainer ) 
        {
          ATH_MSG_WARNING( "Container '" << m_caloClusterKey 
                           << "' could not be retrieved from StoreGate !" 
                           << " Will NOT use this container for thinning in all subsequent processing!!!"
                           << " Please fix your job options..." );
          sc = StatusCode::SUCCESS;
          m_keepOnlyForwardElectronClusters = false;
        } 
      else
        {
          ATH_MSG_DEBUG("Container '" << m_caloClusterKey 
                        << "' retrieved from StoreGate" );
        }
    }


  // retrive the PixelClusterContainer
  const InDet::PixelClusterContainer* pixelContainer = 0;
  if ( m_thinPixelClusters ) 
    {
      sc = evtStore()->retrieve(pixelContainer, m_pixelClusterKey);
      if ( sc.isFailure() || !pixelContainer ) 
        {
          ATH_MSG_WARNING( "Container '" << m_pixelClusterKey 
                           << "' could not be retrieved from StoreGate !" 
                           << " Will NOT use this container for thinning in all subsequent processing!!!"
                           << " Please fix your job options..." );
          sc = StatusCode::SUCCESS;
          m_thinPixelClusters = false;
        } 
      else
        {
          ATH_MSG_DEBUG( "Container '" << m_pixelClusterKey 
                         << "' retrieved from StoreGate" );
        }
    }


  // retrive the SCT_ClusterContainer
  const InDet::SCT_ClusterContainer* sctContainer = 0;
  if ( m_thinSCTClusters ) 
    {
      sc = evtStore()->retrieve(sctContainer, m_sctClusterKey);
      if ( sc.isFailure() || !sctContainer ) 
        {
          ATH_MSG_WARNING(  "Container '" << m_sctClusterKey 
                            << "' could not be retrieved from StoreGate !" 
                            << " Will NOT use this container for thinning in all subsequent processing!!!"
                            << " Please fix your job options..." );
          sc = StatusCode::SUCCESS;
          m_thinSCTClusters = false; 
        }
      else
        {
          ATH_MSG_DEBUG( "Container '" << m_sctClusterKey 
                         << "' retrieved from StoreGate" );
        }
    }


  // retrive the TRT_DriftCircleContainer
  const InDet::TRT_DriftCircleContainer* trtContainer = 0;
  if ( m_thinTRTDriftCircles ) 
    {
      sc = evtStore()->retrieve(trtContainer, m_trtDriftCircleKey);
      if ( sc.isFailure() || !trtContainer ) 
        {
          ATH_MSG_WARNING( "Container '" << m_trtDriftCircleKey 
                           << "' could not be retrieved from StoreGate !" 
                           << " Will NOT use this container for thinning in all subsequent processing!!!"
                           << " Please fix your job options..." );
          sc = StatusCode::SUCCESS;
          m_thinTRTDriftCircles = false;
        }
      else
        {
          ATH_MSG_DEBUG( "Container '" << m_trtDriftCircleKey 
                         << "' retrieved from StoreGate" );
        }
    }


  // retrive the TrackParticle Container
  const Rec::TrackParticleContainer* trackParticleContainer = 0;
  if ( m_thinTrackParticles ) 
    {
      sc = evtStore()->retrieve(trackParticleContainer, m_trackParticleKey);
      if ( sc.isFailure() || !trackParticleContainer ) 
        {
          ATH_MSG_WARNING( "Container '" << m_trackParticleKey 
                           << "' could not be retrieved from StoreGate !" 
                           << " Will NOT use this container for thinning in all subsequent processing!!!"
                           << " Please fix your job options..." );
          sc = StatusCode::SUCCESS;
          m_thinTrackParticles = false;
        }
      else
        {
          ATH_MSG_DEBUG( "Container '" << m_trackParticleKey 
                         << "' retrieved from StoreGate" );
        }
    }

  // retrive the Tracks Container
  const TrackCollection* trackCollection;
  if ( m_thinTracks ) 
    {
      sc = evtStore()->retrieve(trackCollection, m_tracksKey);
      if ( sc.isFailure() || !trackCollection ) 
        {
          ATH_MSG_WARNING( "Container '" << m_tracksKey 
                           << "' could not be retrieved from StoreGate !" 
                           << " Will NOT use this container for thinning in all subsequent processing!!!"
                           << " Please fix your job options..." );
          sc = StatusCode::SUCCESS;
          m_thinTracks = false;
        }
      else
        {
          ATH_MSG_DEBUG( "Container '" << m_tracksKey 
                         << "' retrieved from StoreGate" );
        }
    }
   
  //Retrieve the Level 1 RoIs 
  const LVL1_ROI* lvl1Roi(NULL);
  if ( m_keepNearLvl1Roi )
    {
      sc = evtStore()->retrieve( lvl1Roi, "LVL1_ROI" );
      if ( sc.isFailure() || !lvl1Roi )
        {
          ATH_MSG_WARNING( "Could not be retrieve LVL1_ROI from StoreGate!"
                           << " It Will NOT be used in further processing!!!" );
          m_keepNearLvl1Roi = false;
          sc = StatusCode::SUCCESS;
        } 
      else
        {
          ATH_MSG_DEBUG( "LVL1_ROI retrieved from StoreGate" );
        }
    } // End: if ( m_keepNearLvl1Roi )


  //Retrieve the electron container
  const ElectronContainer* electronContainer = 0;
  if ( m_keepNearElectron || m_keepForwardElectronCells || m_keepOnlyForwardElectronClusters )
    {
      sc = evtStore()->retrieve( electronContainer, m_electronKey);
      if ( sc.isFailure() || !electronContainer )
        {
          ATH_MSG_WARNING( "Container '" << m_electronKey 
                           << "' could not be retrieved from StoreGate!"
                           << " It Will NOT be used in further processing!!!" );
          m_keepNearElectron = false;
          m_keepForwardElectronCells =false;
          m_keepOnlyForwardElectronClusters = false;
          sc = StatusCode::SUCCESS;
        } 
      else
        {
          ATH_MSG_DEBUG( "Container '" << m_electronKey 
                         << "' retrieved from StoreGate" );
        }
    } // End: if ( m_keepNearElectron )


  //Retrieve the photon container
  const PhotonContainer* photonContainer = 0;
  if ( m_keepNearPhoton )
    {
      sc = evtStore()->retrieve( photonContainer, m_photonKey);
      if ( sc.isFailure() || !photonContainer )
        {
          ATH_MSG_WARNING( "Container '" << m_photonKey 
                           << "' could not be retrieved from StoreGate!"
                           << " It Will NOT be used in further processing!!!" );
          m_keepNearPhoton = false;
          sc = StatusCode::SUCCESS;
        } 
      else
        {
          ATH_MSG_DEBUG( "Container '" << m_photonKey 
                         << "' retrieved from StoreGate" );
        }
    } // End: if ( m_keepNearPhoton )
   

  //Retrieve the muon containers
  std::vector<const Analysis::MuonContainer*> muonContainers;
  if ( m_keepNearMuon )
    {
      for ( unsigned int i=0; i<m_muonKeys.size(); ++i )
        {
          const Analysis::MuonContainer* tmpMuonContainer;
          sc = evtStore()->retrieve( tmpMuonContainer, m_muonKeys[i]);
          if ( sc.isFailure() || !tmpMuonContainer )
            {
              ATH_MSG_WARNING( "Container '" << m_muonKeys[i] 
                               << "' could not be retrieved from StoreGate!"
                               << " It Will NOT be used in further processing!!!" );
              sc = StatusCode::SUCCESS;
              m_keepNearMuon = false;
            }
          else
            {
              ATH_MSG_DEBUG( "Container '" << m_muonKeys[i]
                             << "' retrieved from StoreGate" );
              muonContainers.push_back(tmpMuonContainer);
            }
        }
      if(muonContainers.size() == 0)	m_keepNearMuon = false;
    } // End: if ( m_keepNearMuon )
   

  //Retrieve the tau container
  const Analysis::TauJetContainer* tauContainer = 0;
  if ( m_keepNearTau )
    {
      sc = evtStore()->retrieve( tauContainer, m_tauKey);
      if ( sc.isFailure() || !tauContainer )
        {
          ATH_MSG_WARNING( "Container '" << m_tauKey 
                           << "' could not be retrieved from StoreGate!"
                           << " It Will NOT be used in further processing!!!" );
          m_keepNearTau = false;
          sc = StatusCode::SUCCESS;
        } 
      else
        {
          ATH_MSG_DEBUG( "Container '" << m_tauKey 
                         << "' retrieved from StoreGate" );
        }
    } // End: if ( m_keepNearTau )


  //Retrieve the jet containers
  std::vector<const JetCollection*> jetContainer;
  if ( m_keepNearJet )
    {
      for ( unsigned int i=0; i<m_jetKeys.size(); ++i )
        {
          const JetCollection* tmpJetContainer;
          sc = evtStore()->retrieve( tmpJetContainer, m_jetKeys[i]);
          if ( sc.isFailure() || !tmpJetContainer )
            {
              ATH_MSG_WARNING( "Container '" << m_jetKeys[i] 
                               << "' could not be retrieved from StoreGate!"
                               << " It Will NOT be used in further processing!!!" );
              sc = StatusCode::SUCCESS;
            }
          else
            {
              jetContainer.push_back( tmpJetContainer );
              ATH_MSG_DEBUG( "Container '" << m_jetKeys[i] 
                             << "' retrieved from StoreGate" );
            }
        } // End: loop over number of given jet collections
      if ( jetContainer.size() == 0 )
        {
          m_keepNearJet = false;
        }
    } // End: if ( m_keepNearJet )



  //Retrieve the track containers for checking PRDs
  std::vector<const TrackCollection*> trackForPRDContainers;
  if ( m_keepPRDFromTracks 
       && ( m_thinPixelClusters || m_thinSCTClusters || m_thinTRTDriftCircles ) )
    {
      for ( unsigned int i=0; i<m_trackForPRDKeys.size(); ++i )
        {
          const TrackCollection* tmpTrackForPRDContainer;
          sc = evtStore()->retrieve( tmpTrackForPRDContainer, m_trackForPRDKeys[i]);
          if ( sc.isFailure() || !tmpTrackForPRDContainer )
            {
              ATH_MSG_WARNING( "Container '" << m_trackForPRDKeys[i] 
                               << "' could not be retrieved from StoreGate!"
                               << " It Will NOT be used in further processing!!!" );
              sc = StatusCode::SUCCESS;
              m_keepPRDFromTracks = false;
            }
          else
            {
              ATH_MSG_DEBUG( "Container '" << m_trackForPRDKeys[i]
                             << "' retrieved from StoreGate" );
              trackForPRDContainers.push_back(tmpTrackForPRDContainer);
            }
        }
      if( trackForPRDContainers.size() == 0 )
        {
          m_keepPRDFromTracks = false;
          ATH_MSG_WARNING( "No Track Containers retrieved from StoreGate. Won't use for further processing!!!" );
        }
    } // End: if ( m_keepPRDFromTracks )

  


  //Retrieve the cell link containers
  std::vector<const CaloCellLinkContainer*> cellLinkContainer;
  if ( m_thinCaloCells && m_keepCellsFromLinkContainers )
    {
      for ( unsigned int i=0; i<m_cellLinkKeys.size(); ++i )
        {
          const CaloCellLinkContainer* tmpCellLinkContainer;
          sc = evtStore()->retrieve( tmpCellLinkContainer, m_cellLinkKeys[i]);
          if ( sc.isFailure() || !tmpCellLinkContainer )
            {
              ATH_MSG_WARNING( "Container '" << m_cellLinkKeys[i] 
                               << "' could not be retrieved from StoreGate!"
                               << " It Will NOT be used in further processing!!!" );
              sc = StatusCode::SUCCESS;
            }
          else
            {
              cellLinkContainer.push_back( tmpCellLinkContainer );
              ATH_MSG_DEBUG( "Container '" << m_cellLinkKeys[i] 
                             << "' retrieved from StoreGate" );
            }
        } // End: loop over number of given cellLink collections
      if ( cellLinkContainer.size() == 0 )
        {
          m_keepCellsFromLinkContainers = false;
        }
    } // End: if ( m_keepNearCellLink )
  



  //Retrieve the vertex container to get the position of the vertex
  const VxContainer* importedVxContainer = 0;
  static const std::string containerName = "VxPrimaryCandidate";
  Hep3Vector VertexPos(0.0,0.0,0.0);
  if ( evtStore()->contains<VxContainer>(containerName))
    {
      if ( StatusCode::SUCCESS != evtStore()->retrieve(importedVxContainer,containerName))
        {
          // in general this means this container won't be around at all for this run
          ATH_MSG_WARNING( "No " << containerName
                           << " found in StoreGate."
                           << " Will use the default 0,0,0 vertex position instead!" );
          sc = StatusCode::SUCCESS;
        } 
      else
        {
          //Get the primary vertex 
          const Trk::Vertex& vertex = importedVxContainer->at(0)->recVertex();
          VertexPos.setX(vertex.position().x());
          VertexPos.setY(vertex.position().y());
          VertexPos.setZ(vertex.position().z());
        }
    }
  






  //-----------------------------------------
  // Search for objects near Level 1 RoIs
  //-----------------------------------------
  if ( m_keepNearLvl1Roi )
    {
      // Loop over given Level 1 Item names
      for ( unsigned int i=0; i<m_Lvl1Items.size(); ++i )
        {
          //Loop over Level 1 RoIs...
          LVL1_ROI::emtaus_type::const_iterator roiItr    = lvl1Roi->getEmTauROIs().begin();
          LVL1_ROI::emtaus_type::const_iterator roiItrEnd = lvl1Roi->getEmTauROIs().end();
          for ( ; roiItr != roiItrEnd; ++roiItr )
            {
              // Check if this RoI also comes from the given Items
              bool checkROI = false;
              std::vector<std::string>::const_iterator namesItr = roiItr->getThresholdNames().begin();
              for ( ; namesItr != roiItr->getThresholdNames().end(); ++namesItr )
                {	
                  if ( m_Lvl1Items[i] == (*namesItr) )
                    {
                      checkROI = true;
                    }
                }
              if ( checkROI )
                {
                  const double em_eta  = (*roiItr).getEta();
                  const double em_phi  = (*roiItr).getPhi();
                  P4EEtaPhiM myP4(10000.0, em_eta, em_phi, 0.0);
		   
                  ATH_MSG_DEBUG( "Found ROI with name=" << m_Lvl1Items[i]
                                 << " and eta=" << em_eta << " and phi=" << em_phi 
                                 << " and EMClus=" << (*roiItr).getEMClus() );

                  if ( m_thinCaloCells ) 
                    {
                      ATH_CHECK( findGoodCells(cellContainer, myP4, m_maxDeltaRLvl1Roi,
                                               m_useDeltaEtaPhiLvl1Roi,
                                               m_maxDeltaEtaLvl1Roi,
                                               m_maxDeltaPhiLvl1Roi) );
                    }
                  if ( m_thinPixelClusters ) 
                    {
                      ATH_CHECK( findGoodIDObjects(PIXEL, myP4.hlv(), VertexPos,
                                                   m_maxDeltaEtaPixelLvl1Roi,
                                                   m_maxDeltaPhiPixelLvl1Roi,
                                                   m_maxDeltaZPixelLvl1Roi) );
                    }
                  if ( m_thinSCTClusters ) 
                    {
                      ATH_CHECK( findGoodIDObjects(SCT, myP4.hlv(), VertexPos,
                                                   m_maxDeltaEtaSCTLvl1Roi,
                                                   m_maxDeltaPhiSCTLvl1Roi,
                                                   m_maxDeltaZSCTLvl1Roi) );
                    }
                  if ( m_thinTRTDriftCircles ) 
                    {
                      ATH_CHECK( findGoodIDObjects(TRT, myP4.hlv(), VertexPos, 
                                                   m_maxDeltaEtaTRTLvl1Roi,
                                                   m_maxDeltaPhiTRTLvl1Roi,
                                                   m_maxDeltaZTRTLvl1Roi) );
                    }
                  if ( m_thinTrackParticles ) 
                    {
                      ATH_CHECK( findGoodTrackParticles(trackParticleContainer, myP4,
                                                        m_maxDeltaRLvl1Roi,
                                                        m_useDeltaEtaPhiLvl1Roi,
                                                        m_maxDeltaEtaLvl1Roi,
                                                        m_maxDeltaPhiLvl1Roi) );
                    }
                } // End: if ( checkROI )
           } // End: loop ever Lvl1 RoIs
        } // End: loop over given Level 1 Item names
    } // End: if ( m_keepNearLvl1Roi )
   

  //-----------------------------------------
  // Search for objects near electrons
  //-----------------------------------------
  if ( m_keepNearElectron )
    {
      //Loop over Electrons...
      ElectronContainer::const_iterator eleItr    = electronContainer->begin();
      ElectronContainer::const_iterator eleItrEnd = electronContainer->end();
      for (; eleItr != eleItrEnd; ++eleItr)
        {
          if ( (*eleItr)->et() > m_minEtElectron ) 
            {
              // Also get the associated CaloCluster and if their positions differ, widen the selection
              const CaloCluster* clus = (*eleItr)->cluster();
              double maxDeltaR = m_maxDeltaRElectron;
              double maxDeltaEta = m_maxDeltaEtaElectron;
              double maxDeltaPhi = m_maxDeltaPhiElectron;
              if ( clus )
                {
                  double absDeltaEta = std::fabs( clus->eta() - (*eleItr)->eta() );
                  double absDeltaPhi = std::fabs( clus->phi() - (*eleItr)->phi() );
                  if (absDeltaPhi > pi ) absDeltaPhi = 2*pi - absDeltaPhi;  
                  maxDeltaR = m_maxDeltaRElectron + std::max( absDeltaEta, absDeltaPhi );
                  maxDeltaEta = m_maxDeltaEtaElectron + absDeltaEta;
                  maxDeltaPhi = m_maxDeltaPhiElectron + absDeltaPhi;
                }
              // Now, go and find the objects near electrons
              if ( m_thinCaloCells ) 
                {
                  ATH_CHECK( findGoodCells(cellContainer, **eleItr, maxDeltaR,
                                           m_useDeltaEtaPhiElectron, maxDeltaEta, maxDeltaPhi) );
                }
              if ( m_thinPixelClusters ) 
                {
                  ATH_CHECK( findGoodIDObjects(PIXEL, (*eleItr)->hlv(), VertexPos,
                                               m_maxDeltaEtaPixelElectron,
                                               m_maxDeltaPhiPixelElectron,
                                               m_maxDeltaZPixelElectron) );
                }
              if ( m_thinSCTClusters ) 
                {
                  ATH_CHECK( findGoodIDObjects(SCT, (*eleItr)->hlv(), VertexPos,
                                               m_maxDeltaEtaSCTElectron,
                                               m_maxDeltaPhiSCTElectron,
                                               m_maxDeltaZSCTElectron) );
                }
              if ( m_thinTRTDriftCircles ) 
                {
                  ATH_CHECK( findGoodIDObjects(TRT, (*eleItr)->hlv(), VertexPos, 
                                               m_maxDeltaEtaTRTElectron,
                                               m_maxDeltaPhiTRTElectron,
                                               m_maxDeltaZTRTElectron) );
                }
              if ( m_thinTrackParticles ) 
                {
                  // First, keep the TrackParticles used explicitly by this electron
                  const ElementLink<Rec::TrackParticleContainer> trackLink = (*eleItr)->trackParticleElementLink();
                  if ( trackLink.isValid() )
                    {
                      m_goodTrackParticleIDs.insert( trackLink.index() );
                    }
                  ATH_CHECK( findGoodTrackParticles(trackParticleContainer, **eleItr,
                                                    maxDeltaR,
                                                    m_useDeltaEtaPhiElectron,
                                                    maxDeltaEta, maxDeltaPhi) );
               }
            } // End: if ( > m_minEtElectron ) 
        } // End: loop ever electrons
    } // End: if ( m_keepNearElectron )
   

  //-----------------------------------------
  // Search for objects near photons
  //-----------------------------------------
  if ( m_keepNearPhoton )
    {
      //Loop over Photons...
      PhotonContainer::const_iterator phoItr    = photonContainer->begin();
      PhotonContainer::const_iterator phoItrEnd = photonContainer->end();
      for (; phoItr != phoItrEnd; ++phoItr)
        {
          if ( (*phoItr)->et() > m_minEtPhoton ) 
            {
              // Also get the associated CaloCluster and if their positions differ, widen the selection
              const CaloCluster* clus = (*phoItr)->cluster();
              double maxDeltaR = m_maxDeltaRPhoton;
              double maxDeltaEta = m_maxDeltaEtaPhoton;
              double maxDeltaPhi = m_maxDeltaPhiPhoton;
              if ( clus )
                {
                  double absDeltaEta = std::fabs( clus->eta() - (*phoItr)->eta() );
                  double absDeltaPhi = std::fabs( clus->phi() - (*phoItr)->phi() );
                  if (absDeltaPhi > pi ) absDeltaPhi = 2*pi - absDeltaPhi;  
                  maxDeltaR = m_maxDeltaRPhoton + std::max( absDeltaEta, absDeltaPhi );
                  maxDeltaEta = m_maxDeltaEtaPhoton + absDeltaEta;
                  maxDeltaPhi = m_maxDeltaPhiPhoton + absDeltaPhi;
                }
              // Now, go and find the objects near photons
              if ( m_thinCaloCells ) 
                {
                  ATH_CHECK( findGoodCells(cellContainer, **phoItr, maxDeltaR,
                                           m_useDeltaEtaPhiPhoton, maxDeltaEta, maxDeltaPhi) );
                }
              if ( m_thinPixelClusters ) 
                {
                  ATH_CHECK( findGoodIDObjects(PIXEL, (*phoItr)->hlv(), VertexPos,
                                               m_maxDeltaEtaPixelPhoton,
                                               m_maxDeltaPhiPixelPhoton,
                                               m_maxDeltaZPixelPhoton) );
                }
              if ( m_thinSCTClusters ) 
                {
                  ATH_CHECK( findGoodIDObjects(SCT, (*phoItr)->hlv(), VertexPos,
                                               m_maxDeltaEtaSCTPhoton,
                                               m_maxDeltaPhiSCTPhoton,
                                               m_maxDeltaZSCTPhoton) );
                }
              if ( m_thinTRTDriftCircles ) 
                {
                  ATH_CHECK( findGoodIDObjects(TRT, (*phoItr)->hlv(), VertexPos, 
                                               m_maxDeltaEtaTRTPhoton,
                                               m_maxDeltaPhiTRTPhoton,
                                               m_maxDeltaZTRTPhoton) );
                }
              if ( m_thinTrackParticles ) 
                {
                  // First, keep the TrackParticles used explicitly by this photon
                  const ElementLink<Rec::TrackParticleContainer> trackLink = (*phoItr)->trackParticleElementLink();
                  if ( trackLink.isValid() )
                    {
                      m_goodTrackParticleIDs.insert( trackLink.index() );
                    }
                  // Also, find the TrackParticles of a conversion
                  const Trk::VxCandidate* vxCand = (*phoItr)->conversion();
                  if ( vxCand )
                    {
                      // this gets the list of tracks for this vertex
                      const std::vector<Trk::VxTrackAtVertex*>* trklist = vxCand->vxTrackAtVertex();
                      // loop over those tracks (one or two tracks)
                      for ( unsigned int i=0; i< trklist->size() ; i++ )
                        {       
                          const Trk::VxTrackAtVertex* tmpVxAtVtx = (*trklist)[i];
                          if ( !tmpVxAtVtx ) continue;
                          // Get the link
                          const Trk::ITrackLink* trkLink = tmpVxAtVtx->trackOrParticleLink();
                          if ( trkLink )
                            {
                              // Cast the link to TrackParticleBase
                              const Trk::LinkToTrackParticleBase * linkToTrackPB = 
                                dynamic_cast<const Trk::LinkToTrackParticleBase *>(trkLink); 
                              if ( linkToTrackPB )
                                {
                                  if ( linkToTrackPB->isValid() )
                                    {
                                      // If we have a valid ElementLink, get the index in the 
                                      // TrackParticle container it is pointing to
                                      m_goodTrackParticleIDs.insert( linkToTrackPB->index() );
                                    }
                                }
                            }
                        } // End loop over the associated tracks
                    } // End if vxCand
                  // Find all TrackParticles in the vicinity of the photon
                  ATH_CHECK( findGoodTrackParticles(trackParticleContainer, **phoItr, maxDeltaR,
                                                    m_useDeltaEtaPhiPhoton, maxDeltaEta, maxDeltaPhi) );
                }
            } // End: if ( > m_minEtPhoton ) 
        } // End: loop ever photons
    } // End: if ( m_keepNearPhoton )
   

  //-----------------------------------------
  // Search for objects near muons
  //-----------------------------------------
  if ( m_keepNearMuon )
    {
      for ( unsigned int i=0; i<muonContainers.size(); ++i )
        {
          //Loop over Muons...
          Analysis::MuonContainer::const_iterator muItr    = muonContainers[i]->begin();
          Analysis::MuonContainer::const_iterator muItrEnd = muonContainers[i]->end();
          for (; muItr != muItrEnd; ++muItr)
            {
              if ( (*muItr)->et() > m_minEtMuon ) 
                {
                  if ( m_thinCaloCells ) 
                    {
                      ATH_CHECK( findGoodCells(cellContainer, **muItr, m_maxDeltaRMuon,
                                               m_useDeltaEtaPhiMuon, m_maxDeltaEtaMuon, m_maxDeltaPhiMuon) );
                    }
                  if ( m_thinPixelClusters ) 
                    {
                      ATH_CHECK( findGoodIDObjects(PIXEL, (*muItr)->hlv(), VertexPos,
                                                   m_maxDeltaEtaPixelMuon,
                                                   m_maxDeltaPhiPixelMuon,
                                                   m_maxDeltaZPixelMuon) );
                    }
                  if ( m_thinSCTClusters ) 
                    {
                      ATH_CHECK( findGoodIDObjects(SCT, (*muItr)->hlv(), VertexPos,
                                                   m_maxDeltaEtaSCTMuon,
                                                   m_maxDeltaPhiSCTMuon,
                                                   m_maxDeltaZSCTMuon) );
                    }
                  if ( m_thinTRTDriftCircles ) 
                    {
                      ATH_CHECK( findGoodIDObjects(TRT, (*muItr)->hlv(), VertexPos, 
                                                   m_maxDeltaEtaTRTMuon,
                                                   m_maxDeltaPhiTRTMuon,
                                                   m_maxDeltaZTRTMuon) );
                    }
                  if ( m_thinTrackParticles ) 
                    {
                      ATH_CHECK( findGoodTrackParticles(trackParticleContainer, **muItr, m_maxDeltaRMuon,
                                                        m_useDeltaEtaPhiMuon, m_maxDeltaEtaMuon, m_maxDeltaPhiMuon) );
                    }
                } // End: if ( > m_minEtMuon ) 
            } // End: loop ever muons
        } //End: loop over muon containers
    } // End: if ( m_keepNearMuon )
   

  //-----------------------------------------
  // Search for objects near taus
  //-----------------------------------------
  if ( m_keepNearTau )
    {
      //Loop over Taus...
      Analysis::TauJetContainer::const_iterator tauItr    = tauContainer->begin();
      Analysis::TauJetContainer::const_iterator tauItrEnd = tauContainer->end();
      for (; tauItr != tauItrEnd; ++tauItr)
        {
          if ( (*tauItr)->et() > m_minEtTau ) 
            {
              if ( m_thinCaloCells ) 
                {
                  ATH_CHECK( findGoodCells(cellContainer, **tauItr, m_maxDeltaRTau,
                                           m_useDeltaEtaPhiTau, m_maxDeltaEtaTau, m_maxDeltaPhiTau) );
                }
              if ( m_thinPixelClusters ) 
                {
                  ATH_CHECK( findGoodIDObjects(PIXEL, (*tauItr)->hlv(), VertexPos,
                                               m_maxDeltaEtaPixelTau,
                                               m_maxDeltaPhiPixelTau,
                                               m_maxDeltaZPixelTau) );
                }
              if ( m_thinSCTClusters ) 
                {
                  ATH_CHECK( findGoodIDObjects(SCT, (*tauItr)->hlv(), VertexPos,
                                               m_maxDeltaEtaSCTTau,
                                               m_maxDeltaPhiSCTTau,
                                               m_maxDeltaZSCTTau) );
                }
              if ( m_thinTRTDriftCircles )
                {
                  ATH_CHECK( findGoodIDObjects(TRT, (*tauItr)->hlv(), VertexPos,
                                               m_maxDeltaEtaTRTTau,
                                               m_maxDeltaPhiTRTTau,
                                               m_maxDeltaZTRTTau) );
                }
              if ( m_thinTrackParticles )
                {
                  ATH_CHECK( findGoodTrackParticles(trackParticleContainer, **tauItr, m_maxDeltaRTau,
                                                    m_useDeltaEtaPhiTau, m_maxDeltaEtaTau, m_maxDeltaPhiTau) );
                }
              if ( m_thinTracks )
                {
                  if ( m_nEventsProcessed < 10 ) msg(MSG::DEBUG) << "Thin tracks with Taus" << endmsg;
                  ATH_CHECK( findGoodTracks(trackCollection, (*tauItr)->hlv(), m_maxDeltaRTau,
                                            m_useDeltaEtaPhiTau, m_maxDeltaEtaTau, m_maxDeltaPhiTau) );
                }
	       
            } // End: if ( > m_minEtTau ) 
        } // End: loop ever taus
    } // End: if ( m_keepNearTau )


  //-----------------------------------------
  // Search for objects near jets
  //-----------------------------------------
  if ( m_keepNearJet )
    {
      for ( unsigned int i=0; i<jetContainer.size(); ++i )
        {
          //Loop over Jets...
          JetCollection::const_iterator jetItr    = jetContainer[i]->begin();
          JetCollection::const_iterator jetItrEnd = jetContainer[i]->end();
          for ( ; jetItr != jetItrEnd; ++jetItr )
            {
              if ( (*jetItr)->et() > m_minEtJet ) 
                {
                  if ( m_thinCaloCells ) 
                    {
                      ATH_CHECK( findGoodCells(cellContainer, **jetItr, m_maxDeltaRJet,
                                               m_useDeltaEtaPhiJet, m_maxDeltaEtaJet, m_maxDeltaPhiJet) );
                    }
                  if ( m_thinPixelClusters ) 
                    {
                      ATH_CHECK( findGoodIDObjects(PIXEL, (*jetItr)->hlv(), VertexPos,
                                                   m_maxDeltaEtaPixelJet,
                                                   m_maxDeltaPhiPixelJet,
                                                   m_maxDeltaZPixelJet) );
                    }
                  if ( m_thinSCTClusters ) 
                    {
                      ATH_CHECK( findGoodIDObjects(SCT, (*jetItr)->hlv(), VertexPos,
                                                   m_maxDeltaEtaSCTJet,
                                                   m_maxDeltaPhiSCTJet,
                                                   m_maxDeltaZSCTJet) );
                    }
                  if ( m_thinTRTDriftCircles )
                    {
                      ATH_CHECK( findGoodIDObjects(TRT, (*jetItr)->hlv(), VertexPos, 
                                                   m_maxDeltaEtaTRTJet,
                                                   m_maxDeltaPhiTRTJet,
                                                   m_maxDeltaZTRTJet) );
                    }
                  if ( m_thinTrackParticles )
                    {
                      ATH_CHECK( findGoodTrackParticles(trackParticleContainer, **jetItr, m_maxDeltaRJet,
                                                        m_useDeltaEtaPhiJet, m_maxDeltaEtaJet, m_maxDeltaPhiJet) );
                    }
                } // End: if ( > m_minEtJet ) 
            } // End: loop ever jets
        } // End: loop over jet collections
    } // End: if ( m_keepNearJet )
   


  //-----------------------------------------
  // Keep PRD from the Tracks
  //-----------------------------------------
  if ( m_keepPRDFromTracks 
       && ( m_thinPixelClusters || m_thinSCTClusters || m_thinTRTDriftCircles ) )
    {
      for ( unsigned int i=0; i<trackForPRDContainers.size(); ++i )
        {
          //Loop over the tracks...
          for ( unsigned int j=0; j<(trackForPRDContainers[i])->size(); ++j )
            {
              const Trk::Track* track = (*(trackForPRDContainers[i]))[j];
              if ( track )
                {
                  // Now, check that we want to actually keep this track
                  if( m_thinningSvc->index( trackForPRDContainers[i], j ) != IThinningSvc::RemovedIdx )
                    {
                      // Print a debug message
                      ATH_MSG_DEBUG( "Found a Trk::Track at index j=" << j
                                     << " that is scheduled to be kept!" );

                      // Loop over the hits (PRD)
                      DataVector<const Trk::MeasurementBase>::const_iterator measItr    = track->measurementsOnTrack()->begin();
                      DataVector<const Trk::MeasurementBase>::const_iterator measItrEnd = track->measurementsOnTrack()->end(); 
                      for ( ; measItr != measItrEnd; ++measItr )
                        { 
                          if ( *measItr )
                            {
                              // Avoid pseudo measurements
                              if ( dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(*measItr) ) continue;
                              
                              // Check if we have a PixelCluster at hand
                              if( m_thinPixelClusters )
                                {
                                  const InDet::PixelClusterOnTrack* pixelClusterOnTrack = dynamic_cast<const InDet::PixelClusterOnTrack*>(*measItr);
                                  if( pixelClusterOnTrack )
                                    {
                                      const InDet::PixelCluster* pixelCluster 
                                        = dynamic_cast<const InDet::PixelCluster*>( pixelClusterOnTrack->prepRawData() );
                                      if( pixelCluster )
                                        {
                                          m_goodPixelIDs.push_back( pixelCluster->getHashAndIndex().collHash() );
                                          //m_goodPixelIDs.push_back( pixelCluster->detectorElement()->identifyHash() );
                                        }
                                    } // End: PixelCluster
                                } // End: if( m_thinPixelClusters )
                              
                              // Check if we have a PixelCluster at hand
                              if ( m_thinSCTClusters )
                                {
                                  const InDet::SCT_ClusterOnTrack* sctClusterOnTrack = dynamic_cast<const InDet::SCT_ClusterOnTrack*>(*measItr);
                                  if( sctClusterOnTrack )
                                    {
                                      const InDet::SCT_Cluster* sctCluster 
                                        = dynamic_cast<const InDet::SCT_Cluster*>( sctClusterOnTrack->prepRawData() );
                                      if( sctCluster )
                                        {
                                          m_goodSCTClusterIDs.push_back( sctCluster->getHashAndIndex().collHash() );
                                          //m_goodSCTClusterIDs.push_back( sctCluster->detectorElement()->identifyHash() );
                                        }
                                    } // End: SCTCluster
                                } // End: if ( m_thinSCTClusters )
                              
                              // Check if we have a TRT DriftCircle at hand
                              if ( m_thinTRTDriftCircles )
                                {
                                  const InDet::TRT_DriftCircleOnTrack* trtCircleOnTrack = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(*measItr);
                                  if( trtCircleOnTrack )
                                    {
                                      const InDet::TRT_DriftCircle* trtCircle 
                                        = dynamic_cast<const InDet::TRT_DriftCircle*>( trtCircleOnTrack->prepRawData() );
                                      if( trtCircle )
                                        {
                                          m_goodTRTDriftCircleIDs.push_back( trtCircle->getHashAndIndex().collHash() );
                                          //m_goodTRTDriftCircleIDs.push_back( trtCircle->detectorElement()->identifyHash() );
                                        }
                                    } // End: TRTDriftCircle
                                } // End: if ( m_thinTRTDriftCircles )

                            } // End: if ( measItr )
                        } // End: loop over DataVector<const Trk::MeasurementBase>

                    } // End: if track is not scheduled to be thinned away
                  else
                    {
                      // Print a debug message
                      ATH_MSG_DEBUG( "Found a Trk::Track at index j=" << j
                                     << " that is scheduled to be thinned away!" );
                    } // End: else: track is scheduled to be thinned away
                } // End: if ( track )

            } // End: loop over one TrackCollection
        } // End: loop over track collections
    } // End: if ( keepPRDFromTrack ... )




  //-----------------------------------------
  // Keep cells from the Link Containers
  //-----------------------------------------
  if ( m_thinCaloCells && m_keepCellsFromLinkContainers )
    {
      for ( unsigned int i=0; i<cellLinkContainer.size(); ++i )
        {
          //Loop over the links...
          CaloCellLinkContainer::const_iterator linkItr    = cellLinkContainer[i]->begin();
          CaloCellLinkContainer::const_iterator linkItrEnd = cellLinkContainer[i]->end();
          for ( ; linkItr != linkItrEnd; ++linkItr )
            {
              // Loop over the cells
              CaloCellLink::cell_iterator cellItr    = (*linkItr)->begin();
              CaloCellLink::cell_iterator cellItrEnd = (*linkItr)->end();
              for (; cellItr != cellItrEnd; ++cellItr)
                {
                  m_goodCellIDs.insert( (*cellItr)->ID() );
                  ATH_MSG_DEBUG( "From a CaloCellLinkContainer, found and add a CaloCell:"
                                 << " ID="      << (*cellItr)->ID()
                                 << " energy="  << (*cellItr)->energy()
                                 << " time="    << (*cellItr)->time()
                                 << " quality=" << (*cellItr)->quality()
                                 << " eta="     << (*cellItr)->eta()
                                 << " phi="     << (*cellItr)->phi() );
                } // End loop over cells
            } // End loop over links inside the CaloCellLinkContainer
        } // End: loop over cellLinkContainers
    } // End: if ( m_keepCellsFromLinkContainers )
   


  //-----------------------------------------
  // Keep cluster and/or cells from forward electrons
  //-----------------------------------------
  if ( m_keepForwardElectronCells || m_keepOnlyForwardElectronClusters ) 
    {
      //Loop over Electrons...
      ElectronContainer::const_iterator eleItr    = electronContainer->begin();
      ElectronContainer::const_iterator eleItrEnd = electronContainer->end();
      for ( ; eleItr != eleItrEnd; ++eleItr )
        {
          // Only take forward electrons
          if ( (*eleItr)->author(egammaParameters::AuthorFrwd) ) 
            {
              // keep the associated cluster, if wanted
              if ( m_keepOnlyForwardElectronClusters ) 
                {
                  // Get the ElementLink to the associated CaloCluster
                  ElementLink<CaloClusterContainer> myClusterLink = (*eleItr)->clusterElementLink();
                  if ( myClusterLink.isValid() )
                    {
                      m_goodClusterIDs.insert( ((int)myClusterLink.index()) );
                    } // End: if cluster
                } // End: if ( m_keepOnlyForwardElectronClusters ) 	       

              // keep also the associated cells, if wanted
              if ( m_thinCaloCells || m_keepForwardElectronCells ) 
                {   
                  const CaloCluster* clus = NULL;
                  clus = (*eleItr)->cluster();
                  if ( clus != NULL )
                    {
                      // Protect agains zero cells linked to this cluster
                      int nCells(0);
                      nCells = clus->getNumberOfCells();
                      if ( nCells > 0 )
                        {
                          // Loop over all cells of this cluster
                          CaloCluster::cell_iterator cellItr    = clus->cell_begin();
                          CaloCluster::cell_iterator cellItrEnd = clus->cell_end();
                          for ( ; cellItr != cellItrEnd; ++cellItr )
                            {
                              m_goodCellIDs.insert( (*cellItr)->ID() );
                            } // End: loop over cells
                        } // End: if ( nCells > 0 )
                    } // End: if ( clus )
                }// End: if ( m_thinCaloCells ) 
            } // End: electron author
        } // End: loop over electron container
		   
    } // End: if ( m_keepForwardElectronClusters )




  //-----------------------------------------
  // Now, do the actual thinning of the containers
  //-----------------------------------------
  if ( m_keepOnlyForwardElectronClusters ) 
    {
      ATH_CHECK( thinCaloClusters( clusterContainer ) );
    }
  if ( m_thinCaloCells || m_keepForwardElectronCells ) 
    {
      ATH_CHECK( thinCells(cellContainer) );
    }
  if ( m_thinPixelClusters ) 
    {
      ATH_CHECK( thinPixels(pixelContainer) );
    }
  if ( m_thinSCTClusters ) 
    {
      ATH_CHECK( thinSCTClusters(sctContainer) );
    }
  if ( m_thinTRTDriftCircles ) 
    {
      ATH_CHECK( thinTRTDriftCircles(trtContainer) );
    }
  if ( m_thinTrackParticles ) 
    {
      ATH_CHECK( thinTrackParticles(trackParticleContainer) );
    }
  if ( m_thinTracks ) 
    {
      ATH_CHECK( thinTracks(trackCollection) );
    }
 

  return sc;
}




//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode ThinContainers::finalize() 
{
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // Print info messages
  ATH_MSG_DEBUG( "==> finalize " << name() << "..." );
  ATH_MSG_DEBUG( "***************************************************************" );
  ATH_MSG_DEBUG( "Results of " << name() << " thinning algorithm:" );
  ATH_MSG_DEBUG( "-------------" );
  ATH_MSG_DEBUG( " Number of processed events:  " << m_nEventsProcessed );

  // Figure out what to write out to the log
  int numberOfThinnedContainers = 0;
  std::vector<unsigned long> nProcessed;
  std::vector<unsigned long> nKept;
  std::vector<unsigned long> nRemoved;
  std::vector<unsigned long> nClustersProcessed;
  std::vector<unsigned long> nClustersKept;
  std::vector<unsigned long> nClustersRemoved;
  std::vector<std::string> key;
  std::vector<std::string> object;

  if ( m_keepOnlyForwardElectronClusters )
    {
      numberOfThinnedContainers++;
      nProcessed.push_back(m_nClustersProcessed);
      nKept.push_back(m_nClustersKept);
      nRemoved.push_back(m_nClustersRemoved);
      nClustersProcessed.push_back(0);
      nClustersKept.push_back(0);
      nClustersRemoved.push_back(0);
      key.push_back(m_caloClusterKey);
      object.push_back("CaloClusters");
    }
  if ( m_thinTrackParticles )
    {
      numberOfThinnedContainers++;
      nProcessed.push_back(m_nTrackParticlesProcessed);
      nKept.push_back(m_nTrackParticlesKept);
      nRemoved.push_back(m_nTrackParticlesRemoved);
      nClustersProcessed.push_back(0);
      nClustersKept.push_back(0);
      nClustersRemoved.push_back(0);
      key.push_back(m_trackParticleKey);
      object.push_back("TrackParticles");
    }
  if ( m_thinTracks )
    {
      numberOfThinnedContainers++;
      nProcessed.push_back(m_nTracksProcessed);
      nKept.push_back(m_nTracksKept);
      nRemoved.push_back(m_nTracksRemoved);
      nClustersProcessed.push_back(0);
      nClustersKept.push_back(0);
      nClustersRemoved.push_back(0);
      key.push_back(m_trackParticleKey);
      object.push_back("Tracks");
    }
  if ( m_thinCaloCells || m_keepForwardElectronCells )
    {
      numberOfThinnedContainers++;
      nProcessed.push_back(m_nCellsProcessed);
      nKept.push_back(m_nCellsKept);
      nRemoved.push_back(m_nCellsRemoved);
      nClustersProcessed.push_back(0);
      nClustersKept.push_back(0);
      nClustersRemoved.push_back(0);
      key.push_back(m_caloCellKey);
      object.push_back("cells");
    }
  if ( m_thinPixelClusters ) 
    {
      numberOfThinnedContainers++;
      nProcessed.push_back(m_nPixelCollectionsProcessed);
      nKept.push_back(m_nPixelCollectionsKept);
      nRemoved.push_back(m_nPixelCollectionsRemoved);
      nClustersProcessed.push_back(m_nPixelClustersProcessed);
      nClustersKept.push_back(m_nPixelClustersKept);
      nClustersRemoved.push_back(m_nPixelClustersRemoved);
      key.push_back(m_pixelClusterKey);
      object.push_back("pixels");
    }
  if ( m_thinSCTClusters )
    {
      numberOfThinnedContainers++;
      nProcessed.push_back(m_nSCTClusterCollectionsProcessed);
      nKept.push_back(m_nSCTClusterCollectionsKept);
      nRemoved.push_back(m_nSCTClusterCollectionsRemoved);
      nClustersProcessed.push_back(m_nSCTClustersProcessed);
      nClustersKept.push_back(m_nSCTClustersKept);
      nClustersRemoved.push_back(m_nSCTClustersRemoved);
      key.push_back(m_sctClusterKey);
      object.push_back("SCT_Clusters");
    }
  if ( m_thinTRTDriftCircles )
    {
      numberOfThinnedContainers++;
      nProcessed.push_back(m_nTRTDriftCircleCollectionsProcessed);
      nKept.push_back(m_nTRTDriftCircleCollectionsKept);
      nRemoved.push_back(m_nTRTDriftCircleCollectionsRemoved);
      nClustersProcessed.push_back(m_nTRTDriftCirclesProcessed);
      nClustersKept.push_back(m_nTRTDriftCirclesKept);
      nClustersRemoved.push_back(m_nTRTDriftCirclesRemoved);
      key.push_back(m_trtDriftCircleKey);
      object.push_back("TRTDriftCircles");
    }
  
  // Now, loop over all the thinned containers 
  // and write out the statistics to the log file
  for ( int i=0; i<numberOfThinnedContainers; ++i )
    {
      if (msgLvl(MSG::DEBUG))
        {
          msg(MSG::DEBUG) 
            << " Number of " << key[i] << " " << object[i] << " processed:                   " << nProcessed[i]
            << endmsg;
          msg(MSG::DEBUG) 
            << " Number of " << key[i] << " " << object[i] << " kept:                        " << nKept[i]
            << endmsg;
          msg(MSG::DEBUG) 
            << " Number of " << key[i] << " " << object[i] << " removed:                     " << nRemoved[i]
            << endmsg;
        }

      if ( m_nEventsProcessed != 0 )
        {
          if (msgLvl(MSG::DEBUG))
            {
              msg(MSG::DEBUG) 
                << " Average per event number of " << key[i] << " " << object[i] << " processed: "
                << format("%6.3f", nProcessed[i]/(double)m_nEventsProcessed)
                << endmsg;
              msg(MSG::DEBUG) 
                << " Average per event number of " << key[i] << " " << object[i] << " kept:      "
                << format("%6.3f", nKept[i]/(double)m_nEventsProcessed)
                << endmsg;
              msg(MSG::DEBUG) 
                << " Average per event number of " << key[i] << " " << object[i] << " removed:   "
                << format("%6.3f", nRemoved[i]/(double)m_nEventsProcessed)
                << endmsg;
            }
        }
       
      if ( nProcessed[i] != 0 )
        {
          if (msgLvl(MSG::DEBUG))
            {
              msg(MSG::DEBUG) 
                << " Average percent of " << key[i] << " " << object[i] << " (collections) kept:               "
                << format("%6.3f", 100.0*nKept[i]/(double)nProcessed[i])
                << endmsg;
              msg(MSG::DEBUG) 
                << " Average percent of " << key[i] << " " << object[i] << " (collections) removed:               "
                << format("%6.3f", 100.0*nRemoved[i]/(double)nProcessed[i])
                << endmsg;
            }
          if ( nClustersProcessed[i] != 0 )
            {
              if (msgLvl(MSG::DEBUG))
                {
                  msg(MSG::DEBUG) 
                    << " Average percent of " << key[i] << " " << object[i] << " clusters kept:               "
                    << format("%6.3f", 100.0*nClustersKept[i]/(double)nClustersProcessed[i])
                    << endmsg;
                  msg(MSG::DEBUG) 
                    << " Average percent of " << key[i] << " " << object[i] << " clusters removed:               "
                    << format("%6.3f", 100.0*nClustersRemoved[i]/(double)nClustersProcessed[i])
                    << endmsg;
                }
            }
        }
    }

  return sc;
}











//=============================================================================
// find the good cells
//=============================================================================
StatusCode ThinContainers::findGoodCells(const CaloCellContainer* cellCont,
                                         const I4Momentum& candHepLorentz, 
                                         double maxDeltaR, bool useDeltaEtaPhi,
                                         double maxDeltaEta, double maxDeltaPhi) 
{
  ATH_MSG_DEBUG( "==> findGoodCells " << name() << "..." );

  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // Calculate the squares of the cut values
  const double maxDeltaEtaSq = maxDeltaEta*maxDeltaEta;
  const double maxDeltaPhiSq = maxDeltaPhi*maxDeltaPhi;

  CaloCellContainer::const_iterator cellItr    = cellCont->begin();
  CaloCellContainer::const_iterator cellItrEnd = cellCont->end();
  for (; cellItr != cellItrEnd; ++cellItr)
    {
      if ( useDeltaEtaPhi )
        {
          if ( P4Helpers::deltaEtaSq(**cellItr, candHepLorentz) < maxDeltaEtaSq
               && P4Helpers::deltaPhiSq(**cellItr, candHepLorentz) < maxDeltaPhiSq )
            {
              m_goodCellIDs.insert( (*cellItr)->ID() );
              ATH_MSG_DEBUG( "Near an electron, found and add a CaloCell:"
                             << " ID="      << (*cellItr)->ID()
                             << " energy="  << (*cellItr)->energy()
                             << " time="    << (*cellItr)->time()
                             << " quality=" << (*cellItr)->quality()
                             << " eta="     << (*cellItr)->eta()
                             << " phi="     << (*cellItr)->phi() );
            } // End check deltaEtaPhi
	  
        }
      else
        {
          if ( P4Helpers::isInDeltaR( **cellItr, candHepLorentz, maxDeltaR ) )
            {
              m_goodCellIDs.insert( (*cellItr)->ID() );
              ATH_MSG_DEBUG( "Near an electron, found and add a CaloCell:"
                             << " ID="      << (*cellItr)->ID()
                             << " energy="  << (*cellItr)->energy()
                             << " time="    << (*cellItr)->time()
                             << " quality=" << (*cellItr)->quality()
                             << " eta="     << (*cellItr)->eta()
                             << " phi="     << (*cellItr)->phi() );
            } // End check deltaR
        }
    } // End loop over cells
  
  return sc;
}



//=============================================================================
// find the good Inner Detector objects
//=============================================================================
StatusCode ThinContainers::findGoodIDObjects(DETID detectorID,
                                             HepLorentzVector candHepLorentz,
                                             Hep3Vector primaryVertex,
                                             double maxDeltaEta, double maxDeltaPhi, double maxDeltaZ)
{
	
  std::vector<IdentifierHash>* listOfHashIDs;

  switch(detectorID)
    {
    case PIXEL:
      listOfHashIDs = &m_goodPixelIDs;
      break;
    case SCT:
      listOfHashIDs = &m_goodSCTClusterIDs;
      break;
    case TRT:
      listOfHashIDs = &m_goodTRTDriftCircleIDs; /// what is this?? The RegionSelector does not return a list of "DriftCircleIDs"
      break;
    default: // everything else
      ATH_MSG_ERROR( "Invalid Detector ID." );
      return StatusCode::FAILURE;
      break;
    }

  ATH_MSG_DEBUG( "==> findGoodIDObjects " << name() << "..." );
	

  double roiZMin =  primaryVertex.z() - maxDeltaZ/2.;
  double roiZMax =  primaryVertex.z() + maxDeltaZ/2.;

  //// AAAARGH DON'T USE eta IN THIS WAY !!! 
  double roiEtaMin = candHepLorentz.eta() - maxDeltaEta/2.;
  double roiEtaMax = candHepLorentz.eta() + maxDeltaEta/2.;

  double roiPhiMin = candHepLorentz.phi() - maxDeltaPhi/2.;
  double roiPhiMax = candHepLorentz.phi() + maxDeltaPhi/2.;

  double roiPhi = candHepLorentz.phi();

  while ( roiPhi<-M_PI ) roiPhi += 2*M_PI;
  while ( roiPhi> M_PI ) roiPhi -= 2*M_PI;

  while ( roiPhiMin<-M_PI ) roiPhiMin += 2*M_PI;
  while ( roiPhiMin> M_PI ) roiPhiMin -= 2*M_PI;

  while ( roiPhiMax<-M_PI ) roiPhiMax += 2*M_PI;
  while ( roiPhiMax> M_PI ) roiPhiMax -= 2*M_PI;


  RoiDescriptor roi( candHepLorentz.eta(), roiEtaMin, roiEtaMax, 
		      roiPhi, roiPhiMin, roiPhiMax, 
		      primaryVertex.z(), roiZMin, roiZMax );

  m_regionSelector->DetHashIDList( detectorID, roi, *listOfHashIDs);

  return StatusCode::SUCCESS;
}


//=============================================================================
// find the good TrackParticles
//=============================================================================
StatusCode ThinContainers::findGoodTrackParticles(const Rec::TrackParticleContainer* trackParticleCont,
                                                  const I4Momentum& candHepLorentz, 
                                                  double maxDeltaR, bool useDeltaEtaPhi,
                                                  double maxDeltaEta, double maxDeltaPhi) 
{
  ATH_MSG_DEBUG( "==> findGoodTrackParticles " << name() << "..." );

  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // Calculate the squares of the cut values
  const double maxDeltaEtaSq     = maxDeltaEta*maxDeltaEta;
  const double maxDeltaPhiSq     = maxDeltaPhi*maxDeltaPhi;
  const double maxDeltaEtaWideSq = m_wideDeltaEtaForBadTracks*m_wideDeltaEtaForBadTracks;

  Rec::TrackParticleContainer::const_iterator trackParticleItr    = trackParticleCont->begin();
  Rec::TrackParticleContainer::const_iterator trackParticleItrEnd = trackParticleCont->end();
  long int i=0; // This is the counter...
  for (; trackParticleItr != trackParticleItrEnd; ++trackParticleItr)
    {
      const Rec::TrackParticle* track = (*trackParticleItr);
   
      if (!track){ continue; }


      // Check how many hits the TrackParticle has in the silicon
      const Trk::TrackSummary* trackSum = track->trackSummary();
      long int nPixelHits = (long int) trackSum->get(Trk::numberOfPixelHits);
      long int nSctHits   = (long int) trackSum->get(Trk::numberOfSCTHits);
      long int nBLayHits  = (long int) trackSum->get(Trk::numberOfInnermostPixelLayerHits);
      if ( nPixelHits < 0 ) nPixelHits = 0; 
      if ( nSctHits   < 0 ) nSctHits   = 0; 
      if ( nBLayHits  < 0 ) nBLayHits  = 0; 
      const long int nSiliconHits = nPixelHits + nSctHits + nBLayHits;


      if ( useDeltaEtaPhi )
        {
          if ( P4Helpers::deltaEtaSq(*track, candHepLorentz) < maxDeltaEtaSq
               && P4Helpers::deltaPhiSq(*track, candHepLorentz) < maxDeltaPhiSq )
            {
              m_goodTrackParticleIDs.insert( i );
              ATH_MSG_DEBUG( "Near an electron, found and add a TrackParticle:"
                             << " eta="    << track->hlv().eta()
                             << " phi="    << track->hlv().phi() );
            } // End check deltaEtaPhi
          else if ( m_treatBadTracks && nSiliconHits < 5 
                    && P4Helpers::deltaEtaSq(*track, candHepLorentz) < maxDeltaEtaWideSq
                    && P4Helpers::deltaPhiSq(*track, candHepLorentz) < maxDeltaPhiSq )
            {
              m_goodTrackParticleIDs.insert( i );
              ATH_MSG_DEBUG( "Near an electron, found and add a (bad) TrackParticle:"
                             << " eta="    << track->hlv().eta()
                             << " phi="    << track->hlv().phi() );

            } // End check deltaEtaPhi
        }
      else
        {
          if ( P4Helpers::isInDeltaR( *track, candHepLorentz, maxDeltaR ) )
            {
              m_goodTrackParticleIDs.insert( i );
              ATH_MSG_DEBUG( "Near an electron, found and add a TrackParticle:"
                             << " eta="    << track->hlv().eta()
                             << " phi="    << track->hlv().phi() );
            } // End check deltaR
          else if ( m_treatBadTracks && nSiliconHits < 5 
                    && P4Helpers::deltaPhiSq(*track, candHepLorentz) < maxDeltaR*maxDeltaR
                    && P4Helpers::deltaEtaSq(*track, candHepLorentz) < maxDeltaEtaWideSq )
            {
              m_goodTrackParticleIDs.insert( i );
              ATH_MSG_DEBUG( "Near an electron, found and add a (bad) TrackParticle:"
                             << " eta="    << track->hlv().eta()
                             << " phi="    << track->hlv().phi() );
	      
            } // End check deltaEtaPhi for bad TrackParticles
        }

      ++i; // Increment the counter
    } // End loop over TrackParticleContainer

  return sc;
}


//=============================================================================
// find the good Tracks
//=============================================================================
StatusCode ThinContainers::findGoodTracks(const TrackCollection* trackCont,
                                          HepLorentzVector candHepLorentz, 
                                          double maxDeltaR, bool useDeltaEtaPhi,
                                          double maxDeltaEta, double maxDeltaPhi) 
{
  ATH_MSG_DEBUG( "==> findGoodTracks " << name() << "..." );

  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  TrackCollection::const_iterator trackItr    = trackCont->begin();
  TrackCollection::const_iterator trackItrEnd = trackCont->end();
  long int i=0; // This is the counter...
  for (; trackItr != trackItrEnd; ++trackItr)
    {
      const Trk::Track* track = (*trackItr);
      
      if (!track){ continue; }	
	
      // need eta and phi from Track !
      // to be updated !
      ATH_MSG_DEBUG( " Track thinning still under development, so far only rough Eta estimate used " );

      double trketa = 0.,trkphi = 0;
      // temporary: last point
      const DataVector <const Trk::TrackParameters>* paramvec = track->trackParameters();
      if (paramvec)
        { 
          DataVector <const Trk::TrackParameters>::const_iterator it = paramvec->begin();
          DataVector <const Trk::TrackParameters>::const_iterator itEnd = paramvec->end();
          for (;it!=itEnd; it++) 
            {
              trketa = (*it)->eta();
            } 
        }
      const double absDeltaEta = std::fabs(trketa - candHepLorentz.eta() );
      const double absDeltaPhi = 0.;//temporary
      const double deltaR = sqrt(absDeltaEta*absDeltaEta+absDeltaPhi*absDeltaPhi);
      ATH_MSG_DEBUG( "Thin Tracks: deltaR= " << deltaR );

      // Check how many hits the TrackParticle has in the silicon
      const Trk::TrackSummary* trackSum = track->trackSummary();
      ATH_MSG_DEBUG( "Thin Tracks: try TrackSummary" );

      long int nPixelHits =0;
      long int nSctHits   =0;
      long int nBLayHits  =0;
      if(trackSum)
        {
          ATH_MSG_DEBUG( "Thin Tracks: Track sum" );

          nPixelHits = (long int) trackSum->get(Trk::numberOfPixelHits);
          nSctHits   = (long int) trackSum->get(Trk::numberOfSCTHits);
          nBLayHits  = (long int) trackSum->get(Trk::numberOfInnermostPixelLayerHits);
        }

      if ( nPixelHits < 0 ) nPixelHits = 0; 
      if ( nSctHits   < 0 ) nSctHits   = 0; 
      if ( nBLayHits  < 0 ) nBLayHits  = 0; 
      const long int nSiliconHits = nPixelHits + nSctHits + nBLayHits;
	
	
      if ( useDeltaEtaPhi )
        {
          if ( absDeltaEta < maxDeltaEta && absDeltaPhi < maxDeltaPhi )
            {
              m_goodTrackIDs.insert( i );
              ATH_MSG_DEBUG( "found and add a TrackParticle:"
                             << " eta="    << trketa
                             << " phi="    << trkphi );
            } // End check deltaEtaPhi
          else if ( m_treatBadTracks && nSiliconHits < 5 
                    && absDeltaPhi < maxDeltaPhi 
                    && absDeltaEta < m_wideDeltaEtaForBadTracks )
            {
              m_goodTrackIDs.insert( i );
              ATH_MSG_DEBUG( "found and add a (bad) TrackParticle:"
                             << " eta="    << trketa
                             << " phi="    << trkphi );
		
            } // End check deltaEtaPhi
        }
      else
        {
          ATH_MSG_DEBUG( "Thin tracks with MaxDeltaR" );
          if ( deltaR < maxDeltaR )
            {
              m_goodTrackIDs.insert( i );
              ATH_MSG_DEBUG( "found and add a TrackParticle:"
                             << " eta="    << trketa
                             << " phi="    << trkphi );
            } // End check deltaR
          else if ( m_treatBadTracks && nSiliconHits < 5 
                    && absDeltaPhi < maxDeltaR
                    && absDeltaEta < m_wideDeltaEtaForBadTracks )
            {
              m_goodTrackIDs.insert( i );
              ATH_MSG_DEBUG( "found and add a (bad) TrackParticle:"
                             << " eta="    << trketa
                             << " phi="    << trkphi );

            } // End check deltaEtaPhi for bad TrackParticles
        }
      
      ++i; // Increment the counter
      
      
    } // End loop over Tracks
    
    
  return sc;
}





//-----------------------------------------
// Now, do the actual thinning of the CaloCells
//-----------------------------------------
StatusCode ThinContainers::thinCaloClusters(const CaloClusterContainer* clusCont)
{
  ATH_MSG_DEBUG( "==> thinCaloClusters " << name() << "..." );

  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // First, create the mask to be used for thinning
  std::vector<bool> mask(clusCont->size());

  // Then, iterate over the pixels to figure out which one should be kept
  // Create also some bookkeeping counters
  unsigned long nTotal  = 0;
  unsigned long nKeep   = 0;
  unsigned long nReject = 0;
  for ( std::size_t i=0; i<clusCont->size(); ++i )
    {
      ++nTotal;
      if ( m_goodClusterIDs.find( i ) != m_goodClusterIDs.end() )
        {
          mask[i] = true;
          ++nKeep;
        }
      else
        {
          mask[i] = false;
          ++nReject;
        }
    } // End loop over CaloClusters
  
  // Update the counters
  m_nClustersProcessed += nTotal;
  m_nClustersKept      += nKeep;
  m_nClustersRemoved   += nReject;


  // Write out a statistics message
  ATH_MSG_DEBUG( " CaloClusterThinning statistics: keeping " << nKeep << " cells"
                 << " and rejecting " << nReject << " cells" );

  // Perform the actual thinning
  ATH_CHECK( m_thinningSvc->filter(*clusCont, mask, IThinningSvc::Operator::Or) );

  return sc;
}









//-----------------------------------------
// Now, do the actual thinning of the CaloCells
//-----------------------------------------
StatusCode ThinContainers::thinCells(const CaloCellContainer* cellCont)
{
  ATH_MSG_DEBUG( "==> thinCells " << name() << "..." );

  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // First, create the mask to be used for thinning
  //IThinningSvc::Filter_t mask;
  std::vector<bool> mask(cellCont->size());

  // Then, iterate over the pixels to figure out which one should be kept
  // Create also some bookkeeping counters
  unsigned long nTotal  = 0;
  unsigned long nKeep   = 0;
  unsigned long nReject = 0;
  for ( std::size_t i=0; i<cellCont->size(); ++i )
    {
      ++nTotal;
      if ( ( ( m_useCaloCellEnergyCut) && ( cellCont->at(i)->energy() > m_minCaloCellEnergy ) ) 
           ||
           ( m_goodCellIDs.find(cellCont->at(i)->ID()) != m_goodCellIDs.end() ) )
        {
          mask[i] = true;
          ++nKeep;
        }
      else
        {
          mask[i] = false;
          ++nReject;
        }
    } // End loop over cells
  
  // Update the counters
  m_nCellsProcessed += nTotal;
  m_nCellsKept      += nKeep;
  m_nCellsRemoved   += nReject;


  // Write out a statistics message
  ATH_MSG_DEBUG( " CaloCellThinning statistics: keeping " << nKeep << " cells"
                 << " and rejecting " << nReject << " cells" );

  // Perform the actual thinning
  ATH_CHECK( m_thinningSvc->filter(*cellCont, mask, IThinningSvc::Operator::Or) );

  return sc;
}







//-----------------------------------------
// Now, do the actual thinning of the pixels
//-----------------------------------------
StatusCode ThinContainers::thinPixels(const InDet::PixelClusterContainer* pixelCont) 
{
  ATH_MSG_DEBUG( "==> thinPixels " << name() << "..." );

  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // First, create the mask to be used for thinning
  IThinningSvc::Filter_t mask;

  // Then, iterate over the pixels to figure out which one should be kept
  // Create also some bookkeeping counters
  unsigned long nTotal  = 0;
  unsigned long nKeep   = 0;
  unsigned long nReject = 0;
  unsigned long nClustersTotal  = 0;
  unsigned long nClustersKeep   = 0;
  unsigned long nClustersReject = 0;

  // Loop over the PixelClusterContainer
  InDet::PixelClusterContainer::const_iterator pixelItr    = pixelCont->begin();
  InDet::PixelClusterContainer::const_iterator pixelItrEnd = pixelCont->end();
  for (; pixelItr != pixelItrEnd; ++pixelItr)
    {
      ++nTotal;
      ATH_MSG_DEBUG( "There are " << (*pixelItr)->size() 
                     << " entries in the PixelClusterContainer!" );
      const PixelClusterCollection *colNext = (*pixelItr);
      
      if (!colNext){ continue; }

      // Each PixelClusterContainer contains several hundred collections of the type
      // DataVector<PixelCluster>
      // And each of these DataVector<PixelCluster> contains only a few (1-5 or so)
      // PixelClusters. Going to keep the whole DataVector<PixelCluster> if one of
      // its PixelClusters is near the interesting objects!
      
      bool keepThisCollection = false;
      unsigned long nClustersInThisContainer = 0;

      nClustersTotal += colNext->size();
      nClustersInThisContainer += colNext->size();

      // Determine if this ClusterCollection is interesting and should be kept
      if ( std::find(m_goodPixelIDs.begin(), m_goodPixelIDs.end(), colNext->identifyHash()) != m_goodPixelIDs.end() )
	      {
		      keepThisCollection = true;
		      ++nKeep;
		      nClustersKeep+= nClustersInThisContainer;
	      }
      else
	      {
		      ++nReject;
		      nClustersReject+= nClustersInThisContainer;
	      }
      
      // Fill the thinning mask with the results      
      mask[colNext->identifyHash()] = keepThisCollection;
    } // End loop over PixelClusterContainer

  // Update the counters
  m_nPixelCollectionsProcessed += nTotal;
  m_nPixelCollectionsKept      += nKeep;
  m_nPixelCollectionsRemoved   += nReject;
  m_nPixelClustersProcessed += nClustersTotal;
  m_nPixelClustersKept      += nClustersKeep;
  m_nPixelClustersRemoved   += nClustersReject;

  // Write out a statistics message
  ATH_MSG_DEBUG( " PixelClusterThinning statistics: keeping " << nKeep << " pixelCollections"
                 << " and rejecting " << nReject << " pixelCollections" );

  // Perform the actual thinning
  ATH_CHECK( m_thinningSvc->filter(*pixelCont, mask, IThinningSvc::Operator::Or) );

  return sc;
}

//-----------------------------------------
// Now, do the actual thinning of the SCT_Clusters
//-----------------------------------------
StatusCode ThinContainers::thinSCTClusters(const InDet::SCT_ClusterContainer* sctCont) 
{
  ATH_MSG_DEBUG( "==> thinSCTClusters " << name() << "..." );

  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // First, create the mask to be used for thinning
  IThinningSvc::Filter_t mask;
  
  // Then, iterate over the SCT_Clusters to figure out which one should be kept
  // Create also some bookkeeping counters
  unsigned long nTotal  = 0;
  unsigned long nKeep   = 0;
  unsigned long nReject = 0;
  unsigned long nClustersTotal  = 0;
  unsigned long nClustersKeep   = 0;
  unsigned long nClustersReject = 0;
  
  // Loop over the SCT_ClusterContainer
  InDet::SCT_ClusterContainer::const_iterator sctItr    = sctCont->begin();
  InDet::SCT_ClusterContainer::const_iterator sctItrEnd = sctCont->end();
  for (; sctItr != sctItrEnd; ++sctItr)
    {
      ++nTotal;
      ATH_MSG_DEBUG( "There are " << (*sctItr)->size() 
                     << " entries in the SCT_ClusterContainer!" );
      const SCT_ClusterCollection *colNext = (*sctItr);
      
      if (!colNext){ continue; }
      
      // Each SCT_ClusterContainer contains several hundred collections of the type
      // DataVector<SCT_Cluster>
      // And each of these DataVector<SCT_Cluster> contains only a few (1-5 or so)
      // SCT_Clusters. Going to keep the whole DataVector<SCT_Cluster> if one of
      // its SCT_Clusters is near the interesting objects!
      
      bool keepThisCollection = false;
      unsigned long nClustersInThisContainer = 0;
      
      nClustersTotal += colNext->size();
      nClustersInThisContainer += colNext->size();

      // Determine if this ClusterCollection is interesting and should be kept
      if (std::find(m_goodSCTClusterIDs.begin(), m_goodSCTClusterIDs.end(), colNext->identifyHash()) != m_goodSCTClusterIDs.end())
        {
          keepThisCollection = true;
          ++nKeep;
          nClustersKeep+= nClustersInThisContainer;
        }
      else
        {
          ++nReject;
          nClustersReject+= nClustersInThisContainer;
        }
      
      // Fill the thinning mask with the results
      mask[colNext->identifyHash()] = keepThisCollection;
    } // End loop over SCT_ClusterContainer
  
  // Update the counters
  m_nSCTClusterCollectionsProcessed += nTotal;
  m_nSCTClusterCollectionsKept      += nKeep;
  m_nSCTClusterCollectionsRemoved   += nReject;
  m_nSCTClustersProcessed += nClustersTotal;
  m_nSCTClustersKept      += nClustersKeep;
  m_nSCTClustersRemoved   += nClustersReject;

  // Write out a statistics message
  ATH_MSG_DEBUG( " SCT_ClusterThinning statistics: keeping " << nKeep << " SCT_ClusterCollections"
                 << " and rejecting " << nReject << " SCT_ClusterCollections" );

  // Perform the actual thinning
  ATH_CHECK( m_thinningSvc->filter(*sctCont, mask, IThinningSvc::Operator::Or) );

  return sc;
}






//-----------------------------------------
// Now, do the actual thinning of the TRT_DriftCircles
//-----------------------------------------
StatusCode ThinContainers::thinTRTDriftCircles(const InDet::TRT_DriftCircleContainer* trtCont) 
{
  ATH_MSG_DEBUG( "==> thinTRTDriftCircles " << name() << "..." );

  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // First, create the mask to be used for thinning
  IThinningSvc::Filter_t mask;
  
  // Then, iterate over the SCT_Clusters to figure out which one should be kept
  // Create also some bookkeeping counters
  unsigned long nTotal  = 0;
  unsigned long nKeep   = 0;
  unsigned long nReject = 0;
  unsigned long nClustersTotal  = 0;
  unsigned long nClustersKeep   = 0;
  unsigned long nClustersReject = 0;

  // Loop over the SCT_ClusterContainer
  InDet::TRT_DriftCircleContainer::const_iterator trtItr    = trtCont->begin();
  InDet::TRT_DriftCircleContainer::const_iterator trtItrEnd = trtCont->end();
  for (; trtItr != trtItrEnd; ++trtItr)
    {
      ++nTotal;
      ATH_MSG_DEBUG( "There are " << (*trtItr)->size() 
                     << " entries in the TRT_DriftCircleContainer!" );
      const TRT_DriftCircleCollection *colNext = (*trtItr);
      
      if (!colNext){ continue; }
      
      // Each TRT_DriftCircleContainer contains several hundred collections of the type
      // DataVector<TRT_DriftCircle>
      // And each of these DataVector<TRT_DriftCircle> contains only a few (1-5 or so)
      // TRT_DriftCircles. Going to keep the whole DataVector<TRT_DriftCircle> if one of
      // its TRT_DriftCircle is near the interesting objects!
      
      bool keepThisCollection = false;
      unsigned long nClustersInThisContainer = 0;

      nClustersTotal += colNext->size();
      nClustersInThisContainer += colNext->size();

      // Determine if this ClusterCollection is interesting and should be kept
      if (std::find(m_goodTRTDriftCircleIDs.begin(), m_goodTRTDriftCircleIDs.end(), colNext->identifyHash()) != m_goodTRTDriftCircleIDs.end())
        {
          keepThisCollection = true;
          ++nKeep;
          nClustersKeep+= nClustersInThisContainer;
        }
      else
        {
          keepThisCollection = false;
          ++nReject;
          nClustersReject+= nClustersInThisContainer;
        }
      // Fill the thinning mask with the results
      mask[colNext->identifyHash()] = keepThisCollection;
    } // End loop over TRT_DriftCircles

  // Update the counters
  m_nTRTDriftCircleCollectionsProcessed += nTotal;
  m_nTRTDriftCircleCollectionsKept      += nKeep;
  m_nTRTDriftCircleCollectionsRemoved   += nReject;
  m_nTRTDriftCirclesProcessed += nClustersTotal;
  m_nTRTDriftCirclesKept      += nClustersKeep;
  m_nTRTDriftCirclesRemoved   += nClustersReject;

  // Write out a statistics message
  ATH_MSG_DEBUG( " TRT_DriftCircleThinning statistics: keeping " << nKeep << " TRT_DriftCircleCollections"
                 << " and rejecting " << nReject << " TRT_DriftCircleCollections" );

  // Perform the actual thinning
  ATH_CHECK( m_thinningSvc->filter(*trtCont, mask, IThinningSvc::Operator::Or) );
  
  return sc;
}





//-----------------------------------------
// Now, do the actual thinning of the CaloCells
//-----------------------------------------
StatusCode ThinContainers::thinTrackParticles(const Rec::TrackParticleContainer* trackParticleCont)
{
  ATH_MSG_DEBUG( "==> thinTrackParticles " << name() << "..." );

  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // First, create the mask to be used for thinning
  //IThinningSvc::Filter_t mask;
  std::vector<bool> mask(trackParticleCont->size());
  
  // Then, iterate over the pixels to figure out which one should be kept
  // Create also some bookkeeping counters
  unsigned long nTotal  = 0;
  unsigned long nKeep   = 0;
  unsigned long nReject = 0;
  for ( std::size_t i=0; i<trackParticleCont->size(); ++i )
    {
      ++nTotal;
      if ( m_goodTrackParticleIDs.find( i ) != m_goodTrackParticleIDs.end() )
        {
          mask[i] = true;
          ++nKeep;
        }
      else
        {
          mask[i] = false;
          ++nReject;
        }
    } // End loop over TrackParticles
  
  // Update the counters
  m_nTrackParticlesProcessed += nTotal;
  m_nTrackParticlesKept      += nKeep;
  m_nTrackParticlesRemoved   += nReject;

  // Write out a statistics message
  ATH_MSG_DEBUG( " TrackParticleThinning statistics: keeping " << nKeep << " TrackParticles"
                 << " and rejecting " << nReject << " TrackParticles" );

  // Perform the actual thinning
  ATH_CHECK( m_thinningSvc->filter(*trackParticleCont, mask, IThinningSvc::Operator::Or) );

  return sc;
}





//-----------------------------------------
// Now, do the actual thinning of the Tracks
//-----------------------------------------
StatusCode ThinContainers::thinTracks(const TrackCollection* trackCont)
{
  ATH_MSG_DEBUG( "==> thinTracks " << name() << "..." );

  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // First, create the mask to be used for thinning
  //IThinningSvc::Filter_t mask;
  std::vector<bool> mask(trackCont->size());
  
  // Then, iterate over the tracks to figure out which one should be kept
  // Create also some bookkeeping counters
  unsigned long nTotal  = 0;
  unsigned long nKeep   = 0;
  unsigned long nReject = 0;
  for ( std::size_t i=0; i<trackCont->size(); ++i )
    {
      ++nTotal;
      if ( m_goodTrackIDs.find( i ) != m_goodTrackIDs.end() )
        {
          mask[i] = true;
          ++nKeep;
        }
      else
        {
          mask[i] = false;
          ++nReject;
        }
    } // End loop over TrackParticles
  
  // Update the counters
  m_nTracksProcessed += nTotal;
  m_nTracksKept      += nKeep;
  m_nTracksRemoved   += nReject;

  // Write out a statistics message
  ATH_MSG_DEBUG( " TrackThinning statistics: keeping " << nKeep << " Tracks"
                 << " and rejecting " << nReject << " Tracks" );

  // Perform the actual thinning
  ATH_CHECK( m_thinningSvc->filter(*trackCont, mask, IThinningSvc::Operator::Or) );

  return sc;
}



