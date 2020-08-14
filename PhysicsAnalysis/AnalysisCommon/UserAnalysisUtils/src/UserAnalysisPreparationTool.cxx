/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : UserAnalysisPreparationTool.cxx
Package : offline/PhysicsAnalysis/AnalysisCommon/UserAnalysisUtils
Author  : Ketevi A. Assamagan
Created : November 2007
Purpose : User Analysis Preparation - see UserAnalysisPreparationTool.h for details
*****************************************************************************/

#include "GaudiKernel/GaudiException.h"
#include "Gaudi/Property.h"

// Accessing data:
#include "CLHEP/Units/PhysicalConstants.h"

// User Tools
#include "UserAnalysisUtils/UserAnalysisPreparationTool.h"
#include "AthContainers/ConstDataVector.h"

#include <sstream>
#include <iomanip>
#include <iostream>

using namespace Analysis;
using namespace Rec;
using namespace std;

//------------------------------------------------------------------------------
UserAnalysisPreparationTool::UserAnalysisPreparationTool( const std::string& type,
                                                          const std::string& name, 
                                                          const IInterface* parent )
  : AthAlgTool( type, name, parent ),
    m_userSelectionTool ( "UserAnalysisSelectionTool" ) {

  declareInterface<UserAnalysisPreparationTool>( this );

  declareProperty("UserSelectionTool",      m_userSelectionTool);
  declareProperty("InputContainerKeys",     m_inputContainerKeys);
  declareProperty("OutputContainerKeys",    m_outputContainerKeys);
  declareProperty("IsAtlfastData",          m_isAtlfast=false);

  /** initialize counters */
  m_numElectrons      = std::make_pair(0,0);
  m_numPhotons        = std::make_pair(0,0);
  m_numMuons          = std::make_pair(0,0);
  m_numTauJets        = std::make_pair(0,0);
  m_numJets           = std::make_pair(0,0);
  m_numTrackParticles = std::make_pair(0,0);
  m_numCaloClusters   = std::make_pair(0,0);
  m_first             = true;
}

