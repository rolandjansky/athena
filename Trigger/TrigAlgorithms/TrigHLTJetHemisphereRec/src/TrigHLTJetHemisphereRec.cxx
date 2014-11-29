/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTJetHemisphereRec:Trigger algorithm that reads jet collection from
// an incoming trigger element, looks at all combinatorics for a hypothesis of
// two megajet objects. Hemispheres are chosen such that they
// minimize the mass of the di-hemisphere system. The resulting jet collection 
// contains hemisphere 4-vectors and is attatched to the outgoing trigger element.
//

#include "TrigHLTJetHemisphereRec/TrigHLTJetHemisphereRec.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include "JetEDM/IndexedConstituentUserInfo.h"

#include "xAODJet/Jet.h"

#include <algorithm>

TrigHLTJetHemisphereRec::TrigHLTJetHemisphereRec(const std::string& name, 
                           ISvcLocator* pSvcLocator ):
  HLT::FexAlgo( name, pSvcLocator ) {
  declareProperty( "jetPtCut", m_jetPtCut = 20000. );
  declareProperty( "jetEtaCut", m_jetEtaCut = 3.2 );
  declareProperty( "nJetPassThrough", m_nJetPassThrough = 10 );
}


TrigHLTJetHemisphereRec::~TrigHLTJetHemisphereRec() { }




HLT::ErrorCode TrigHLTJetHemisphereRec::hltInitialize() {

  // StatusCode sc;
  ATH_MSG_INFO("Initializing " << name() << "...");

  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    msg() << MSG::WARNING << name() << ": Unable to locate TrigTimer Service" << endreq;
  }
  if (m_timersvc) {
    TrigTimer* tmp = m_timersvc->addItem("TrigHLTJetHemisphereRec.TrigHLTJetHemisphereRecTot");
    m_timers.push_back(tmp);
    tmp = m_timersvc->addItem("TrigHLTJetHemisphereRec.TrigHLTJetHemisphereRecHemiFind");
    m_timers.push_back(tmp);
  }

  ATH_MSG_INFO("Leaving initialize in "  << name()  );

  return HLT::OK;

}


HLT::ErrorCode TrigHLTJetHemisphereRec::hltFinalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return HLT::OK;
}


HLT::ErrorCode TrigHLTJetHemisphereRec::acceptInput(const HLT::TriggerElement* inputTE, bool& pass){

  // Time total TrigEFRazorAllTE execution time.
  // -------------------------------------
  if (m_timersvc) m_timers[0]->start();


  ATH_MSG_INFO("In acceptInput() for " << name() << "...");

  // Incoming jet container //////////////////////////////////////////

  const xAOD::JetContainer*  jetContainer = 0;
  HLT::ErrorCode ec = getFeature(inputTE, jetContainer);

  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get the Jets " << endreq;
    return ec;
  }

  if (jetContainer == 0){
    ATH_MSG_ERROR("jetContainer address = 0, giving up");
    return HLT::ERROR;
  }

  ATH_MSG_DEBUG("Number of incoming jets: " << jetContainer->size());

  // Got input jet collection /////////////////////////////////////////

  // Preselected jet container - May be removed ///////////////////////

  m_selectedInputJetContainer.clear();

  // Attempt to do with STL below but failed for the time being... //
  for (xAOD::JetContainer::const_iterator jet = jetContainer->begin(); jet != jetContainer->end(); jet++) {
    if((*jet)->pt() < m_jetPtCut ) continue;
    if( fabs((*jet)->eta() ) > m_jetEtaCut ) continue;
    m_selectedInputJetContainer.push_back( (*jet)->jetP4() );
  }

  // remove_copy_if attempted but never got to work //
  // std::remove_copy_if( jetContainer->begin(), jetContainer->end(), std::back_inserter(m_selectedInputJetContainer), passJet);

  // Check number of preselected jets for passthrough requirement ////

  if (m_selectedInputJetContainer.size() < m_nJetPassThrough ){
    ATH_MSG_DEBUG("Continuing to hltExecute with Njet: " << m_selectedInputJetContainer.size());
    pass = true;
  } else {
    ATH_MSG_DEBUG("Too many jets input to hemisphere finding - Njet: " << m_selectedInputJetContainer.size());
    pass = false;
    if (m_timersvc) m_timers[0]->stop();
  }


  if (m_selectedInputJetContainer.size() < 2 ){ // njet < 2 is undefined
    pass = false;
  } 

  return HLT::OK;
}




