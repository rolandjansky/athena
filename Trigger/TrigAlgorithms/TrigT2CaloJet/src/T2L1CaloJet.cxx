/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2L1CaloJet.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet
//
// AUTHOR:   Denis Oliveira Damazio
// ********************************************************************


#include "GaudiKernel/IToolSvc.h"


#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "TrigT2CaloJet/T2L1CaloJet.h"
#include "TrigT1CaloByteStream/ITrigT1CaloDataAccess.h"
#include "TrigT1CaloToolInterfaces/IL1JetTools.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include <sys/time.h>

#include <math.h>

class ISvcLocator;

T2L1CaloJet::T2L1CaloJet(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator),
    m_jet(0),
    m_log(NULL),
    m_dataL1("LVL1BS__TrigT1CaloDataAccess/TrigT1CaloDataAccess",0),
    m_jetL1Tools("LVL1::L1JetTools/L1JetTools",this),m_storedJEs(0),
    m_retrievedJetTool(false)
{
  declareProperty("TrigT1CaloDataAccess", m_dataL1, "L1 Calo Data Access");
  declareProperty("L1JetTool", m_jetL1Tools, "L1 Calo Jet Algorithm");
  declareProperty("jetOutputKey",m_jetOutputKey = "TrigT2L1CaloJet");
  

  declareMonitoredVariable("E",   m_e);
  declareMonitoredVariable("Et",  m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
}

HLT::ErrorCode T2L1CaloJet::hltInitialize()
{
  m_log = new MsgStream(msgSvc(), name());
  
  if((*m_log).level() <= MSG::DEBUG){
     (*m_log) << MSG::DEBUG << " Intializing: " << name() <<endmsg;
  }

  if (timerSvc()) {
    m_timer.push_back(addTimer("TimeTot"));    
    m_timer.push_back(addTimer("JEUnpacking"));
    m_timer.push_back(addTimer("JEformsums"));    
    m_timer.push_back(addTimer("RoIMaking"));
  }

  // Create helper tools
  if ( m_dataL1.retrieve().isFailure() ) {
    (*m_log) << MSG::ERROR << "Failed to retreive helper tools: " << m_dataL1 << endmsg;
    m_retrievedJetTool = false;
  } else {
    m_retrievedJetTool = true;
    (*m_log) << MSG::INFO << "Retrieved " << m_dataL1 << endmsg;
  }

  if ( m_jetL1Tools.retrieve().isFailure() ) {
    (*m_log) << MSG::ERROR << "Failed to retreive helper tools: " << m_jetL1Tools << endmsg;
  } else {
    (*m_log) << MSG::INFO << "Retrieved " << m_jetL1Tools << endmsg;
  }

  m_storedJEs = new ConstDataVector<DataVector<LVL1::JetElement> >(SG::VIEW_ELEMENTS);
  

  return HLT::OK;
}

T2L1CaloJet::~T2L1CaloJet(){
}

HLT::ErrorCode T2L1CaloJet::hltExecute(const HLT::TriggerElement* inputTE,
				     HLT::TriggerElement* outputTE)
{

  if(timerSvc()){
      m_timer[0]->start();      
  }

  m_e = -99;
  m_et = -99;
  //m_eem0 = -99;
  //m_ehad0 = -99;
  m_eta = -99;
  m_phi = -99;
  
  
  
#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
     (*m_log) << MSG::DEBUG << " Executing T2L1CaloJet FEX " << name() << endmsg;
     (*m_log) << MSG::DEBUG << "m_jetOutputKey: " << m_jetOutputKey << endmsg;
     (*m_log) << MSG::DEBUG
              << "outputTE->getId(): "
              << outputTE->getId() << endmsg;
     (*m_log) << MSG::DEBUG
              << "inputTE->getId(): "
              << inputTE->getId() << endmsg;
  }
#endif

  // Note: new TriggerElement has no label() anymore !


  // =========================
  //       retrieve RoI:
  // =========================
#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
     (*m_log) << MSG::DEBUG << "getting RoI" << endmsg;
  }
#endif

  const TrigRoiDescriptor* roiDescriptor = 0;

  HLT::ErrorCode hltStatus = getFeature(inputTE, roiDescriptor);

  if ( hltStatus == HLT::OK ) {
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
      (*m_log) << "got the roi descriptor" << endmsg;
      (*m_log) << MSG::DEBUG  << *roiDescriptor << endmsg;
    }
