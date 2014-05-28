/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    Authors: E. Reinherz-Aronis, A. Kreisel 
             based on MuGirl by Shlomit Tarem
***************************************************************************/
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/Navigation.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#ifdef HAVE_NEW_IOSTREAMS
#include <iostream>
#include<iomanip>
#else
#include <iostream.h>
#include<iomanip>
#endif

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "GaudiKernel/NTuple.h"

#include "TrigMuGirl/TrigMuGirlAlg.h"

#include "TrigMuonToolInterfaces/ITrigMuGirlTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"



TrigMuGirlAlg::TrigMuGirlAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::FexAlgo(name, pSvcLocator),
  m_trigMuGirlTool("TrigMuGirlTool/TrigMuGirlTool"),  
  m_particleCreatorTool("TrackToTrackParticleConvTool"),
  m_idTrackParticlesName("InDetTrigParticleCreation_Muon_EFID"),
  m_doMuonFeature(false),
  m_eMuonEFInfoCont(0),
  m_eTrkParCont(0)
{
  
  // configure property values
  declareProperty("TrigMuGirlTool",          m_trigMuGirlTool);  
  declareProperty("doMuonFeature",           m_doMuonFeature);
  declareProperty("IdTrackParticles",        m_idTrackParticlesName);
  declareProperty("ParticleCreatorTool",     m_particleCreatorTool);
  
}

//** ----------------------------------------------------------------------------------------------------------------- **//