//------------------------------------------------------------------------------
StatusCode UserAnalysisPreparationTool::initialize() {

  /// get a handle on the selection tools
  StatusCode sc = m_userSelectionTool.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Can't get handle on analysis selection tool");
    return sc;
  }

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode UserAnalysisPreparationTool::finalize() {

  ATH_MSG_DEBUG("in finalize()");
 
  this->summarize();


  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------
StatusCode UserAnalysisPreparationTool::execute() {
  ATH_MSG_DEBUG("in execute()");

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  if ( m_first ) {
     if ( m_outputContainerKeys.size() != m_inputContainerKeys.size() ) {
       ATH_MSG_FATAL("Input/Output container mis-match: please fix job options");
        return StatusCode::FAILURE;
     }
     if ( m_outputContainerKeys.size() == 0 ) {
      ATH_MSG_ERROR("You should input at least one container : please fix jobOptions");
      return StatusCode::FAILURE;
     }
  }

  /** now object preparation with selection */
  for ( unsigned int i=0; i<m_inputContainerKeys.size(); ++i ) {

    string::size_type loc = m_inputContainerKeys[i].find( "Electron", 0);
    if ( loc != string::npos ) { 
       if ( m_first ) m_outputElectronKey = m_outputContainerKeys[i]; 
       sc = this->electronPreparation( m_inputContainerKeys[i] );
    }

    loc = m_inputContainerKeys[i].find( "Photon", 0);
    if ( loc != string::npos ) {
       if ( m_first ) m_outputPhotonKey = m_outputContainerKeys[i];       
       sc = this->photonPreparation( m_inputContainerKeys[i] );
    }

    loc = m_inputContainerKeys[i].find( "Muon", 0);
    if ( loc != string::npos ) {
       if ( m_first ) m_outputMuonKey = m_outputContainerKeys[i];       
       sc = this->muonPreparation( m_inputContainerKeys[i] );
    }

    std::string tau = "Tau";
    if ( m_isAtlfast ) tau = "TauJet";
    loc = m_inputContainerKeys[i].find( tau, 0);
    if ( loc != string::npos ) { 
      if ( m_first ) m_outputTauJetKey = m_outputContainerKeys[i];
      sc = this->tauJetPreparation( m_inputContainerKeys[i] );
    }

    std::string jet = "Jets";
    if ( m_isAtlfast ) jet = "Jet";
    loc = m_inputContainerKeys[i].find( jet, 0);
    if ( loc != string::npos ) { 
       if ( m_first ) m_outputJetKey = m_outputContainerKeys[i];
       sc = this->jetPreparation( m_inputContainerKeys[i] );
    }

    loc = m_inputContainerKeys[i].find( "Track", 0);
    if ( loc != string::npos ) { 
       if ( m_first ) m_outputTrackParticleKey = m_outputContainerKeys[i]; 
       sc = this->trackParticlePreparation( m_inputContainerKeys[i] );
    }

    loc = m_inputContainerKeys[i].find( "Cluster", 0);
    if ( loc != string::npos ) { 
       if ( m_first ) m_outputCaloClusterKey = m_outputContainerKeys[i];
       sc = this->caloClusterPreparation( m_inputContainerKeys[i] );
    }
    if ( sc.isFailure() ) return sc;
  }

  this->print();
  m_first = false;

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------
const PhotonContainer * UserAnalysisPreparationTool::selectedPhotons() {
  ATH_MSG_DEBUG("in selectedPhotons()");
  const PhotonContainer * container = 0;
  StatusCode sc = evtStore()->retrieve(container, m_outputPhotonKey);
  if ( sc.isFailure() || container ==0 ) ATH_MSG_ERROR("Final State Photons not found");
  return container;
}

const ElectronContainer * UserAnalysisPreparationTool::selectedElectrons() {
  ATH_MSG_DEBUG("in selectedElectrons()");
  const ElectronContainer * container = 0;
  StatusCode sc = evtStore()->retrieve(container, m_outputElectronKey);
  if ( sc.isFailure() || container ==0 ) ATH_MSG_ERROR("Final State Electrons not found");
  return container;
}

const MuonContainer * UserAnalysisPreparationTool::selectedMuons() {
  ATH_MSG_DEBUG("in selectedMuons()");
  const MuonContainer * container = 0;
  StatusCode sc = evtStore()->retrieve(container, m_outputMuonKey);
  if ( sc.isFailure() || container ==0 ) ATH_MSG_DEBUG("Final State Muons not found");
  return container;
}

const TauJetContainer * UserAnalysisPreparationTool::selectedTauJets() {
  ATH_MSG_DEBUG("in selectedTauJets()");
  const TauJetContainer * container = 0;
  StatusCode sc = evtStore()->retrieve(container, m_outputTauJetKey);
  if ( sc.isFailure() || container ==0 ) ATH_MSG_ERROR("Final State TauJets not found");
  return container;
}

const JetCollection * UserAnalysisPreparationTool::selectedJets() {
  ATH_MSG_DEBUG("in selectedJets()");
  const JetCollection * container = 0;
  StatusCode sc = evtStore()->retrieve(container, m_outputJetKey);
  if ( sc.isFailure() || container ==0 ) ATH_MSG_ERROR("Final State Jets not found");
  return container;
}

const TrackParticleContainer * UserAnalysisPreparationTool::selectedTrackParticles() {
  ATH_MSG_DEBUG("in selectedTrackParticles()");
  const TrackParticleContainer * container = 0;
  StatusCode sc = evtStore()->retrieve(container, m_outputTrackParticleKey);
  if ( sc.isFailure() || container ==0 ) ATH_MSG_ERROR("Final State TrackParticles not found");
  return container;
}

const CaloClusterContainer * UserAnalysisPreparationTool::selectedCaloClusters() {
  ATH_MSG_DEBUG("in selectedCaloClusters()");
  const CaloClusterContainer * container = 0;
  StatusCode sc = evtStore()->retrieve(container, m_outputCaloClusterKey);
  if ( sc.isFailure() || container ==0 )ATH_MSG_ERROR("Final State CaloClusters not found");
  return container;
}

  /** container preparation */
StatusCode UserAnalysisPreparationTool::electronPreparation( std::string key ) {
  ATH_MSG_DEBUG("in electronPreparation() ");

  /** create an empty container of all electrons and record it */
  auto electrons = std::make_unique<ConstDataVector<ElectronContainer> >( SG::VIEW_ELEMENTS );

  const ElectronContainer * aod_electrons = 0;
  ATH_CHECK( evtStore()->retrieve( aod_electrons, key ) );
  ATH_MSG_DEBUG("AOD ElectronContainer size is " << aod_electrons->size());
  m_numElectrons.first += aod_electrons->size();

  /// iterators over the container 
  ElectronContainer::const_iterator elecItr  = aod_electrons->begin();
  ElectronContainer::const_iterator elecItrE = aod_electrons->end();

  for (; elecItr != elecItrE; ++elecItr) {
    if ( m_userSelectionTool->isSelected( *elecItr ) ) electrons->push_back( *elecItr );
  }
  m_numElectrons.second += electrons->size();

  ATH_CHECK( evtStore()->record (std::move (electrons), m_outputElectronKey) );
  
  return StatusCode::SUCCESS;
}

StatusCode UserAnalysisPreparationTool::photonPreparation( std::string key ) {
  ATH_MSG_DEBUG("in photonPreparation() ");

  /** create an empty container of all photons and record it */
  auto photons = std::make_unique<ConstDataVector<PhotonContainer> >( SG::VIEW_ELEMENTS );

  const PhotonContainer * aod_photons = 0;
  ATH_CHECK( evtStore()->retrieve( aod_photons, key ) );
  ATH_MSG_DEBUG("AOD PhotonContainer size is " << aod_photons->size());
  m_numPhotons.first += aod_photons->size();

  /// iterators over the container 
  PhotonContainer::const_iterator photItr  = aod_photons->begin();
  PhotonContainer::const_iterator photItrE = aod_photons->end();

  /** check if this electron passes pre-selection */
  for (; photItr != photItrE; ++photItr) {
    if ( m_userSelectionTool->isSelected( *photItr ) ) photons->push_back( *photItr );
  }
  m_numPhotons.second += photons->size();

  ATH_CHECK( evtStore()->record ( std::move(photons), m_outputPhotonKey ) );

  return StatusCode::SUCCESS;
}

StatusCode UserAnalysisPreparationTool::muonPreparation( std::string key ) {
  ATH_MSG_DEBUG("in muonPreparation() ");
  StatusCode sc = StatusCode::SUCCESS;

  /** create an empty container of all muons and record it */
  ConstDataVector<MuonContainer> * muons =
    new ConstDataVector<MuonContainer>( SG::VIEW_ELEMENTS );
  sc = evtStore()->record ( muons, m_outputMuonKey );
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("Not able to create a collection of muons in StoreGate: key= " << m_outputMuonKey);
     return sc;
  }

  const MuonContainer * aod_muons = 0;
  sc = evtStore()->retrieve( aod_muons, key );
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("No ESD/AOD/DPD muon container found: key = " << key);
    return sc; 
  }
  ATH_MSG_DEBUG("AOD MuonContainer size is " << aod_muons->size());
  m_numMuons.first += aod_muons->size();

  /// iterators over the container 
  MuonContainer::const_iterator muonItr  = aod_muons->begin();
  MuonContainer::const_iterator muonItrE = aod_muons->end();

  /** check if this muon passes pre-selection */
  for (; muonItr != muonItrE; ++muonItr) {
    //    if ( m_userSelectionTool->isSelected( *muonItr ) ) muons->push_back( *muonItr );
    if ( m_userSelectionTool->isSelected( *muonItr ) ) {
      muons->push_back( *muonItr );
    }
 
  }  
  m_numMuons.second += muons->size();

  sc = evtStore()->setConst( muons );
  if ( sc.isFailure()) ATH_MSG_WARNING("Not able to lock the container of muons ");

  return sc;
}