#endif

  } else {
    (*m_log) <<  MSG::WARNING << " Failed to find RoiDescriptor " << endmsg;
    return hltStatus;
  }

  // Some debug output:
#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG) {
    (*m_log) << MSG::DEBUG  << "Inspect the LVL1 object; LVL1 phi="
	  << roiDescriptor->phi()
	  << ", LVL1 eta="
	  << roiDescriptor->eta()
	  << endmsg;
  }
#endif

  // =========================
  //       make Jet
  // =========================
#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
     (*m_log) << MSG::DEBUG  << " Making TrigT2Jet "<< endmsg;
  }
#endif
  m_jet = new TrigT2Jet();

  // Add RoI word to Jet
  m_jet->set_RoIword(roiDescriptor->roiWord());

  // Set eta,phi
  m_jet->setEta(roiDescriptor->eta());
  m_jet->setPhi(roiDescriptor->phi());
  
  if(timerSvc()) m_timer[1]->start(); // start of JE unpacking
  int counter=0;
  DataVector<LVL1::JetElement>::const_iterator begj, endj;

  if ( m_retrievedJetTool ){
#ifndef NDEBUG
      if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG  << "Loading the L1 data using ITrigT1CaloDataAccess tool, this loads the jet elements for the entire event"<< endmsg;
      }
#endif
      (m_dataL1->loadCollection(begj,endj)).ignore();
  }
  
  m_storedJEs->clear();
  
#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG  << "Looping over the jet elements"<< endmsg;
  }
#endif

  for(; begj!=endj;begj++){
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG  << "JetElement["<<counter<<"]: ETs [GeV]: EM: "<< (*begj)->emEnergy()<<", HAD: "<< (*begj)->hadEnergy()<<", Total: "<<(*begj)->energy()<<", eta: "<<(*begj)->eta()<<", phi: "<<(*begj)->phi()<<endmsg;
        }        
#endif

        m_storedJEs->push_back(*begj);
        counter++;
  }
#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "Stored L1 data as DataVector<LVL1::JetElement>* m_storedJEs" << endmsg;
        (*m_log) << MSG::DEBUG << "A total of " << counter << " elements were retreived" << endmsg;
        (*m_log) << MSG::DEBUG << "These will now be mapped onto a map of ints and LVL1::JetInput[s]" << endmsg;
  }
#endif
  
  std::map<int, LVL1::JetInput*> jetInputs;  // This will have to be cleared after being used
  if (m_retrievedJetTool) {
      m_jetL1Tools->mapJetInputs(m_storedJEs->asDataVector(), &jetInputs);
  }
  
  double roiPhi = (roiDescriptor)->phi();
  if( roiPhi < 0 ) roiPhi += 2 * M_PI;
  
  
#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG << "RoI descriptor phi: "<<(roiDescriptor)->phi()<<", in L1 coordinates: "<<roiPhi<<endmsg;
  }
#endif
  
  if(timerSvc()) m_timer[1]->stop(); // end of JE unpacking
  if(timerSvc()) m_timer[2]->start(); // start of jet finding
  
  if ( m_retrievedJetTool ) {
#ifndef NDEBUG
          if((*m_log).level() <= MSG::DEBUG){
             (*m_log) << MSG::DEBUG << "Using m_jetL1Tools to 'formSums' at the roi descriptor coordinates, this reproduces the L1 Jet object"<<endmsg;
          }
#endif
          m_jetL1Tools->formSums( (roiDescriptor)->eta(), roiPhi, & jetInputs );
          // now we must clear the map of jet inputs
          std::map<int, LVL1::JetInput*> ::const_iterator begin_JetInputMap,
                                                          end_JetInputMap;
          begin_JetInputMap = jetInputs.begin();
          end_JetInputMap = jetInputs.end();
          for(; begin_JetInputMap!=end_JetInputMap;begin_JetInputMap++){
              delete begin_JetInputMap->second;
          }
            
          m_jet->setEta( (roiDescriptor)->eta() );
          // The L1 roiDescriptor phi0 can sometimes be == Pi which causes navigation errors
          if ((roiDescriptor)->phi() >= M_PI){
#ifndef NDEBUG
              if((*m_log).level() <= MSG::DEBUG){
                  (*m_log) << MSG::DEBUG << "RoI descriptor phi >= M_PI, this causes issues with the creation of RoI descriptors: therefore setting phi to M_PI-0.001"<<endmsg;
              }
#endif
              m_jet->setPhi( M_PI-0.001 );
              }
          else{
              m_jet->setPhi( (roiDescriptor)->phi() );
          }
          //m_jet->setPhi( (roiDescriptor)->phi() );
          m_jet->setE ( m_jetL1Tools->ET8x8()*1e3*cosh((roiDescriptor)->eta()) );
          //m_jet->setEem0 ( 0.0 );
          //m_jet->setEhad0 ( 0.0 );
  }

