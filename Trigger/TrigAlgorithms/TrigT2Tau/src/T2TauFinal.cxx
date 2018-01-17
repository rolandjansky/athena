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

  declareMonitoredCustomVariable("DeltaRTkClust", new T2TauFinalMonOfDelta(*&m_myTrackColl,*&m_roiDescriptor,1) );
  declareMonitoredCustomVariable("DeltaEtaTkClust", new T2TauFinalMonOfDelta(*&m_myTrackColl,*&m_roiDescriptor,2) );
  declareMonitoredCustomVariable("DeltaPhiTkClust", new T2TauFinalMonOfDelta(*&m_myTrackColl,*&m_roiDescriptor,3) );
  
  declareProperty("updateRoiDescriptor", m_updateRoiDescriptor, "option to update RoiDescriptor after execution");

  m_pTrackColl = 0;
  m_myTrackColl = 0;
  m_roiDescriptor = 0;
 
}

T2TauFinal::~T2TauFinal(){
}

/*-------------------------------------------*/
HLT::ErrorCode T2TauFinal::hltInitialize()
/*-------------------------------------------*/
{
  ATH_MSG_DEBUG( "Initialization set of tools"  );
  
  ToolHandleArray<IAlgToolTau>::iterator it = m_algTools.begin();
  
  for (; it != m_algTools.end();  ++it) {
    StatusCode sc = it->retrieve();
    if( sc.isFailure() ) {
      ATH_MSG_ERROR( "Unable to initialize tool " << *it  );
      return HLT::BAD_ALGO_CONFIG ;
    }
    ATH_MSG_DEBUG( "REGTEST: Created " << *it << " AlgTool"  );
    
  }
  
  
  if( m_updateRoiDescriptor )
    ATH_MSG_DEBUG( "REGTEST: TrigRoiDescriptor will be updated "  );
  else
    ATH_MSG_DEBUG( "REGTEST: TrigRoiDescriptor will NOT be updated "  );
  
  ATH_MSG_DEBUG( "Initialization completed successfully"  );
  
  return HLT::OK;
}