StatusCode UserAnalysisPreparationTool::tauJetPreparation( std::string key ) {
  ATH_MSG_DEBUG("in tauJetPreparation() ");
  StatusCode sc = StatusCode::SUCCESS;

  /** create an empty container of all tauJets and record it */
  ConstDataVector<TauJetContainer> * tauJets =
    new ConstDataVector<TauJetContainer>( SG::VIEW_ELEMENTS );
  sc = evtStore()->record ( tauJets, m_outputTauJetKey );
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("Not able to create a collection of tau jets in StoreGate: key= " << m_outputTauJetKey);
    return sc;
  }

  const TauJetContainer * aod_tauJets = 0;
  sc = evtStore()->retrieve( aod_tauJets, key );
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("No ESD/AOD/DPD tauJet container found: key = " << key);
    return sc;
  }
  ATH_MSG_DEBUG("AOD TauJetContainer size is " << aod_tauJets->size());
  m_numTauJets.first += aod_tauJets->size();

  /// iterators over the container 
  TauJetContainer::const_iterator tauJetItr  = aod_tauJets->begin();
  TauJetContainer::const_iterator tauJetItrE = aod_tauJets->end();

  /** check if this tauJet passes pre-selection */
  for (; tauJetItr != tauJetItrE; ++tauJetItr) {
    //    if ( m_userSelectionTool->isSelected( *tauJetItr ) ) tauJets->push_back( *tauJetItr );
    if ( m_userSelectionTool->isSelected( *tauJetItr ) ) {
      tauJets->push_back( *tauJetItr );
    }
 
  }
  m_numTauJets.second += tauJets->size();

  sc = evtStore()->setConst( tauJets );
  if ( sc.isFailure() ) ATH_MSG_WARNING("Not able to lock the container of tauJets ");

  return sc;
}

