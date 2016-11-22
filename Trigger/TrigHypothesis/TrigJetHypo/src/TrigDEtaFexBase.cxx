/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * 
 * Author: James Monk <jmonk@hep.ucl.ac.uk>
 *
 * This FEX algorithm runs over all jets, orders them in eta and stores in the output TE
 * those jets that are separated by delta eta > detaCut.
 * Since the jets are ordered in eta, there can be no third jet in the gap that is above the
 * ET cut of the input jets.
 *
 * If there are multiple gaps then all jets in all gaps are stored.  Any jet that is on the boundary 
 * of both gaps will be stored twice, hence there is always an even number of jets in the output TE.
 *
 */


#include "TrigJetHypo/TrigDEtaFexBase.h"

#include "EventKernel/INavigable4Momentum.h"
//#include "TrigCaloEvent/TrigT2Jet.h"
//#include "TrigCaloEvent/TrigT2JetContainer.h"
//#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <map>

/// A struct to use with stl sort to sort the jets by et
struct sorter{
  bool operator () (const INavigable4Momentum *left, const INavigable4Momentum *right) const {
    return left->et() > right->et();
  }
};

/// A struct to use with stl sort to sort the jets by eta
struct sorter_eta{
  bool operator () (const INavigable4Momentum *left, const INavigable4Momentum *right) const {
    return left->eta() > right->eta();
  }
};


TrigDEtaFexBase::TrigDEtaFexBase(const std::string & name, ISvcLocator* pSvcLocator):
HLT::AllTEAlgo(name, pSvcLocator){
  
  declareProperty("DEtaCut", m_dEtaCut=2.0);
  declareProperty("MjjCut", m_MjjCut=-100*CLHEP::GeV, "Minimal Invariant Mass of two Jets");
  declareProperty("DEtaLabel", m_label = "DeltaEta", "Label for the jets delta eta feature in the HLT");
  declareProperty("EtCut", m_EtCut=-100*CLHEP::GeV);//leave default value -100 for backwards compatibility
  declareProperty("EtaMaxCut", m_etaMaxCut=100);
  declareProperty("CheckMostForwardBackwardPair", m_checkMostForwardBackwardPair=false, 
		  "Check DeltaEta for most forward/backward pair of jets");

  declareMonitoredVariable("GapSize", m_gapSize);
  declareMonitoredStdContainer("JetEta", m_jetEta);
  declareMonitoredVariable("NJets", m_nJets);
  declareMonitoredVariable("NJetsSelected", m_nJetsSelected);
}


HLT::ErrorCode TrigDEtaFexBase::hltInitialize(){
  
  msg()<<MSG::DEBUG<<"Hello from TrigDEtaFex algorithm"<<endmsg;
  
  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    msg() << MSG::WARNING << name()
    << ": Unable to locate TrigTimer Service" << endmsg;
  }
  if (m_timersvc) {
    
    TrigTimer* tmp = m_timersvc->addItem("TrigDEtaFex.TrigDEtaFexTot");
    m_timers.push_back(tmp);
  }
  
  return HLT::OK;
}

