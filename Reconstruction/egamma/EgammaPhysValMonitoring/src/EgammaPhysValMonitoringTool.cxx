///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EgammaPhysValMonitoringTool.cxx 
// Implementation file for class EgammaPhysValMonitoringTool
// Author:
/////////////////////////////////////////////////////////////////// 

// PhysVal includes
#include "EgammaPhysValMonitoringTool.h"

// STL includes
#include <vector>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaDefs.h" 
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"

#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

#include <iostream>
using namespace std;
using namespace MCTruthPartClassifier;

namespace EgammaPhysValMonitoring {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

EgammaPhysValMonitoringTool::EgammaPhysValMonitoringTool( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent ):
  ManagedMonitorToolBase( type, name, parent ),
  m_oElectronValidationPlots(0, "Electron/"),
  m_oPhotonValidationPlots(0, "Photon/"),
  m_truthClassifier("MCTruthClassifier/MCTruthClassifier")
{    
  declareProperty( "ElectronContainerName", m_electronContainerName = "ElectronCollection" ); 
  declareProperty( "ElectronContainerFrwdName", m_electronContainerFrwdName = "FwdElectrons" ); 
  declareProperty( "PhotonContainerName", m_photonContainerName = "PhotonCollection" ); 
  declareProperty( "TruthParticleContainerName", m_truthParticleContainerName = "TruthParticle");
  declareProperty( "MCTruthClassifier",   m_truthClassifier);
}

// Destructor
///////////////
EgammaPhysValMonitoringTool::~EgammaPhysValMonitoringTool()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode EgammaPhysValMonitoringTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");    
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  ATH_CHECK(m_truthClassifier.retrieve());  

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------
StatusCode EgammaPhysValMonitoringTool::bookHistograms() 
//-------------------------------------------------------------------
{
  ATH_MSG_INFO ("Booking hists " << name() << "...");

  m_oElectronValidationPlots.initialize();
  std::vector<HistData> hists = m_oElectronValidationPlots.retrieveBookedHistograms();
  for (auto &hist : hists){
    ATH_MSG_INFO ("Initializing " << hist.first << " " << hist.first->GetName() << " " << hist.second << "...");
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }

  m_oPhotonValidationPlots.initialize();
  hists = m_oPhotonValidationPlots.retrieveBookedHistograms();
  for (auto &hist : hists){
    ATH_MSG_INFO ("Initializing " << hist.first << " " << hist.first->GetName() << " " << hist.second << "...");
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }



  return StatusCode::SUCCESS;      
}


//-------------------------------------------------------------------
StatusCode EgammaPhysValMonitoringTool::fillHistograms()
//-------------------------------------------------------------------
{
  ATH_MSG_INFO ("Filling hists " << name() << "...");
  
  m_truthParticles = evtStore()->retrieve< const xAOD::TruthParticleContainer >( m_truthParticleContainerName );
  if (!m_truthParticles) {
    ATH_MSG_ERROR ("Couldn't retrieve Truth container with key: " << m_truthParticleContainerName);
    return StatusCode::FAILURE;
  }         
  

  std::pair<ParticleType,ParticleOrigin>  partClass;
  MCTruthPartClassifier::ParticleType type;
  
  for(const auto truthParticle : *m_truthParticles){

    //--electrons
    if (abs(truthParticle->pdgId()) == 11 && truthParticle->status() == 1  && truthParticle->barcode() < 1000000) {
      partClass=std::make_pair(Unknown,NonDefined);
      type = Unknown;
      
      if(truthParticle->isAvailable <int>("type")) {
	type   = (MCTruthPartClassifier::ParticleType) truthParticle->auxdata< int >("type");
      } else m_oElectronValidationPlots.m_oTruthPlots.fill(*truthParticle); 
      
      if(type==IsoElectron) m_oElectronValidationPlots.m_oTruthPlots.fill(*truthParticle);
    	
  } //-- end electrons

    //--photons 	   
  if (abs(truthParticle->pdgId()) == 22 && truthParticle->status() == 1 && truthParticle->barcode() < 1000000){
    
    partClass=std::make_pair(Unknown,NonDefined);
    type = Unknown;
    
    m_oPhotonValidationPlots.m_oTruthPlots.fill(*truthParticle);
    
    if(truthParticle->isAvailable <int>("type")) {
      type   = (MCTruthPartClassifier::ParticleType) truthParticle->auxdata< int >("type");
      
    } else  m_oPhotonValidationPlots.m_oTruthIsoPlots.fill(*truthParticle);
    
    if(type!=IsoPhoton) continue;
    if(truthParticle->pt()*0.001>20. && fabs(truthParticle->eta())<2.47){
      m_oPhotonValidationPlots.m_oTruthIsoPlots.fill(*truthParticle);
      ParticleOutCome photOutCome = m_truthClassifier->getParticleOutCome();
      
      //fill only iso photon for conv and not converted
      if (photOutCome == Converted)   
	m_oPhotonValidationPlots.m_oTruthIsoConvPlots.fill(*truthParticle);
      else 
	m_oPhotonValidationPlots.m_oTruthIsoUncPlots.fill(*truthParticle);
    }//end cuts on truth
  } // -- end photons 
  
} 


	 
  //---------Electrons----------------------
  if(!fillRecoElecHistograms()) {
    ATH_MSG_ERROR ("Filling reco elecectron hists  failed " << name() << "...");
    return StatusCode::FAILURE;
  }
  //---------Frwd Electrons----------------------  
  if(!fillRecoFrwdElecHistograms()) {
    ATH_MSG_ERROR ("Filling reco frwd elecectron hists  failed " << name() << "...");
    return StatusCode::FAILURE;
  }
  //----------Photons
  if(!fillRecoPhotHistograms()) {
    ATH_MSG_ERROR ("Filling reco photon  hists  failed " << name() << "...");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------
StatusCode EgammaPhysValMonitoringTool::fillRecoElecHistograms()
//-------------------------------------------------------------------
{
  ATH_MSG_DEBUG ("Filling reco electron hists " << name() << "...");
  
  const xAOD::ElectronContainer* Electrons = evtStore()->retrieve< const xAOD::ElectronContainer >( m_electronContainerName );
  if (!Electrons) {
    ATH_MSG_ERROR ("Couldn't retrieve Electron container with key: " << m_electronContainerName);
    return StatusCode::FAILURE;
  } 
  
  int numofele=0;
  bool isElecPrompt=false;
  
  for(const auto electron : *Electrons){
    
    if(electron->isGoodOQ (xAOD::EgammaParameters::BADCLUSELECTRON) != 0) continue;
    
    if(electron->isAvailable <int>("type")) {
      
      MCTruthPartClassifier::ParticleType type = (MCTruthPartClassifier::ParticleType) electron->auxdata<int>("type");
      if(type==MCTruthPartClassifier::IsoElectron) isElecPrompt=true;
      
    } else if(Match(electron,11)!=0 ) isElecPrompt=true;
    
    
    m_oElectronValidationPlots.fill(*electron,isElecPrompt);
    if(electron->author()&xAOD::EgammaParameters::AuthorElectron)   numofele++;
    
  } 
  
  m_oElectronValidationPlots.m_oCentralElecPlots.nParticles->Fill(numofele);
  
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------------
StatusCode EgammaPhysValMonitoringTool::fillRecoFrwdElecHistograms()
//-------------------------------------------------------------------
{
  ATH_MSG_DEBUG ("Filling reco frwd electron hists " << name() << "...");

  const xAOD::ElectronContainer* ElectronsFrwd = 
    evtStore()->retrieve< const xAOD::ElectronContainer >( m_electronContainerFrwdName );
  if(!ElectronsFrwd ) {
    ATH_MSG_ERROR ("Couldn't retrieve Forward Electrons container with key: " << m_electronContainerFrwdName);
    return StatusCode::FAILURE;
  }
  
  int numoffrwdele=0;
  bool isElecPrompt=false;
  for(const auto frwdelectron : *ElectronsFrwd){
    if(frwdelectron->isGoodOQ (xAOD::EgammaParameters::BADCLUSELECTRON) != 0) continue;
    
    if(Match(frwdelectron,11)!=0 ) isElecPrompt=true;
    
    m_oElectronValidationPlots.fill(*frwdelectron,isElecPrompt);
    numoffrwdele++;
    isElecPrompt=false;
  }
  
  m_oElectronValidationPlots.m_oFrwdElecPlots.nParticles->Fill(numoffrwdele);
  
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------------
StatusCode EgammaPhysValMonitoringTool::fillRecoPhotHistograms()
//-------------------------------------------------------------------
{
  ATH_MSG_DEBUG ("Filling reco photon  hists " << name() << "...");
 
  const xAOD::PhotonContainer* Photons = evtStore()->retrieve< const xAOD::PhotonContainer >( m_photonContainerName );
  if (!Photons) {
    ATH_MSG_ERROR ("Couldn't retrieve Photons container with key: " << m_photonContainerName);
    return StatusCode::FAILURE;
  } 
  
  int numofPhot=0;
  int numofTopo=0;
  int numofAmb=0; 
  int numPhotAll=0; 
  int numofCnv=0;

  for(auto photon : *Photons){
    bool isPhotPrompt=false;
    if(photon->isGoodOQ (xAOD::EgammaParameters::BADCLUSPHOTON) != 0) continue;
    
    if(Match(photon,22)!=0 ) isPhotPrompt=true;    

    m_oPhotonValidationPlots.fill(*photon, isPhotPrompt);

    if(photon->author()&xAOD::EgammaParameters::AuthorPhoton)           numofPhot++;
    else if(photon->author()&xAOD::EgammaParameters::AuthorCaloTopo35)  numofTopo++;  
    else if(photon->author()&xAOD::EgammaParameters::AuthorAmbiguous)   numofAmb++; 
    if(xAOD::EgammaHelpers::isConvertedPhoton(photon))                  numofCnv++;
  }
  numPhotAll = numofPhot+numofTopo+numofAmb;
  m_oPhotonValidationPlots.m_oAllPlots.nParticles->Fill(numPhotAll);
  m_oPhotonValidationPlots.m_oPhotPlots.nParticles->Fill(numofPhot);
  m_oPhotonValidationPlots.m_oTopoPhotPlots.nParticles->Fill(numofTopo);    
  m_oPhotonValidationPlots.m_oAmbPhotPlots.nParticles->Fill(numofAmb);
  m_oPhotonValidationPlots.m_oConvPhotPlots.nParticles->Fill(numofCnv);
 

  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------------
StatusCode EgammaPhysValMonitoringTool::procHistograms() 
//-------------------------------------------------------------------
  {
    ATH_MSG_INFO ("Finalising hists " << name() << "...");
    m_oElectronValidationPlots.finalize();
    m_oPhotonValidationPlots.finalize();
    return StatusCode::SUCCESS;
  }

//--------------------------------------------------------------------------------------------
const xAOD::TruthParticle* EgammaPhysValMonitoringTool::Match(const xAOD::Egamma* particle,
                           int pdg){
//-------------------------------------------------------------------------------------------
  float currentdr = 0.05;
  const xAOD::TruthParticle* matchedTruthParticle = nullptr;
  for (auto truthParticle: *m_truthParticles){
    if (abs(truthParticle->pdgId()) != pdg || truthParticle->status() != 1) continue;
    float dr = particle->p4().DeltaR(truthParticle->p4());
    if (dr < currentdr){
      currentdr = dr;
      matchedTruthParticle = truthParticle;
    }
  }
  return matchedTruthParticle;
}


}

