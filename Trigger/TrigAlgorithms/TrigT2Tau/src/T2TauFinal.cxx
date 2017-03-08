/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2TauFinal.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2Tau
// 
// AUTHOR:   M.P. Casado
// 
// Purpose: Create TrigTau particle and perform energy calibration.
// ********************************************************************
 

#include "GaudiKernel/IToolSvc.h"
#include "TrigT2Tau/T2TauFinal.h"
#include "TrigT2Tau/IAlgToolTau.h"


#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEvent/TrigTauClusterContainer.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigParticle/TrigTau.h"
#include "TrigParticle/TrigTauContainer.h"

class ISvcLocator;

T2TauFinal::T2TauFinal(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator),
  m_algTools(this),
  m_Eta (-99),
  m_Phi  (-99),
  m_EtCombined(-99),
  m_simpleEtFlow(-99),
  m_nMatchedTracks(-99),
  m_dPhiTrigTau_cluster(-99),
  m_dEtaTrigTau_cluster(-99),
  m_trkAvgDist(-99),
  m_etOverPtLeadTrk(-99),
  m_updateRoiDescriptor(false)
{

/** List of T2Tau tools */
  declareProperty("T2TauTools", m_algTools,"list of AlgToolTau");

//   declareMonitoredVariable("EtaL1",m_EtaL1);
//   declareMonitoredVariable("PhiL1",m_PhiL1);
  declareMonitoredVariable("Eta",m_Eta);
  declareMonitoredVariable("Phi",m_Phi);  
  declareMonitoredVariable("EtCombined",m_EtCombined);
  declareMonitoredVariable("SimpleEtFlow",m_simpleEtFlow);
  declareMonitoredVariable("NMatchedTracks",m_nMatchedTracks);
  declareMonitoredVariable("dPhiTrigTau_cluster",m_dPhiTrigTau_cluster);
  declareMonitoredVariable("dEtaTrigTau_cluster",m_dEtaTrigTau_cluster);
  declareMonitoredVariable("trkAvgDist",m_trkAvgDist);
  declareMonitoredVariable("etOverPtLeadTrk",m_etOverPtLeadTrk);

  declareMonitoredCustomVariable("DeltaRTkClust", new T2TauFinalMonOfDelta(*&myTrackColl,*&roiDescriptor,1) );
  declareMonitoredCustomVariable("DeltaEtaTkClust", new T2TauFinalMonOfDelta(*&myTrackColl,*&roiDescriptor,2) );
  declareMonitoredCustomVariable("DeltaPhiTkClust", new T2TauFinalMonOfDelta(*&myTrackColl,*&roiDescriptor,3) );
  
  declareProperty("updateRoiDescriptor", m_updateRoiDescriptor, "option to update RoiDescriptor after execution");

  pTrackColl = 0;
  myTrackColl = 0;
  roiDescriptor = 0;
 
}

T2TauFinal::~T2TauFinal(){
}

/*-------------------------------------------*/
HLT::ErrorCode T2TauFinal::hltInitialize()
/*-------------------------------------------*/
{
  if(msgLvl() <= MSG::DEBUG){ msg() << MSG::DEBUG  << "Initialization set of tools" << endmsg;}
  
  ToolHandleArray<IAlgToolTau>::iterator it = m_algTools.begin();
  
  for (; it != m_algTools.end();  ++it) {
    StatusCode sc = it->retrieve();
    if( sc.isFailure() ) {
      msg() << MSG::ERROR << "Unable to initialize tool " << *it << endmsg;
      return HLT::BAD_ALGO_CONFIG ;
    }
    if (msgLvl() <= MSG::DEBUG) 
        msg() << MSG::DEBUG << "REGTEST: Created " << *it << " AlgTool" << endmsg;
    
  }
  
  
  if(  msgLvl() <= MSG::DEBUG ) {
    if( m_updateRoiDescriptor )
        msg() << MSG::DEBUG << "REGTEST: TrigRoiDescriptor will be updated " << endmsg;
    else
        msg() << MSG::DEBUG << "REGTEST: TrigRoiDescriptor will NOT be updated " << endmsg;
  }
  
  if ( msgLvl() <= MSG::DEBUG)  
      msg() << MSG::DEBUG << "Initialization completed successfully" << endmsg;
  
  return HLT::OK;
}

/*-------------------------------------------*/
HLT::ErrorCode T2TauFinal::hltFinalize()
/*-------------------------------------------*/
{
  
  msg() << MSG::DEBUG << "in finalize()" << endmsg;
  return HLT::OK;
}

/*------------------------------------------------*/
HLT::ErrorCode T2TauFinal::hltExecute(const HLT::TriggerElement* inputTE, 
				      HLT::TriggerElement* outputTE)