HLT::ErrorCode TrigDEtaFexBase::hltExecute(std::vector<std::vector<HLT::TriggerElement*> > &inputTEs,
                                           unsigned int outType){

  m_nJets = 0;
  m_nJetsSelected = 0;
  
  msg()<<MSG::DEBUG<<"Hello from TrigDEtaFex::hltExecute"<<endmsg;
  
  m_gapSize = -1.0;
  
  // Can only deal with input of one type of TE
  if(inputTEs.size() != 1){

    msg() << MSG::WARNING <<" Number of input TE types is "<<inputTEs.size()<<".  It should be 1"<<std::endl;

    return HLT::OK;
  }

  
  std::map<const INavigable4Momentum*, HLT::TriggerElement*, sorter> allJets;
  std::map<const INavigable4Momentum*, HLT::TriggerElement*, sorter_eta> allJets_eta;
  
  for(std::vector<std::vector<HLT::TriggerElement*> >::iterator it = inputTEs.begin();
      it != inputTEs.end(); ++it){
    
    for(std::vector<HLT::TriggerElement*>::const_iterator te = it->begin();
        te != it->end(); ++te){
      
      std::vector<const INavigable4Momentum*> jets;
      HLT::ErrorCode status = getJets(*te, jets);
      
      //      std::vector<const TrigT2Jet*> jets;
      //      HLT::ErrorCode status = getFeatures((*te), jets);
      
      if(status == HLT::OK){
        
        for(std::vector<const INavigable4Momentum*>::const_iterator jet = jets.begin();
            jet != jets.end(); ++jet){
	  m_nJets++;
	  if( (*jet)->et() < m_EtCut ) continue;
	  if( fabs((*jet)->eta()) > m_etaMaxCut ) continue;
	  m_nJetsSelected++;
          if(!m_checkMostForwardBackwardPair) allJets[*jet] = *te;
	  else allJets_eta[*jet] = *te;
        }
      }
    }
  }
  
  m_gapSize = 0.0;
  
  int nJets = allJets.size();
  if(m_checkMostForwardBackwardPair) nJets = allJets_eta.size();

  msg()<<MSG::DEBUG<<" There are "<<nJets<<" jets selected from "<<m_nJets<<" jets"<<endmsg;
  
  std::set<HLT::TriggerElement*> dEtaTEs;
  std::vector<const INavigable4Momentum*> dEtaJets;
  
  // If there are fewer than two jets then stop here
  if(nJets< 2){
    
    msg()<<MSG::DEBUG<<" not enough jets above ET threshold!"<<endmsg;
    
    return HLT::OK;
  }
  
  std::map<const INavigable4Momentum*, HLT::TriggerElement*>::const_iterator highJet = allJets.begin(); 
  std::map<const INavigable4Momentum*, HLT::TriggerElement*>::const_iterator end = allJets.end(); 
  if(m_checkMostForwardBackwardPair) {
    highJet = allJets_eta.begin();
    end = allJets_eta.end();
  }

  for(; highJet != end; ++highJet){
    msg()<<MSG::DEBUG<<"jet et: "<< highJet->first->et() << " eta: "<< highJet->first->eta() << endmsg;
  }

  highJet = allJets.begin(); 
  if(m_checkMostForwardBackwardPair) highJet = allJets_eta.begin();

  std::map<const INavigable4Momentum*, HLT::TriggerElement*>::const_iterator lowJet = highJet++;
  if(m_checkMostForwardBackwardPair) {
    highJet = allJets_eta.begin();
    lowJet = allJets_eta.end();
    lowJet--;
  }

  msg()<<MSG::DEBUG<< "jet1 et: "<< lowJet->first->et() << " eta: "<< lowJet->first->eta() << " jet2 et: " << highJet->first->et() << " eta: "<< highJet->first->eta() <<  endmsg;

  //absolute value??
  double deta = fabs(highJet->first->eta() - lowJet->first->eta());
  double mjj = (highJet->first->hlv()+lowJet->first->hlv()).m();

  if(deta > m_dEtaCut && mjj > m_MjjCut){
    
    msg()<<MSG::DEBUG<<"found gap of size "<<deta<<endmsg;
    
    //monitor only the largest gap size in the event
    if(deta > m_gapSize) m_gapSize = deta;
    m_jetEta.push_back(highJet->first->eta());
    m_jetEta.push_back(lowJet->first->eta());

    dEtaTEs.insert(lowJet->second);
    dEtaTEs.insert(highJet->second);
    
    dEtaJets.push_back(lowJet->first);
    dEtaJets.push_back(highJet->first);
  }


  int counter=0;
//   for(std::map<const INavigable4Momentum*, HLT::TriggerElement*>::const_iterator highJet = allJets.begin(),
// 	lowJet = highJet++;//lowjet = highjet; ++highjet;
//       highJet != allJets.end(); ++lowJet, ++highJet){
    
//     //only consider leading jet pair
//     if(counter>0)break;

//     msg()<<MSG::DEBUG<< "jet1 et: "<< lowJet->first->et() << " eta: "<< lowJet->first->eta() << " jet2 et: " << highJet->first->et() << " eta: "<< highJet->first->eta() <<  endmsg;

//     //absolute value??
//     double deta = fabs(highJet->first->eta() - lowJet->first->eta());
    
//     if(deta > m_dEtaCut){
      
//       msg()<<MSG::DEBUG<<"found gap of size "<<deta<<endmsg;
      
//       //monitor only the largest gap size in the event
//       if(deta > m_gapSize) m_gapSize = deta;
//       m_jetEta.push_back(highJet->first->eta());
//       m_jetEta.push_back(lowJet->first->eta());

//       dEtaTEs.insert(lowJet->second);
//       dEtaTEs.insert(highJet->second);
      
//       dEtaJets.push_back(lowJet->first);
//       dEtaJets.push_back(highJet->first);
//     }
//     counter++;
//   }
  
  std::vector<HLT::TriggerElement*> teVec(dEtaTEs.begin(), dEtaTEs.end());
  
  // If no gap is found then stop here
  if(dEtaTEs.size() == 0){
    msg()<<MSG::DEBUG<<" no gap found!"<<endmsg;
    return HLT::OK;
  }


  // If more than one jet pair considered return WARNING
  if(counter > 1){
    msg()<<MSG::WARNING<<" More than one jet pair considered! Only two leading jets should be used"<<endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN);  
  }
  
  HLT::TriggerElement *output = config()->getNavigation()->addNode(teVec, outType);
  output->setActiveState(true);
  
  HLT::ErrorCode status = saveJets(output, dEtaJets, m_label);
  if(status != HLT::OK){
    msg()<<MSG::ERROR<<"Could not write jet for delta eta into output TE"<<endmsg;
    return status;
  }
  
  /*
  for(std::vector<const INavigable4Momentum*>::const_iterator jet = dEtaJets.begin();
      jet != dEtaJets.end(); ++jet){
    //HLT::ErrorCode status = reAttachFeature(output, dynamic_cast<const TrigT2Jet*>(*jet), m_label);
    HLT::ErrorCode status = reAttachFeature(output, *jet, m_label);
    if(status != HLT::OK){
      msg()<<MSG::ERROR<<"Could not write jet for delta eta into output TE"<<endmsg;
      return status;
    }
  }
  */
  msg()<<MSG::DEBUG<<" found gap: " << m_gapSize <<" and finished!"<<endmsg;
  
  return HLT::OK;
}

HLT::ErrorCode TrigDEtaFexBase::hltFinalize(){
  return HLT::OK;
}