StatusCode UserAnalysisPreparationTool::jetPreparation( std::string key ) {
  ATH_MSG_DEBUG("in jetPreparation() ");
  StatusCode sc = StatusCode::SUCCESS;

  /** create an empty container of all jets and record it */
  ConstDataVector<JetCollection> * jets =
    new ConstDataVector<JetCollection>( SG::VIEW_ELEMENTS );
  sc = evtStore()->record ( jets, m_outputJetKey );
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("Not able to create a collection of jets in StoreGate: key= " << m_outputJetKey);
     return sc;
  }

  const JetCollection * aod_jets = 0;
  sc = evtStore()->retrieve( aod_jets, key );
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("No ESD/AOD/DPD jet container found: key = " << key);
    return sc;
  }
  ATH_MSG_DEBUG("AOD JetCollection size is " << aod_jets->size());
  m_numJets.first      += aod_jets->size();

  /// iterators over the container 
  JetCollection::const_iterator jetItr  = aod_jets->begin();
  JetCollection::const_iterator jetItrE = aod_jets->end();

  /** check if this jet passes pre-selection */
  for (; jetItr != jetItrE; ++jetItr) {
    //    if ( m_userSelectionTool->isSelected( *jetItr ) ) jets->push_back( *jetItr );
    if ( m_userSelectionTool->isSelected( *jetItr ) ) {
      jets->push_back( *jetItr );
    }

  }
  m_numJets.second      += jets->size();

  sc = evtStore()->setConst( jets );
  if ( sc.isFailure() ) ATH_MSG_WARNING("Not able to lock the container of jets ");

  return sc;
}

StatusCode UserAnalysisPreparationTool::trackParticlePreparation( std::string key ) {
  ATH_MSG_DEBUG("in trackParticlePreparation() ");
  StatusCode sc = StatusCode::SUCCESS;

  /** create an empty container of TrackParticles and record it */
  ConstDataVector<TrackParticleContainer> * trackParticles =
    new ConstDataVector<TrackParticleContainer>( SG::VIEW_ELEMENTS );
  sc = evtStore()->record ( trackParticles, m_outputTrackParticleKey );
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("Not able to create a collection of TrackParticles in StoreGate: key= " << m_outputTrackParticleKey);
     return sc;
  }

  const TrackParticleContainer * aod_trackParticles = 0;
  sc = evtStore()->retrieve( aod_trackParticles, key );
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("No ESD/AOD/DPD trackParticle container found: key = " << key);
    return sc;
  }
  ATH_MSG_DEBUG("AOD TrackParticleContainer size is " << aod_trackParticles->size());
  m_numTrackParticles.first += aod_trackParticles->size();

  /// iterators over the container 
  TrackParticleContainer::const_iterator trackParticleItr  = aod_trackParticles->begin();
  TrackParticleContainer::const_iterator trackParticleItrE = aod_trackParticles->end();

  /** check if this trackParticle passes pre-selection */
  for (; trackParticleItr != trackParticleItrE; ++trackParticleItr) {
    //    if ( m_userSelectionTool->isSelected( *trackParticleItr ) ) trackParticles->push_back( *trackParticleItr );
    if ( m_userSelectionTool->isSelected( *trackParticleItr ) ) {
      trackParticles->push_back( *trackParticleItr );
    }

  }
  m_numTrackParticles.second += trackParticles->size();

  sc = evtStore()->setConst( trackParticles );
  if ( sc.isFailure() ) ATH_MSG_WARNING("Not able to lock the container of trackParticles ");

  return sc;
}