/*-------------------------------------------*/
HLT::ErrorCode T2TauFinal::hltFinalize()
/*-------------------------------------------*/
{
  
  ATH_MSG_DEBUG( "in finalize()"  );
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
  
  ATH_MSG_DEBUG( "in execute()"  );
  
  //--------- 1. get RoI descriptor
  //const TrigRoiDescriptor* m_roiDescriptor = 0;
  m_roiDescriptor = 0;
  
  HLT::ErrorCode hltStatus = getFeature(inputTE, m_roiDescriptor);
  
  if ( hltStatus == HLT::OK && m_roiDescriptor !=0 ) {
    ATH_MSG_DEBUG( "REGTEST: RoI id " 
                   << m_roiDescriptor->roiId()
                   << " word " << m_roiDescriptor->roiWord()
                   << " LVL1 id " << m_roiDescriptor->l1Id()
                   << " located at  eta=" << m_roiDescriptor->eta() 
                   << ", phi=" <<  m_roiDescriptor->phi()  );
  } else {
    ATH_MSG_ERROR( " No RoI for this Trigger Element! "  );
    return hltStatus;
  }

  //-------- 2.  get cluster
  const TrigTauCluster* pClus(0);  
  hltStatus = getFeature(inputTE, pClus ); 

  if(hltStatus== HLT::OK) {
    ATH_MSG_DEBUG( "Got TrigTauCluster"  );
  }else{
    ATH_MSG_DEBUG( "REGTEST: No TrigTauCluster is found in RoI! "  );
  }
    
    
  //-------- 3. Get tracksInfo
    
  const TrigTauTracksInfo* pTracksInfo(0);
  hltStatus = getFeature(inputTE, pTracksInfo ); 

  if(hltStatus== HLT::OK) {
    ATH_MSG_DEBUG( "Got  TrigTauTracksInfo"  );
  }else{
    ATH_MSG_DEBUG( "REGTEST: No TrigTauTracksInfo is found in RoI! "  );
  }
    
  //-------- 3. Get vector of tracks 
    
  //const TrigInDetTrackCollection* m_pTrackColl(0);
  m_pTrackColl = 0;
  if( pTracksInfo != 0 ){
    hltStatus = getFeature(inputTE, m_pTrackColl ); 
    if(hltStatus== HLT::OK) {
      ATH_MSG_DEBUG( "Got TrigInDetTrackCollection as well "  );
    }else{
      ATH_MSG_DEBUG( "REGTEST: No TrigInDetTrackCollection is found in RoI but TrigTauTracksInfo exists! "  );
    }    
  }

  m_myTrackColl = m_pTrackColl; //Necessary for downcasting object for monitoring.

  //OI: now, let us check if pointer to track is set in the tracksInfo. It is needed for matching tool
  // this is actually really ugly, as collection can be from SiTrack, while Info is from IDSCAN
  // however, if we re-run HYPOs, we would have these set to 0, and therefore matching will not work.. 
  // Is there a nicer way to proceed?
  if( m_pTrackColl != 0 && pTracksInfo!=0){
    if( pTracksInfo->trackCollection() == 0)
      {
	TrigTauTracksInfo* ptmp = const_cast< TrigTauTracksInfo*>(pTracksInfo);
	ptmp->setTrackCollection(m_pTrackColl);
      }
  }

  if(hltStatus== HLT::OK) {
    ATH_MSG_DEBUG( "Got TrigInDetTrackCollection "  );
  }else{
    ATH_MSG_DEBUG( "REGTEST: No TrigInDetTrackCollection is found in RoI! "  );
  }

  //-------- 4. Make new object
  if( pClus== 0 && pTracksInfo == 0 )
    {
      ATH_MSG_DEBUG( " Nor TrigTauCluster neither  TrigInDetTrackCollection is found in RoI! Wrong configuration!"  );
      return HLT::OK;
    }
  
  TrigTau* pTrigTau = new TrigTau();
  pTrigTau->setRoiId((int)m_roiDescriptor->roiWord());
  pTrigTau->setTauCluster(pClus);
  pTrigTau->setTrackCollection(m_pTrackColl);
  pTrigTau->setTracksInfo(pTracksInfo);
  pTrigTau->setEta(m_roiDescriptor->eta()); // to be changed by T2TauEtaPhiTool
  pTrigTau->setPhi(m_roiDescriptor->phi());
  
  
  ToolHandleArray<IAlgToolTau>::iterator it = m_algTools.begin();
  for (; it < m_algTools.end(); it++)  {
    if ((*it)->execute(pClus,pTracksInfo, *pTrigTau).isFailure() ) {
      ATH_MSG_WARNING( "T2TauFinal AlgTool "<<*it<< " returned Failure"  );
      delete pTrigTau;
      pTrigTau=0;
      return HLT::TOOL_FAILURE;
    }
  }
 
  
  hltStatus = attachFeature(outputTE, pTrigTau );
  if (hltStatus != HLT::OK ){
    ATH_MSG_ERROR( "Write of TrigTau into outputTE failed"  );
    pTrigTau=0;
    return hltStatus;
  }
  if(  m_updateRoiDescriptor ) {

    /// NB: What is the RoiDescriptor for ?? Need to create with the correct size 
    ///     If this is for use by this algorithm later, how big should it be ?
    ///     If it is for the IDTrigger, need to know what the resolution of the 
    ///     found objects actually is. 
    ///     Temporarily create an INCORRECT 0 size RoiDescriptor

    ATH_MSG_WARNING( "Creating an RoiDescriptor with NO SIZE !!!! this is not allowed"  );
    
    TrigRoiDescriptor* newRoiDescriptor = new TrigRoiDescriptor( m_roiDescriptor->roiWord(), 
								 m_roiDescriptor->l1Id(), 
								 m_roiDescriptor->roiId(),
								 pTrigTau->eta(), pTrigTau->eta(), pTrigTau->eta(), 
								 pTrigTau->phi(), pTrigTau->phi(), pTrigTau->phi() );
    
    hltStatus = attachFeature(outputTE, newRoiDescriptor, "T2TauFinal"); 
    if (hltStatus != HLT::OK ){
      ATH_MSG_ERROR( "Can not attach new RoI descriptor"  );
      delete newRoiDescriptor; 
      return hltStatus;
    }
    ATH_MSG_DEBUG( "REGTEST: Recorded an RoiDescriptor " << newRoiDescriptor->roiWord() <<
                   " eta=" << newRoiDescriptor->eta() <<
                   ", phi=" <<  newRoiDescriptor->phi()  );
  }
  ATH_MSG_DEBUG( "REGTEST: TrigTau::pt,eta,phi = " << pTrigTau->pt() <<", " <<
                 pTrigTau->eta() <<", " << pTrigTau->phi()  );
  ATH_MSG_DEBUG( "REGTEST: TrigTau::m_nMatchedTracks; = " << pTrigTau->nMatchedTracks()  );
  ATH_MSG_DEBUG( "REGTEST: TrigTau::simpleEtFlow = " << pTrigTau->simpleEtFlow()  );
   
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
  ATH_MSG_DEBUG( "REGTEST: always accept RoI "  );
    
  return HLT::OK;
}

//////////////////////////////////////////////////////////
double T2TauFinalMonOfDelta::getDeltaR(const TrigInDetTrack* aTrack) const{

  if(!m_roiDescriptor) return -99.0;
  
  float eta0 = m_roiDescriptor->eta();
  float phi0 = m_roiDescriptor->phi();
 
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

  if(!m_roiDescriptor) return -99.0;

  float eta0 = m_roiDescriptor->eta();
  float eta = aTrack->param()->eta();

  return eta - eta0;
}

double T2TauFinalMonOfDelta::getDeltaPhi(const TrigInDetTrack* aTrack) const{

  if(!m_roiDescriptor) return -99.0;

  float phi0 = m_roiDescriptor->phi();
  float phi = aTrack->param()->phi0();
  float dPhi = phi-phi0;
  if(dPhi<-M_PI) dPhi+=2.0*M_PI;
  if(dPhi>M_PI) dPhi-=2.0*M_PI;
  return dPhi;
}
//////////////////////////////////////////////////////////