/*------------------------------------------------*/
{
  
  
//   m_EtaL1 = -99;
//   m_PhiL1 = -99;
  m_Eta   = -99;
  m_Phi   = -99;
  //m_EtCalib = -99;
  m_EtCombined = -99;
  m_simpleEtFlow = -99;
  m_nMatchedTracks = -99;
  m_dPhiTrigTau_cluster = -99;
  m_dEtaTrigTau_cluster = -99;
  m_trkAvgDist = -99;
  m_etOverPtLeadTrk = -99;
  
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endmsg;
  
  //--------- 1. get RoI descriptor
  //const TrigRoiDescriptor* roiDescriptor = 0;
  roiDescriptor = 0;
  
  HLT::ErrorCode hltStatus = getFeature(inputTE, roiDescriptor);
  
  if ( hltStatus == HLT::OK && roiDescriptor !=0 ) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: RoI id " 
				      << roiDescriptor->roiId()
				      << " word " << roiDescriptor->roiWord()
				      << " LVL1 id " << roiDescriptor->l1Id()
				      << " located at  eta=" << roiDescriptor->eta() 
				      << ", phi=" <<  roiDescriptor->phi() << endmsg;
  } else {
    msg() <<  MSG::ERROR << " No RoI for this Trigger Element! " << endmsg;
    return hltStatus;
  }

  //-------- 2.  get cluster
  const TrigTauCluster* pClus(0);  
  hltStatus = getFeature(inputTE, pClus ); 

  if(hltStatus== HLT::OK) {
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Got TrigTauCluster" << endmsg;
  }else{
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "REGTEST: No TrigTauCluster is found in RoI! " << endmsg;
  }
    
    
  //-------- 3. Get tracksInfo
    
  const TrigTauTracksInfo* pTracksInfo(0);
  hltStatus = getFeature(inputTE, pTracksInfo ); 

  if(hltStatus== HLT::OK) {
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Got  TrigTauTracksInfo" << endmsg;
  }else{
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "REGTEST: No TrigTauTracksInfo is found in RoI! " << endmsg;
  }
    
  //-------- 3. Get vector of tracks 
    
  //const TrigInDetTrackCollection* pTrackColl(0);
  pTrackColl = 0;
  if( pTracksInfo != 0 ){
    hltStatus = getFeature(inputTE, pTrackColl ); 
    if(hltStatus== HLT::OK) {
      if (msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << "Got TrigInDetTrackCollection as well " << endmsg;
    }else{
      if (msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << "REGTEST: No TrigInDetTrackCollection is found in RoI but TrigTauTracksInfo exists! " << endmsg;
    }    
  }

  myTrackColl = pTrackColl; //Necessary for downcasting object for monitoring.

  //OI: now, let us check if pointer to track is set in the tracksInfo. It is needed for matching tool
  // this is actually really ugly, as collection can be from SiTrack, while Info is from IDSCAN
  // however, if we re-run HYPOs, we would have these set to 0, and therefore matching will not work.. 
  // Is there a nicer way to proceed?
  if( pTrackColl != 0 && pTracksInfo!=0){
    if( pTracksInfo->trackCollection() == 0)
      {
	TrigTauTracksInfo* ptmp = const_cast< TrigTauTracksInfo*>(pTracksInfo);
	ptmp->setTrackCollection(pTrackColl);
      }
  }

  if(hltStatus== HLT::OK) {
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Got TrigInDetTrackCollection " << endmsg;
  }else{
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "REGTEST: No TrigInDetTrackCollection is found in RoI! " << endmsg;
  }

  //-------- 4. Make new object
  if( pClus== 0 && pTracksInfo == 0 )
    {
      if (msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << " Nor TrigTauCluster neither  TrigInDetTrackCollection is found in RoI! Wrong configuration!" << endmsg;
      return HLT::OK;
    }
  
  TrigTau* pTrigTau = new TrigTau();
  pTrigTau->setRoiId((int)roiDescriptor->roiWord());
  pTrigTau->setTauCluster(pClus);
  pTrigTau->setTrackCollection(pTrackColl);
  pTrigTau->setTracksInfo(pTracksInfo);
  pTrigTau->setEta(roiDescriptor->eta()); // to be changed by T2TauEtaPhiTool
  pTrigTau->setPhi(roiDescriptor->phi());
  
  
  ToolHandleArray<IAlgToolTau>::iterator it = m_algTools.begin();
  for (; it < m_algTools.end(); it++)  {
    if ((*it)->execute(pClus,pTracksInfo, *pTrigTau).isFailure() ) {
      msg() << MSG::WARNING << "T2TauFinal AlgTool "<<*it<< " returned Failure" << endmsg;
      delete pTrigTau;
      pTrigTau=0;
      return HLT::TOOL_FAILURE;
    }
  }
 
  
  hltStatus = attachFeature(outputTE, pTrigTau );
  if (hltStatus != HLT::OK ){
    msg() << MSG::ERROR << "Write of TrigTau into outputTE failed" << endmsg;
    pTrigTau=0;
    return hltStatus;
  }
  if(  m_updateRoiDescriptor ) {

    /// NB: What is the RoiDescriptor for ?? Need to create with the correct size 
    ///     If this is for use by this algorithm later, how big should it be ?
    ///     If it is for the IDTrigger, need to know what the resolution of the 
    ///     found objects actually is. 
    ///     Temporarily create an INCORRECT 0 size RoiDescriptor

    msg() << MSG::WARNING << "Creating an RoiDescriptor with NO SIZE !!!! this is not allowed" << endmsg;
    
    TrigRoiDescriptor* newRoiDescriptor = new TrigRoiDescriptor( roiDescriptor->roiWord(), 
								 roiDescriptor->l1Id(), 
								 roiDescriptor->roiId(),
								 pTrigTau->eta(), pTrigTau->eta(), pTrigTau->eta(), 
								 pTrigTau->phi(), pTrigTau->phi(), pTrigTau->phi() );
    
  hltStatus = attachFeature(outputTE, newRoiDescriptor, "T2TauFinal"); 
  if (hltStatus != HLT::OK ){
    msg() << MSG::ERROR << "Can not attach new RoI descriptor" << endmsg;
   delete newRoiDescriptor; 
   return hltStatus;
  }
  if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG  << "REGTEST: Recorded an RoiDescriptor " << newRoiDescriptor->roiWord() <<
    " eta=" << newRoiDescriptor->eta() <<
    ", phi=" <<  newRoiDescriptor->phi() << endmsg;
  }
  if(msgLvl() <= MSG::DEBUG){
    //  msg() << MSG::DEBUG  << "REGTEST: Recorded an RoiDescriptor " << newRoiDescriptor->roiWord() <<
    //" eta=" << newRoiDescriptor->eta() <<
    //", phi=" <<  newRoiDescriptor->phi() << endmsg;
    msg() << MSG::DEBUG << "REGTEST: TrigTau::pt,eta,phi = " << pTrigTau->pt() <<", " <<
      pTrigTau->eta() <<", " << pTrigTau->phi() << endmsg;
    msg() << MSG::DEBUG << "REGTEST: TrigTau::m_nMatchedTracks; = " << pTrigTau->nMatchedTracks() << endmsg;
    //msg() << MSG::DEBUG << "REGTEST: TrigTau::etCalibCluster = " << pTrigTau->etCalibCluster() << endmsg;
    msg() << MSG::DEBUG << "REGTEST: TrigTau::simpleEtFlow = " << pTrigTau->simpleEtFlow() << endmsg;
  }
   
  // 
  // Get L1 RoiDescriptor
  const TrigRoiDescriptor* roiL1Descriptor = 0;
  /*HLT::ErrorCode tmpStatus =*/ getFeature(inputTE, roiL1Descriptor,"initialRoI");

//   if(roiL1Descriptor){
//     m_EtaL1         =  roiL1Descriptor->eta();
//     m_PhiL1         =  roiL1Descriptor->phi();
//   } 
  m_Eta = pTrigTau->eta();
  m_Phi = pTrigTau->phi();
  m_dPhiTrigTau_cluster = pTrigTau->phi() - roiL1Descriptor->phi();
  m_dEtaTrigTau_cluster = pTrigTau->eta() - roiL1Descriptor->eta();
  // m_EtCalib  = pTrigTau->etCalibCluster();
  m_EtCombined =  pTrigTau->et();
  m_simpleEtFlow =  pTrigTau->simpleEtFlow();
  m_nMatchedTracks = pTrigTau->nMatchedTracks();
  m_trkAvgDist = pTrigTau->trkAvgDist();
  /** etOverPtLeadTrk of TrigTau created (variable for histogramming) */
  m_etOverPtLeadTrk = pTrigTau->etOverPtLeadTrk();




  
  // Reach this point successfully   
  if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "REGTEST: always accept RoI " << endmsg;
  
    
  return HLT::OK;
}

//////////////////////////////////////////////////////////
double T2TauFinalMonOfDelta::getDeltaR(const TrigInDetTrack* aTrack) const{

  if(!roiDescriptor) return -99.0;
  
  float eta0 = roiDescriptor->eta();
  float phi0 = roiDescriptor->phi();
 
  float eta = aTrack->param()->eta();
  float phi = aTrack->param()->phi0();
  float dPhi = phi-phi0;
  if(dPhi<-M_PI) dPhi+=2.0*M_PI;
  if(dPhi>M_PI) dPhi-=2.0*M_PI;
  /// Use eta,phi from ROIDescriptor. Use not propagated track state
  double deltaR = sqrt((eta - eta0)*(eta - eta0) +
		       dPhi*dPhi);

  return deltaR;
}

double T2TauFinalMonOfDelta::getDeltaEta(const TrigInDetTrack* aTrack) const{

  if(!roiDescriptor) return -99.0;

  float eta0 = roiDescriptor->eta();
  float eta = aTrack->param()->eta();

  return eta - eta0;
}

double T2TauFinalMonOfDelta::getDeltaPhi(const TrigInDetTrack* aTrack) const{

  if(!roiDescriptor) return -99.0;

  float phi0 = roiDescriptor->phi();
  float phi = aTrack->param()->phi0();
  float dPhi = phi-phi0;
  if(dPhi<-M_PI) dPhi+=2.0*M_PI;
  if(dPhi>M_PI) dPhi-=2.0*M_PI;
  return dPhi;
}
//////////////////////////////////////////////////////////