HLT::ErrorCode TrigHLTJetHemisphereRec::hltExecute(const HLT::TriggerElement* inputTE,
                                                   HLT::TriggerElement* outputTE) {  





  ATH_MSG_INFO("Executing " << name() << "...");

  ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());
  ATH_MSG_DEBUG("inputTE->getId(): " << inputTE->getId());


  // Output jet hemisphere container creation //////////////////////

  xAOD::JetContainer* jetHemisphereContainer = new xAOD::JetContainer();
  xAOD::JetTrigAuxContainer theJetHemisphereContainer;
  jetHemisphereContainer->setStore(&theJetHemisphereContainer);

  if (m_timersvc) m_timers[1]->start();

  int nJets = m_selectedInputJetContainer.size() ;

  ATH_MSG_DEBUG("size of m_selectedInputJetContainer " << nJets );

  /////////// Starting Hemisphere Reconstruction ////////////////////////
  /////////// Adapted from Chris Rogan //////////////////////////////////

  double M_min = -1.;
  int j_count;
  
  int itemp = 0;
  int count = 0;

  double M_temp=0;

  xAOD::JetFourMom_t h1,h2;
  xAOD::JetFourMom_t h1temp,h2temp;



  // Brute force loop over 2^N-1 jet combinations //////////////////////
  // Not great, we know. But it does the job for now... ////////////////

  int N_comb = pow(2.,m_selectedInputJetContainer.size() );

  for(int i = 1; i < N_comb-1; i++){

    itemp = i;
    j_count = N_comb/2.;
    count = 0;
    h1temp *= 0; h2temp *= 0;
    while(j_count > 0){
        if(itemp/j_count == 1){
            h1temp += m_selectedInputJetContainer.at(count);
        } else {
            h2temp += m_selectedInputJetContainer.at(count);
        }
        itemp -= j_count*(itemp/j_count);
        j_count /= 2.;
        count++;
    }

    if(h1temp.M2()==0. || h2temp.M2()==0.) continue;

    M_temp = h1temp.M2() + h2temp.M2();
    
    // Trying to minimize combined hemisphere mass //////////

    if(M_temp < M_min || M_min < 0){
        M_min = M_temp;
        h1 = h1temp;
        h2 = h2temp;
    }
  }



  // xAOD::JetFourMom_t TotalMomentum;


  // ////////////////////////////
  // // Brute force algorithm that grew as 2^N was replaced by an algorithm
  // // inspired by the thrust calculation in Pythia8 that grows as N^3.
  // // Much better.

  // // Adapted from Crogan, who adapted from Pythia8

  // if(nJets==2){
  //   h1 = m_selectedInputJetContainer.at(0);
  //   h2 = m_selectedInputJetContainer.at(1);
  // } else {

  //   for(int i = 0; i < (int) m_selectedInputJetContainer.size(); i++ ){
  //     TotalMomentum += m_selectedInputJetContainer.at(i);
  //   }

  //   TVector3 BoostToCM = TotalMomentum.BoostVector();

  //   for(int i = 0; i < (int) m_selectedInputJetContainer.size(); i++ ){
  //     m_selectedInputJetContainer.at(i).Boost(-BoostToCM);
  //   }

  //   xAOD::JetFourMom_t j1_temp, j2_temp;
  //   xAOD::JetFourMom_t j1_temp2, j2_temp2; 

  //   double P_max = 0.;

  //   for(int i0 = 0; i0 < Njet-1; i0++){
  //     for(int i1 = i0+1; i1 < Njet; i1++){
  //       TVector3 nRef = m_selectedInputJetContainer.at(i0).Vect().Cross(
  //         m_selectedInputJetContainer.at(i1).Vect() );
  //       j1_temp *= 0.;
  //       j2_temp *= 0.;
  //       int N1 = 0;
  //       int N2 = 0;
  //       for(int iall = 0; iall < Njet; iall++){
  //        if((iall == i0) || (iall == i1)) continue;
  //        if(m_selectedInputJetContainer.at(iall).Vect().Dot(nRef) > 0.){
  //          j1_temp += m_selectedInputJetContainer.at(iall);
  //          N1++;
  //        } else {
  //          j2_temp += m_selectedInputJetContainer.at(iall);
  //          N2++;
  //        }
  //      }
  //     // consider four cases for two probes
  //      for(int i = 0; i < 2; i++){
  //        for(int j = 0; j < 2; j++){
  //          if(i == 0 && j == 0 && N2 == 0) continue;
  //          if(i == 1 && j == 1 && N1 == 0) continue;      
  //          j1_temp2 = j1_temp + float( (i+1)%2 )*m_selectedInputJetContainer.at(i0) + float( (j+1)%2 )*m_selectedInputJetContainer.at(i1);
  //          j2_temp2 = j2_temp + float(i)*m_selectedInputJetContainer.at(i0) + float(j)*m_selectedInputJetContainer.at(i1);

  //          double P_temp = j1_temp2.P()+j2_temp2.P();
  //           // largest momentum in CM
  //          if(P_temp > P_max){
  //            P_max = P_temp;
  //            j1 = j1_temp2;
  //            j2 = j2_temp2;
  //          }
  //        }
  //      }
  //    }

  //    h1.Boost(BoostToCM);
  //    h2.Boost(BoostToCM);

  //  } 


  // Why not have hemisphere 1 be the harder one?... ///////

  if(h2.Pt() > h1.Pt() ){
      xAOD::JetFourMom_t temp = h1;
      h1 = h2;
      h2 = temp;
  }

  // Final jet objects that will go into the output collection /////////////

  xAOD::Jet* aJet = new xAOD::Jet();
  xAOD::Jet* bJet = new xAOD::Jet();

  ATH_MSG_DEBUG("About to fill hemisphere container: " << jetHemisphereContainer->size() << " " << h1.Pt() << " " << h2.Pt() );

  jetHemisphereContainer->push_back(aJet);
  jetHemisphereContainer->push_back(bJet);

  aJet->setJetP4( h1 );
  bJet->setJetP4( h2 );

  HLT::ErrorCode hltStatus;

  std::string key = "";

  // Put the hemisphere container into the output TE ///////////////////////

  hltStatus=recordAndAttachFeature(outputTE, jetHemisphereContainer,key,"TrigHLTJetHemisphereRec");

  ATH_MSG_DEBUG("Number of output hemispheres (should be 2): " << jetHemisphereContainer->size());

  // Time total TrigEFRazorAllTE execution time.
  // -------------------------------------

  if (m_timersvc) m_timers[0]->stop();
  if (m_timersvc) m_timers[1]->stop();

  ATH_MSG_DEBUG("Algorithm took time: " << m_timers[0]->elapsed() ) ;
  ATH_MSG_DEBUG("Hemisphere finding took time: " << m_timers[1]->elapsed() ) ;

  return hltStatus;
}




HLT::ErrorCode TrigHLTJetHemisphereRec::attachJetCollection(HLT::TriggerElement* outputTE,
                                   const xAOD::JetContainer* j_container){

  // We have to explicitly delete the aux store, so get a pointer to it.
  auto auxStore = j_container->getStore();
  std::string label = "TrigHLTJetHemisphereRec";
  std::string key = "";
  
  HLT::ErrorCode hltStatus = 
    recordAndAttachFeature(outputTE, j_container, key, label);
  
  // cleanup
  if (hltStatus != HLT::OK) {
    // this is unrecoverable. report error, and do not try to clean up.
    ATH_MSG_ERROR("Failed to attach xAOD::JetContainer into outputTE- status"
                  << hltStatus);
  } else {
    // j_container.setStore(0) is called in downstream code, so the
    // aux container needs to deleted explcitly here.
    delete auxStore;
    ATH_MSG_DEBUG("Attached xAOD::JetContainer into outputTE");
  }
  return hltStatus;
}


// bool TrigHLTJetHemisphereRec::passJet(xAOD::Jet* jet){
//     if(jet->pt() < m_jetPtCut ) return false;
//     if( fabs(jet->eta() ) > m_jetEtaCut ) return false;

//     return true;
// }