TrigMuGirlAlg::~TrigMuGirlAlg() 
{
  
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigMuGirlAlg::hltInitialize() {
  
  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Initializing TrigMuGirl " << endreq;    

  
  if (m_particleCreatorTool.retrieve().isFailure()) {
    msg() << MSG::ERROR << "Cannot retrieve the ParticleCreator Tool" << endreq;
    return StatusCode::FAILURE;
  }

  if (retrieve(m_trigMuGirlTool).isFailure()) {
    msg()<<MSG::ERROR<<"no m_trigMuGirlTool"<<endreq;    
    return StatusCode::RECOVERABLE;
  }
  
  m_trigMuGirlTool->declareMonitoringVariables(this,m_TrigMuGirlMonVars);
  if (doTiming()) m_trigMuGirlTool->addTimers(this,m_TrigMuGirlTimers);
  m_trigMuGirlTool->initNtuple(this);

  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Initializing TrigMuGirl done successfully" << endreq;    
  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigMuGirlAlg::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigMuGirl" << endreq;

  return HLT::OK;
}

//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigMuGirlAlg::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {

        
  if ( msgLvl() <= MSG::DEBUG ) 
    msg()<<MSG::DEBUG<<"Executing "<<name()<<" for types "
	 <<inputTE->getId()<<" -> "<<outputTE->getId()<<endreq;
  
  m_trigMuGirlTool->initRoiProcessing(m_TrigMuGirlTimers,m_eMuonEFInfoCont);

  HLT::ErrorCode status;

  if (m_doMuonFeature)
    {
      std::vector<const MuonFeature*> vectorOfMuonFeature;
      status = getFeatures(inputTE, vectorOfMuonFeature, "");
      if (status != HLT::OK)
        goto IDtrack;

      ElementLink<CombinedMuonFeatureContainer> CombinedMuonFeatureEL;
      getFeatureLink<CombinedMuonFeatureContainer, CombinedMuonFeature>(inputTE, CombinedMuonFeatureEL);
     
      status = m_trigMuGirlTool->processMuonFeatures(vectorOfMuonFeature, CombinedMuonFeatureEL);
      if (status != HLT::OK) {
	return status;
      }
    }   //if doMuonFeature

 IDtrack:

  // get element links to the ID tracks
  ElementLinkVector<Rec::TrackParticleContainer> elv_idtrks;
  if( HLT::OK != getFeaturesLinks<Rec::TrackParticleContainer,Rec::TrackParticleContainer>(inputTE, elv_idtrks, m_idTrackParticlesName) ) {
    msg() << MSG::WARNING << "Failed to get ElementLinkVector<xAOD::TrackParticleContainer>, aborting this ROI" << endreq;
    return HLT::MISSING_FEATURE;
  }

  std::vector<const xAOD::CaloClusterContainer*> vectorOfClusterContainers;
  if(getFeatures(inputTE, vectorOfClusterContainers,"HLT")!=HLT::OK) 
    {
      if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to get ClusterContainers" << endreq;
    }


  /** Trasform the input TrackParticle into xAOD::Trackparticles */
  ElementLinkVector<xAOD::TrackParticleContainer> idSeeds;
/*
  xAOD::TrackParticleContainer*    idTrackParticles = new xAOD::TrackParticleContainer();
  xAOD::TrackParticleAuxContainer* idTrackParticlesAuxCont = new xAOD::TrackParticleAuxContainer();
  idTrackParticles->setStore( idTrackParticlesAuxCont );
  int itrack=0;
  for (ElementLinkVector<Rec::TrackParticleContainer>::const_iterator ipIt = elv_idtrks.begin();ipIt != elv_idtrks.end();) {
      for 
      /// Use ElementLink so the xAOD::TrackParticle has a link back to the Trk::Track
      const ElementLink<TrackCollection> trackLink(*ipIt, itrack);
      ATH_MSG_VERBOSE("Create xAOD track particle for tract with pt " << (*trackLink)->perigeeParameters()->pT());

      xAOD::TrackParticle* trackparticle = m_particleCreatorTool->createParticle( trackLink, idTrackParticles, 0, xAOD::muon);
      ATH_MSG_VERBOSE("Created xAOD TrackParticle pointer " << trackparticle << ", container size = " << idTrackParticles->size());
      ATH_MSG_VERBOSE("pt, eta, phi, m = " << trackparticle->pt() << ", " << trackparticle->eta() << ", " << trackparticle->phi() << ", " << trackparticle->m() << " valid trk link = " << trackparticle->track().isValid());  
      idSeeds.push_back(idTrackParticles->at(itrack));
      itrack++;
  }
*/
  /** Process the Inner detector seeds */
  status = m_trigMuGirlTool->processTrackCollections(idSeeds,vectorOfClusterContainers);
  if (status!=HLT::OK) return status;
   
  m_eMuonEFInfoCont = new TrigMuonEFInfoContainer();
  m_trigMuGirlTool->finalizeRoiProcessing(m_TrigMuGirlTimers,m_eMuonEFInfoCont,m_eTrkParCont,m_TrigMuGirlMonVars);
   
  status = attachFeature(outputTE, m_eMuonEFInfoCont,"eMuonEFInfo");
  if(status != HLT::OK) 
    {
      msg() << MSG::WARNING << "Failed to attach TrigMuonEFInfoContainer." << endreq;
      delete m_eMuonEFInfoCont;
      m_eMuonEFInfoCont = 0;
      //     return status;
    } 
  else 
    {
      msg() << MSG::DEBUG << "Successfully attached to TEout a TrigMuonEFInfoContainer with size " << m_eMuonEFInfoCont->size() << endreq;
    }


  //attached ID track container
  //   msg()<<MSG::DEBUG<<"number of eTrkParCont="<<m_eTrkParCont->size()<<endreq;
  status = attachFeature(outputTE, m_eTrkParCont, "eGirlID");
  if (status!= HLT::OK) 
    {
      msg() << MSG::WARNING << "Problems with attachFeature TrackParticles  Container"<< endreq;
    }
  else 
    {
      msg()<<MSG::DEBUG<<"attached GirlIDTrk container with size="<<m_eTrkParCont->size()<<endreq;
    }

  ///delete idTrackParticles;
  ///delete idTrackParticlesAuxCont;

  if (msg().level() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "TrigMuGirlAlg::execute done" << endreq;

  return HLT::OK;
}
