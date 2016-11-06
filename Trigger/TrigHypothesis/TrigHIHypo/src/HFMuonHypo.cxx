/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <xAODMuon/Muon.h>
#include <xAODMuon/MuonContainer.h>
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "HFMuonHypo.h"

//Useful links
//http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODMuon/xAODMuon/versions/Muon_v1.h
//http://acode-browser2.usatlas.bnl.gov/lxr-rel20/source/atlas/InnerDetector/InDetTrigRecAlgs/InDetTrigVxSecondary/src/TrigVxSecondary.cxx

HFMuonHypo::HFMuonHypo(const std::string& name, ISvcLocator* pSvcLocator) 
  : HLT::HypoAlgo(name, pSvcLocator){
  ATH_MSG_INFO("IN HFMuonHypo Constructor");

  declareMonitoredVariable("eloss_all"       , m_eloss_all        , -1.);
  declareMonitoredVariable("eloss_passing"   , m_eloss_passing    , -1.);

  declareProperty("ElossCut", m_eloss_cut=0.1, "Cut on momentum imbalance");
}

HLT::ErrorCode HFMuonHypo::hltInitialize() {
  ATH_MSG_INFO("IN HFMuonHypo hltInitialize()");
  return HLT::OK; 
}


HFMuonHypo::~HFMuonHypo() {
}


HLT::ErrorCode HFMuonHypo::hltFinalize( ) { return HLT::OK; }

HLT::ErrorCode HFMuonHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  pass = false;

  //static int event=0;
  //std::cout<<"TEST "<<event++<<std::endl;

  //Get Muon Container
  const xAOD::MuonContainer* m_muons=nullptr;
  HLT::ErrorCode statusMuons;
  statusMuons=getFeature(outputTE,m_muons);
  if(statusMuons== HLT::OK && m_muons!=nullptr){
    ATH_MSG_DEBUG("Got MuonContainer object successfully");
  }
  else {
    ATH_MSG_DEBUG("The MuonContainer object is inaccessible");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }


  //Get primary vertex collection an then the primary vertex
  const xAOD::VertexContainer* prmVtxColl=nullptr;
  if(getFeature(outputTE,prmVtxColl)==HLT::OK && prmVtxColl!=nullptr){
    ATH_MSG_DEBUG("Got PrimaryVertexCollection successfully");
  }
  else {
    ATH_MSG_DEBUG("The PrimaryVertexCollection is inaccessible");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
  const xAOD::Vertex* priVtx = *(prmVtxColl->cbegin());
  if (priVtx->vertexType() != xAOD::VxType::PriVtx) {
    ATH_MSG_WARNING( "First vertex is not of type \"Primary Vertex\"." );
    return HLT::OK;
  }


  //Get TrackParticle Collection
  const xAOD::TrackParticleContainer* tracks=nullptr;
  if(getFeature(outputTE,tracks)==HLT::OK || tracks==nullptr) {
    ATH_MSG_DEBUG("Got TrackParticleContainer successfully");
  }
  else {
    ATH_MSG_DEBUG("The TrackParticleContainer is inaccessible");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
 


  //loop over all muons asd check if one of them meets the criteria
  //to pass the trigger
  for(auto Muon : *m_muons){
    //User only combined muons
    if(Muon->muonType() !=xAOD::Muon::MuonType::Combined) continue;

    //Quality
    if(!(Muon->quality()==xAOD::Muon::Medium || Muon->quality()==xAOD::Muon::Tight)) continue; 

    //Determine Eloss, now cutting on it as well
/*
    const xAOD::TrackParticle*msTrk= Muon->trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle);//MS Only Track
    const xAOD::TrackParticle*idTrk= Muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle);//ID Track
    if(!msTrk || !idTrk){
      if(!msTrk) ATH_MSG_WARNING("msTrk Not Found for muon, will abort chain");
      if(!idTrk) ATH_MSG_WARNING("idTrk Not Found for muon, will abort chain");
      //return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
    }
    ATH_MSG_INFO("Found both track segements");
    float eloss_parameterized;//paramaterized eloss
    if(!(Muon->parameter(eloss_parameterized,xAOD::Muon::ParamEnergyLoss))){
      ATH_MSG_DEBUG("The parameter xAOD::Muon::ParamEnergyLoss is inaccessible");
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
    }
    double eloss=(idTrk->p4().P() - msTrk->p4().P() - eloss_parameterized)/idTrk->p4().P();
// */

    const xAOD::TrackParticle*idTrk= Muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle);//ID Track
    const xAOD::TrackParticle*meTrk= Muon->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);//ME Track
    if(!meTrk || !idTrk){
      if(!meTrk) ATH_MSG_WARNING("meTrk Not Found for muon, will abort chain");
      if(!idTrk) ATH_MSG_WARNING("idTrk Not Found for muon, will abort chain");
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
    }
    ATH_MSG_INFO("Found both muon track segements");
    double eloss=(idTrk->p4().P() - meTrk->p4().P())/idTrk->p4().P();

    if(eloss>m_eloss_cut) continue; 


    //Insert code to match with tracks and see
    //if it is seperated from the primary vertex
    pass=true;
  }

  if(pass==true) ATH_MSG_DEBUG("Accepting Event");
  else           ATH_MSG_DEBUG("Rejecting Event");

  return HLT::OK;
}
