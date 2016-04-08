/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************************
Name    : ObjectThinner.cxx
Author  : Ketevi A. Assamagan
Created : September 2009

DESCRIPTION:

This class thins photon, electron, muon and taujet associated TrackParticle
and CaloCluster containers for DPD making 

*****************************************************************************/

// Include files

#include "AthenaKernel/IThinningSvc.h"

#include "DPDUtils/ObjectThinner.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "muonEvent/MuonContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/PhotonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "JetEvent/Jet.h"

//#include "AnalysisUtils/AnalysisMisc.h"

#include <algorithm>
#include <math.h>

using namespace Analysis;
using namespace Trk;
using namespace Rec;

/////////////////////////////////////////////////////////////////
// Constructor

ObjectThinner::ObjectThinner(const std::string& name, 
			           ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),  
  m_thinningSvc( "ThinningSvc",  name ),
  m_analysisSelectionTool( "UserAnalysisSelectionTool", this )
{

  declareProperty("ThinService", m_thinningSvc);
  declareProperty("AnalysisSelectionTool", m_analysisSelectionTool);
  declareProperty("InDetTrackParticles", m_inDetTrackParticleContainerName="TrackParticleCandidate");
  declareProperty("ObjectContainers", m_containerNames);
  declareProperty("DoObjectThinning", m_doThinning);
  declareProperty("CaloClusters", m_clusterContainerNames);
  declareProperty("MuonSPTrackParticles", m_muontpContainerNames);
  declareProperty("NumberOfPreSelectedMuons", m_numberOfPreSelectedMuons=1); 
  declareProperty("NumberOfPreSelectedElectrons", m_numberOfPreSelectedElectrons=1);
  declareProperty("DoSkimming", m_doSkimming=true);

}

/////////////////////////////////////////////////////////////////
// Destructor

ObjectThinner::~ObjectThinner() {}

/////////////////////////////////////////////////////////////////
// Initialize