StatusCode UserAnalysisPreparationTool::caloClusterPreparation( std::string key ) {
  ATH_MSG_DEBUG("in caloClusterPreparation() ");

  /** create an empty container of all particles and record it */
  CaloClusterContainer * caloClusters = new CaloClusterContainer( SG::VIEW_ELEMENTS );
  StatusCode sc = evtStore()->record ( caloClusters, m_outputCaloClusterKey );
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("Not able to create a collection of CaloClusters in StoreGate: key= " << m_outputCaloClusterKey);
     return sc;
  }

  const CaloClusterContainer * aod_caloClusters = 0;
  sc = evtStore()->retrieve( aod_caloClusters, key );
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("No ESD/AOD/DPD caloCluster container found: key = " << key);
    return sc;
  }
  ATH_MSG_DEBUG("AOD CaloClusterContainer size is " << aod_caloClusters->size());
  m_numCaloClusters.first += aod_caloClusters->size();

  /// iterators over the container 
  CaloClusterContainer::const_iterator caloClusterItr  = aod_caloClusters->begin();
  CaloClusterContainer::const_iterator caloClusterItrE = aod_caloClusters->end();

  /** check if this caloCluster passes pre-selection */
  for (; caloClusterItr != caloClusterItrE; ++caloClusterItr) {
    if ( m_userSelectionTool->isSelected( *caloClusterItr ) ) caloClusters->push_back( *caloClusterItr );
  }
  m_numCaloClusters.second += caloClusters->size();

  sc = evtStore()->setConst( caloClusters );
  if ( sc.isFailure() ) ATH_MSG_WARNING("Not able to lock the container of calo clusters ");

  return sc;
}

//-----------------------------------------------------------------------------------------------
void UserAnalysisPreparationTool::print() {
  ATH_MSG_DEBUG("in print() ");

  /** Get the container of pre-selected Electrons */
  const ElectronContainer * electrons = this->selectedElectrons();
  if(electrons) ATH_MSG_DEBUG("Number of Pre-selected Electrons is " << electrons->size());

  /** Get the container of pre-selected Photons */
  const PhotonContainer * photons = this->selectedPhotons();
  if(photons) ATH_MSG_DEBUG("Number of Pre-selected Photons is " << photons->size());

  /** Get the container of pre-selected Muons */
  const MuonContainer * muons = this->selectedMuons();
  if(muons)ATH_MSG_DEBUG("Number of Pre-selected Muons is " << muons->size());

  /** Get the container of pre-selected TauJets */
  const TauJetContainer * tauJets = this->selectedTauJets();
  if(tauJets) ATH_MSG_DEBUG("Number of Pre-selected TauJets is " << tauJets->size());

  /** Get the container of pre-selected Jets */
  const JetCollection * jets = this->selectedJets();
  if(jets) ATH_MSG_DEBUG("Number of Pre-selected Jets is " << jets->size());

  /** Get the container of pre-selected TrackParticles */
  const TrackParticleContainer * trackParticles = this->selectedTrackParticles();
  if(trackParticles) ATH_MSG_DEBUG("Number of Pre-selected TrackParticles is " << trackParticles->size());

  /** Get the container of pre-selected CaloClusters */
  const CaloClusterContainer * caloClusters = this->selectedCaloClusters();
  if(caloClusters) ATH_MSG_DEBUG("Number of Pre-selected CaloClusters is " << caloClusters->size());

}

//---------------------------------------------------------------------------------------------------------
void UserAnalysisPreparationTool::summarize() {
  ATH_MSG_INFO("in summarize() ");

  ATH_MSG_INFO("Summary of Reconstructed Events/pre-selected events ############");
  ATH_MSG_INFO("---------------------------------------------------------------");
  ATH_MSG_INFO("Reconstructed Electrons        = " << std::setw(10) << m_numElectrons.first 
                      << "   Pre-selected Electrons      = " << std::setw(10) << m_numElectrons.second);
  ATH_MSG_INFO("Reconstructed Photons          = " << std::setw(10) << m_numPhotons.first 
                      << "   Pre-selected Photons        = " << std::setw(10) << m_numPhotons.second);
  ATH_MSG_INFO("Reconstructed Muons            = " << std::setw(10) << m_numMuons.first 
                      << "   Pre-selected Muons          = " << std::setw(10) << m_numMuons.second);
  ATH_MSG_INFO("Reconstructed TauJets          = " << std::setw(10) << m_numTauJets.first  
                      << "   Pre-selected TauJets        = " << std::setw(10) << m_numTauJets.second);
  ATH_MSG_INFO("Reconstructed Jets             = " << std::setw(10) << m_numJets.first 
                      << "   Pre-selected Jets           = " << std::setw(10) << m_numJets.second);
  ATH_MSG_INFO("Reconstructed TrackParticles   = " << std::setw(10) << m_numTrackParticles.first
                      << "   Pre-selected TrackParticles = " << std::setw(10) << m_numTrackParticles.second);
  ATH_MSG_INFO("Reconstructed CaloClusters     = " << std::setw(10) << m_numCaloClusters.first
                      << "   Pre-selected CaloClusters   = " << std::setw(10) << m_numCaloClusters.second);
}