#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
     (*m_log) << MSG::DEBUG << " Values of T2L1CaloJet produced: " << endmsg;
     (*m_log) << MSG::DEBUG << " REGTEST: Jet eta = " << m_jet->eta() << endmsg;
     (*m_log) << MSG::DEBUG << " REGTEST: Jet phi = " << m_jet->phi() << endmsg;
     (*m_log) << MSG::DEBUG << " REGTEST: Jet energy = " << m_jet->e() << endmsg;
     (*m_log) << MSG::DEBUG << " REGTEST: Jet et = " << m_jet->et() << endmsg;
  }
#endif

  m_e = m_jet->e();
  //m_ehad0 = m_jet->ehad0();
  //m_eem0 = m_jet->eem0();
  m_eta = m_jet->eta();
  m_et = m_e/cosh(m_eta);
  m_phi = m_jet->phi();
  
  if(timerSvc()) m_timer[2]->stop(); // end of jet finding
  if(timerSvc()) m_timer[3]->start(); // start of RoI creation

#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << "Recording created features"<<endmsg;
  }
#endif
  
  std::string key = "";

  hltStatus = recordAndAttachFeature(outputTE, m_jet, key, m_jetOutputKey);
  if (hltStatus != HLT::OK) {
    if((*m_log).level() <= MSG::DEBUG) {
      (*m_log) << MSG::ERROR << "Write of TrigT2Jet into outputTE failed"
	    << endmsg;
    }
    return hltStatus;
  }


  // =========================
  //       make OutputTE
  // =========================

  // Create a new RoiDescriptor with updated eta and phi.
  // Note that the steering will propagate l1Id and roiId automatically
  // so no need to set them.
  // How big is theis RoI ?????
  TrigRoiDescriptor* newRoiDescriptor =
    new TrigRoiDescriptor(roiDescriptor->roiWord(), roiDescriptor->l1Id(), roiDescriptor->roiId(),
			  m_jet->eta(), m_jet->eta(), m_jet->eta(), 
			  m_jet->phi(), m_jet->phi(), m_jet->phi()
			  ); // We forward the roiWord to the EF


  hltStatus = attachFeature(outputTE, newRoiDescriptor, m_jetOutputKey);

  if ( hltStatus != HLT::OK ) {
    (*m_log) << MSG::ERROR << "Write of update TrigRoiDescriptor into outputTE failed"
	  << endmsg;
    return hltStatus;
  }

#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG) {
    (*m_log) << MSG::DEBUG  << "Recorded an RoiDescriptor "
	  << " phi " <<  newRoiDescriptor->phi()
	  << " eta " << newRoiDescriptor->eta() << endmsg;
    (*m_log) << MSG::DEBUG
	  << "We assume success, set TE with Id "
	  << outputTE->getId()
	  << " active to signal positive result."
	  << endmsg;
  }
#endif
  
  if (timerSvc()) {
    m_timer[3]->stop();// end of RoI creation
    m_timer[0]->stop();// end of algorithm
  }  

#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << " Finished T2L1CaloJet FEX " << name() << endmsg;
  }
#endif
  
  return HLT::OK;
}


HLT::ErrorCode T2L1CaloJet::hltFinalize()
{
  m_storedJEs->clear();
  delete m_storedJEs;
 
  if ( (*m_log).level() <= MSG::INFO ){
    (*m_log) << MSG::INFO << "Finalizing: " << name() << endmsg;
    
  }
  delete m_log;
  
  return HLT::OK;
}