StatusCode ObjectThinner::initialize() {

  /** Message log */
  MsgStream log( messageService(), name() );
  log << MSG::INFO 
       << "Initializing ObjectThinner" 
       << endreq;

  // Get pointer to IThinningSvc and cache it :
  if ( !m_thinningSvc.retrieve().isSuccess() ) {
     log << MSG::ERROR         
         << "Unable to retrieve pointer to IThinningSvc"
         << endreq;
     return StatusCode::FAILURE;
  }
   
  if ( m_analysisSelectionTool.retrieve().isFailure() ) {
      log << MSG::ERROR << "Can't get handle on analysis selection tool" << endreq;
     return StatusCode::FAILURE;
  }

  log << MSG::INFO << "Done Initializing ..." << endreq;

  return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////
// Finalize

StatusCode ObjectThinner::finalize() {
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// Execute

StatusCode ObjectThinner::execute() {

  /** Message log */
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG 
       << "Executing ObjectThinner" 
       << endreq;

  /** now do the Inner Detector TrackParticles. This one goes through the Thinning Service */
  std::vector<bool> selectedInDetTracks;
  const TrackParticleContainer* trackTES=0;
  StatusCode sc=evtStore()->retrieve( trackTES, m_inDetTrackParticleContainerName);
  if( sc.isFailure()  ||  !trackTES ) {
     log << MSG::WARNING
         << "No InDet container found in TDS : "
         << m_inDetTrackParticleContainerName << endreq;
  }
  if ( trackTES ) selectedInDetTracks.resize( trackTES->size(), false );

  /** egamma clusters */
  std::vector<bool> selectedegClusters;
  const CaloClusterContainer* clusterTES=0;
  sc=evtStore()->retrieve( clusterTES, m_clusterContainerNames[0]);
  if( sc.isFailure()  ||  !clusterTES ) {
     log << MSG::WARNING
         << "No AOD egamma cluster container found in TDS : "
         << m_clusterContainerNames[0] << endreq;
     return StatusCode::SUCCESS;
  }
  if ( clusterTES ) selectedegClusters.resize( clusterTES->size(), false );

  /** do the photon thinning */
  if ( m_doThinning[0] ) {
    if ( this->photonContainerThinning(selectedInDetTracks, selectedegClusters).isFailure() ) {
       log << MSG::WARNING<< "Failed to thin photon container and associated TrackParticle and CaloCluster containers " << endreq;
    }
  }

  /** do the electron thinning */
  unsigned int selectedElectrons = 0;
  if ( m_doThinning[1] ) {
    if ( this->electronContainerThinning(selectedInDetTracks, selectedegClusters, selectedElectrons).isFailure() ) {
       log << MSG::WARNING<< "Failed to thin electron container and associated TrackParticle and CaloCluster containers " << endreq;
    }
  }

  /** do the muon thinning */
  unsigned int selectedMuons = 0;
  if ( m_doThinning[2] ) {
    if ( this->muonContainerThinning(selectedInDetTracks, selectedMuons).isFailure() ) {
       log << MSG::WARNING<< "Failed to thin muon container and associated TrackParticle and CaloCluster containers " << endreq;
    }
  }

  /** do the taujet thinning */
  if ( m_doThinning[3] ) {
    if ( this->taujetContainerThinning(selectedInDetTracks).isFailure() ) {
       log << MSG::WARNING<< "Failed to thin taujet container and associated TrackParticle and CaloCluster containers " << endreq;
    }
  }

  /** do the jet thinning */
  if ( m_doThinning[4] ) {
    if ( this->jetContainerThinning().isFailure() ) {
       log << MSG::WARNING<< "Failed to thin jet container " << endreq;
    }
  }

  /** skim the events based on 2 isolated leptons (electrons or muons) */

  if ( m_doSkimming ) { /** take events with at leas 2 muons or at least 2 electrons or (at least 1 muon and 1 electron) */
     if ( (selectedMuons>m_numberOfPreSelectedMuons || selectedElectrons>m_numberOfPreSelectedElectrons) ||
        (selectedMuons>=m_numberOfPreSelectedMuons && selectedElectrons>=m_numberOfPreSelectedElectrons) )
        setFilterPassed(true);
     else setFilterPassed(false);
  }

  log << MSG::DEBUG << "Nelectrons and Nmuons = " << selectedElectrons << " " << selectedMuons << endreq; 

  /** do the thinning - for the Inner Detector tracks 
      Use the Operator::Or - container may have been thinned already by EgammaRec or TauRec
      The Muon results should be merged with those, thus the "or" operator */  
  if ( trackTES ) sc = m_thinningSvc->filter( *trackTES, selectedInDetTracks, IThinningSvc::Operator::Or );
  if ( sc.isFailure() ) log << MSG::WARNING << "Failed to thin InDet Tracks associated to Muons " << endreq;
 
  /** do the thinning - for the egClusters 
      Use the Operator::Or - for electrons or photons */
  if ( clusterTES ) sc = m_thinningSvc->filter( *clusterTES, selectedegClusters, IThinningSvc::Operator::Or );
  if ( sc.isFailure() ) log << MSG::WARNING << "Failed to thin egamma clusters associated to Electrons or Photons " << endreq;
 
  log << MSG::DEBUG << "done execute() - success" << endreq;

  return StatusCode::SUCCESS;
}

/** to thin the electron container */
StatusCode ObjectThinner::electronContainerThinning ( std::vector<bool>& selectedInDetTracks,
                                                      std::vector<bool>& selectedegClusters,
                                                      unsigned int& selectedElectrons) {

  /** Message log */
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "starting ObjectThinner() - in electrons" << endreq;

  const ElectronContainer* electronTES=0;
  StatusCode sc=evtStore()->retrieve( electronTES, m_containerNames[1]);
  if( sc.isFailure()  ||  !electronTES ) {
     log << MSG::WARNING
          << "No AOD electron container found in TDS"
          << endreq;
     return StatusCode::SUCCESS;

  }

  unsigned int maxCluster = selectedegClusters.size();
  unsigned int maxElectron = electronTES->size();
  std::vector<bool> selElectrons;
  selElectrons.resize(maxElectron, false);

  /** retrieve the softe cluster container */
  const CaloClusterContainer* clusterTES=0;
  sc=evtStore()->retrieve( clusterTES, m_clusterContainerNames[2]);
  if( sc.isFailure()  ||  !clusterTES ) {
     log << MSG::WARNING
         << "No AOD softe cluster container found in TDS : "
         << m_clusterContainerNames[2] << endreq;
  }

  unsigned int max = selectedInDetTracks.size();
  unsigned int maxSofteCluster = 0;
  std::vector<bool> selectedseClusters;
  if ( clusterTES ) {
     maxSofteCluster = clusterTES->size();
     selectedseClusters.resize(maxSofteCluster, false);
  }

  /// iterators over the container
  ElectronContainer::const_iterator electronItr  = electronTES->begin();
  ElectronContainer::const_iterator electronItrE = electronTES->end();

  for (; electronItr != electronItrE; ++electronItr) {
    if ( m_analysisSelectionTool->isSelected( *electronItr ) ) {
       selElectrons[selectedElectrons] = true;

       /** do the Inner Detector Tracks associated to electorns - these are the ones to keep */
       if ( (*electronItr)->trackParticleElementLink().isValid() ) {
          unsigned int index = (*electronItr)->trackParticleElementLink().index();
          if ( index<max && max>0 ) selectedInDetTracks[index] = true;
          else log << MSG::WARNING << "Track not in Container: index = " << index
                   << " container size = " << max
                   << " container name is " << m_inDetTrackParticleContainerName << endreq;
       }

       /** do the caloclusters associated to electrons - these are the ones to keep */
       if ( (*electronItr)->clusterElementLink().isValid() ) {
          unsigned int index = (*electronItr)->clusterElementLink().index();
          std::string key = (*electronItr)->clusterElementLink().dataID(); 
          if ( key==m_clusterContainerNames[1] ) {
             if ( index<maxCluster && maxCluster>0 ) selectedegClusters[index] = true;
             else log << MSG::WARNING << "Electron Cluster not in Container: index = " << index
                      << " container size = " << maxCluster
                      << " container name is "  
                      << key << endreq;
          } else if ( key==m_clusterContainerNames[2] ) { 
             if ( index<maxSofteCluster && maxSofteCluster>0 ) selectedseClusters[index] = true;
             else log << MSG::WARNING << "Soft Electron Cluster not in Container: index = " << index
                      << " container size = " << maxSofteCluster
                      << " container name is " 
                      << key << endreq;
          }

       }

    }
    selectedElectrons++;
  }

  /** thin the softe calo cluster container */
  if ( clusterTES ) sc = m_thinningSvc->filter( *clusterTES, selectedseClusters, IThinningSvc::Operator::Or );
  if ( sc.isFailure() ) log << MSG::WARNING << "Failed to thin softe CaloClusters associated to Electrons " << endreq;

  /** do the thinning - for the Electrons themselves/ 
      Use the Operator::Or - for electrons */
  if ( electronTES ) sc = m_thinningSvc->filter( *electronTES, selElectrons, IThinningSvc::Operator::Or );
  if ( sc.isFailure() ) log << MSG::WARNING << "Failed to thin electrons " << endreq;

  log << MSG::DEBUG << "done electronContainerThinning()" << endreq;

  return StatusCode::SUCCESS;

}

/** thin the photon */
StatusCode ObjectThinner::photonContainerThinning ( std::vector<bool>& selectedInDetTracks,
                                                                      std::vector<bool>& selectedegClusters ) {

  /** Message log */
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "starting ObjectThinner() - in photons" << endreq;

  const PhotonContainer* photonTES=0;
  StatusCode sc=evtStore()->retrieve( photonTES, m_containerNames[0]);
  if( sc.isFailure()  ||  !photonTES ) {
     log << MSG::WARNING
          << "No AOD photon container found in TDS"
          << endreq;
     return StatusCode::SUCCESS;

  }

  unsigned int max = selectedInDetTracks.size();
  unsigned int maxCluster = selectedegClusters.size();
  unsigned int maxPhoton = photonTES->size();
  std::vector<bool> selPhotons;
  selPhotons.resize(maxPhoton, false);

  /// iterators over the container
  PhotonContainer::const_iterator photonItr  = photonTES->begin();
  PhotonContainer::const_iterator photonItrE = photonTES->end();

  unsigned int iPhoton=0;
  for (; photonItr != photonItrE; ++photonItr) {
    if ( m_analysisSelectionTool->isSelected( *photonItr ) ) {
       selPhotons[iPhoton] = true;

       /** do the Inner Detector Tracks associated to converted photons - these are the ones to keep */
       for (unsigned int i=0; i<2; ++i) {
          if ( (*photonItr)->trackParticleElementLink(i).isValid() ) {
             unsigned int index = (*photonItr)->trackParticleElementLink(i).index();
             if ( index<max && max>0 ) selectedInDetTracks[index] = true;
             else log << MSG::WARNING << "Track not in Container: index = " << index
                      << " container size = " << max
                     << " container name is " << m_inDetTrackParticleContainerName << endreq;
          }
       }

       /** do the caloclusters associated to photons - these are the ones to keep */
       if ( (*photonItr)->clusterElementLink().isValid() ) {
          unsigned int index = (*photonItr)->clusterElementLink().index();
          std::string key = (*photonItr)->clusterElementLink().dataID();
          if ( index<maxCluster && maxCluster>0 ) selectedegClusters[index] = true;
          else log << MSG::WARNING << "Photon Cluster not in Container: index = " << index
                   << " container size = " << maxCluster
                   << " container name is " <<  m_clusterContainerNames[0] 
                   << " it should be = " << key << endreq;
       }
    }
    iPhoton++;
  }

  /** do the thinning - for the Photons themselves/ 
      Use the Operator::Or - for photons */
  if ( photonTES ) sc = m_thinningSvc->filter( *photonTES, selPhotons, IThinningSvc::Operator::Or );
  if ( sc.isFailure() ) log << MSG::WARNING << "Failed to thin Photons " << endreq;

  log << MSG::DEBUG << "done photonContainerThinning()" << endreq;

  return StatusCode::SUCCESS;
}

/** to thin the jet container */
StatusCode ObjectThinner::jetContainerThinning () {

  /** Message log */
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "starting ObjectThinner() - jets" << endreq;

  /** retrieve the AOD jet container */
  const JetCollection* jetTES=0;
  StatusCode sc=evtStore()->retrieve( jetTES, m_containerNames[4]);
  if( sc.isFailure()  ||  !jetTES ) {
     log << MSG::WARNING
         << "No AOD jet container found in TDS : "
         << m_containerNames[4] << endreq;
     return StatusCode::SUCCESS;
  }

  unsigned int maxJet = jetTES->size();
  std::vector<bool> selJets;
  selJets.resize(maxJet, false);

  unsigned int iJet = 0;
  JetCollection::const_iterator jetItr  = jetTES->begin();
  JetCollection::const_iterator jetItrE = jetTES->end();
  for (; jetItr != jetItrE; ++jetItr) {
    if ( m_analysisSelectionTool->isSelected( *jetItr ) ) {
       selJets[iJet] = true;
    }
    iJet++;
  }

  /** do the thinning - for the Jets themselves/ 
      Use the Operator::Or - for Jet */
  if ( jetTES ) sc = m_thinningSvc->filter( *jetTES, selJets, IThinningSvc::Operator::Or );
  if ( sc.isFailure() ) log << MSG::WARNING << "Failed to thin Jets " << endreq;

  log << MSG::DEBUG << "done jetContainerThinning()" << endreq;

  return StatusCode::SUCCESS;
}


/** to thin the taujet container */
StatusCode ObjectThinner::taujetContainerThinning ( std::vector<bool>& selectedInDetTracks ) {

  /** Message log */
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "starting ObjectThinner() - taujets" << endreq;

  /** retrieve the AOD taujet container */
  const TauJetContainer* taujetTES=0;
  StatusCode sc=evtStore()->retrieve( taujetTES, m_containerNames[3]);
  if( sc.isFailure()  ||  !taujetTES ) {
     log << MSG::WARNING
         << "No AOD taujet container found in TDS : "
         << m_containerNames[3] << endreq;
     return StatusCode::SUCCESS;
  }

  /** retrieve the taujet cluster container */
  const CaloClusterContainer* clusterTES=0;
  sc=evtStore()->retrieve( clusterTES, m_clusterContainerNames[4]);
  if( sc.isFailure()  ||  !clusterTES ) {
     log << MSG::WARNING
         << "No AOD taujet cluster container found in TDS : "
         << m_clusterContainerNames[4] << endreq;
  }

  unsigned int clusMax = 0;
  std::vector<bool> selClusters;
  if ( clusterTES ) {
     clusMax = clusterTES->size();
     selClusters.resize(clusMax, false);
  }

  unsigned int max = selectedInDetTracks.size();
  
  unsigned int maxTaujet = taujetTES->size();
  std::vector<bool> selTaujets;
  selTaujets.resize(maxTaujet, false);

  unsigned int iTaujet = 0;
  TauJetContainer::const_iterator taujetItr  = taujetTES->begin();
  TauJetContainer::const_iterator taujetItrE = taujetTES->end();
  for (; taujetItr != taujetItrE; ++taujetItr) {
    if ( m_analysisSelectionTool->isSelected( *taujetItr ) ) {
       selTaujets[iTaujet] = true; 

       /** do the Inner Detector Tracks associated to taujets - these are the ones to keep */
       for (unsigned int i=0; i<(*taujetItr)->numTrack(); ++i) {
          if ( (*taujetItr)->trackLinkVector().at(i).isValid() ) {
             unsigned int index = (*taujetItr)->trackLinkVector().at(i).index();
             if ( index<max && max>0 ) selectedInDetTracks[index] = true;
             else log << MSG::WARNING << "Track not in Container: index = " << index
                      << " container size = " << max
                     << " container name is " << m_inDetTrackParticleContainerName << endreq;
          }
       }

       /** do calo cluster associated to taujet - these are the ones to keep */
       if ( (*taujetItr)->clusterLink().isValid() && clusterTES ) {
          unsigned int index = (*taujetItr)->cellClusterLink().index();
          std::string key = (*taujetItr)->cellClusterLink().dataID();
          if ( index<clusMax && clusMax>0 ) selClusters[index] = true;
          else log << MSG::WARNING << "Cluster not in Container: index = " << index
                   << " container size = " << clusMax
                   << " container name is " << m_clusterContainerNames[4] 
                   << " should be = " << key << endreq;
       }

    }
    iTaujet++;
  }

  /** thin the taujet calo cluster container */
  if ( clusterTES ) sc = m_thinningSvc->filter( *clusterTES, selClusters, IThinningSvc::Operator::Or );
  if ( sc.isFailure() ) log << MSG::WARNING << "Failed to thin CaloClusters associated to TauJets " << endreq;

  /** do the thinning - for the TauJets themselves/ 
      Use the Operator::Or - for TauJet */
  if ( taujetTES ) sc = m_thinningSvc->filter( *taujetTES, selTaujets, IThinningSvc::Operator::Or );
  if ( sc.isFailure() ) log << MSG::WARNING << "Failed to thin TauJets " << endreq;

  log << MSG::DEBUG << "done taujetContainerThinning()" << endreq;

  return StatusCode::SUCCESS;
}


/** Thin the muon TrackParticles and Clusters */
StatusCode ObjectThinner::muonContainerThinning( std::vector<bool>& selectedInDetTracks, 
                                                 unsigned int& selectedMuons ) { 

  /** Message log */
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "starting ObjectThinner() - in muons" << endreq;

  /** retrieve the AOD muon container and build the slim tracks */ 
  const MuonContainer* muonTES=0;
  StatusCode sc=evtStore()->retrieve( muonTES, m_containerNames[2]);
  if( sc.isFailure()  ||  !muonTES ) {
     log << MSG::WARNING
         << "No AOD muon container found in TDS : "
         << m_containerNames[2] << endreq; 
     return StatusCode::SUCCESS;
  }  

  /** retrieve the muon cluster container */ 
  const CaloClusterContainer* clusterTES=0;
  sc=evtStore()->retrieve( clusterTES, m_clusterContainerNames[3]);
  if( sc.isFailure()  ||  !clusterTES ) {
     log << MSG::WARNING
         << "No AOD muon cluster container found in TDS : "
         << m_clusterContainerNames[2] << endreq;
  }

  unsigned int clusMax = 0;
  std::vector<bool> selClusters;
  if ( clusterTES ) {
     clusMax = clusterTES->size();
     selClusters.resize(clusMax, false);
  }

  unsigned int max = selectedInDetTracks.size();

  std::map<unsigned int, std::vector<bool> > muonTracks;
  std::vector<const Rec::TrackParticleContainer*> tpContainers; 

  unsigned int maxMuon = muonTES->size();
  std::vector<bool> selMuons;
  selMuons.resize(maxMuon, false);

  std::vector<unsigned int> tpSize;

  /** now select the muon spectrometer trackParticles to keep */
  for ( unsigned int i=0; i<m_muontpContainerNames.size(); ++i) {
      const TrackParticleContainer* tpTES=0;
      sc=evtStore()->retrieve( tpTES, m_muontpContainerNames[i]);
      if ( sc.isFailure()  ||  !tpTES ) {
         log << MSG::WARNING
             << "No AOD muon TrackParticle container found in TDS : "
             << m_muontpContainerNames[i] << endreq;
         continue;
      }

      tpContainers.push_back(tpTES);
      tpSize.push_back( tpTES->size() ) ;
      muonTracks[i].resize( tpTES->size(), false);
  } 

  MuonContainer::const_iterator muonItr  = muonTES->begin();
  MuonContainer::const_iterator muonItrE = muonTES->end();
  for (; muonItr != muonItrE; ++muonItr) {
    if ( m_analysisSelectionTool->isSelected( *muonItr ) ) {
       selMuons[selectedMuons] = true;

       /** do the Inner Detector Tracks associated to Muons first - these are the ones to keep */
       if ( (*muonItr)->inDetTrackLink().isValid() ) {
          unsigned int index = (*muonItr)->inDetTrackLink().index();
          if ( index<max && max>0 ) selectedInDetTracks[index] = true;
          else log << MSG::WARNING << "Track not in Container: index = " << index
                   << " container size = " << max
                   << " container name is " << m_inDetTrackParticleContainerName << endreq;
       }

       /** do calo cluster associated to Muons first - these are the ones to keep */
       if ( (*muonItr)->clusterLink().isValid() && clusterTES ) {
          unsigned int index = (*muonItr)->clusterLink().index();
          std::string key = (*muonItr)->clusterLink().dataID();
          if ( index<max && max>0 ) selClusters[index] = true;
          else log << MSG::WARNING << "Cluster not in Container: index = " << index
                   << " container size = " << clusMax
                   << " container name is " << m_clusterContainerNames[3] 
                   << " it should be " << key << endreq;
       }

       /** now select the muon spectrometer trackParticles to keep */
       for ( unsigned int i=0; i<m_muontpContainerNames.size(); ++i) {

           if ( (*muonItr)->muonExtrapTrackLink().isValid() ) {
              if ( (*muonItr)->muonExtrapTrackLink().dataID() == m_muontpContainerNames[i] ) {
                 unsigned int index = (*muonItr)->muonExtrapTrackLink().index();
                 if ( index<tpSize[i] && tpSize[i]>0 ) muonTracks[i].at(index) = true;
              }
           } else if ( (*muonItr)->innerExtrapTrackLink().isValid() ) {
             if ( (*muonItr)->innerExtrapTrackLink().dataID() == m_muontpContainerNames[i] ) {
                unsigned int index = (*muonItr)->innerExtrapTrackLink().index();
                if ( index<tpSize[i] && tpSize[i]>0 ) muonTracks[i].at(index) = true;
             }
           } else if ( (*muonItr)->combinedTrackLink().isValid() ) {
             if ( (*muonItr)->combinedTrackLink().dataID() == m_muontpContainerNames[i] ) {
                 unsigned int index = (*muonItr)->combinedTrackLink().index();
                 if ( index<tpSize[i] && tpSize[i]>0 ) muonTracks[i].at(index) = true;
             }
           } else if ( (*muonItr)->muonSpectrometerTrackLink().isValid() ) {
             if ( (*muonItr)->muonSpectrometerTrackLink().dataID() == m_muontpContainerNames[i] ) {
                 unsigned int index = (*muonItr)->muonSpectrometerTrackLink().index();
                 if ( index<tpSize[i] && tpSize[i]>0 ) muonTracks[i].at(index) = true;
             }
           }
       }
    }
    selectedMuons++;
  }


  /** do the thinning of the muon associated containers */
  for ( unsigned int i=0; i<m_muontpContainerNames.size(); ++i) {
      /** do the thinning - for the Inner Detector tracks */ 
     if ( tpContainers[i] ) sc = m_thinningSvc->filter( *(tpContainers[i]), muonTracks[i], IThinningSvc::Operator::Or );
     if ( sc.isFailure() ) log << MSG::WARNING << "Failed to thin MS TrackParticles associated to Muons " << endreq;
  }

  /** thin the muon calo cluster container */
  if ( clusterTES ) sc = m_thinningSvc->filter( *clusterTES, selClusters, IThinningSvc::Operator::Or );
  if ( sc.isFailure() ) log << MSG::WARNING << "Failed to thin CaloClusters associated to Muons " << endreq;

  /** do the thinning - for the Muons themselves/ 
      Use the Operator::Or - for muons */
  if ( muonTES ) sc = m_thinningSvc->filter( *muonTES, selMuons, IThinningSvc::Operator::Or );
  if ( sc.isFailure() ) log << MSG::WARNING << "Failed to thin Muons " << endreq;

  log << MSG::DEBUG << "done muonContainerThinning()" << endreq;
  return StatusCode::SUCCESS;
}



