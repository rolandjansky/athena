/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2IDTau/T2IDTau.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigT2IDTau/T2IDTauIPHelper.h"

class ISvcLocator;

T2IDTau::T2IDTau(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator),
  m_dRCore(0.1),
  m_dRIso(0.3),
  m_ptMin(1500),
  m_ptSlowMax(7500),
  m_dZ0Max(-1.0),
  m_centerAroundLeadingTrack(true),
  m_secondStep(false),
  m_TrigTrackingAlgoLabel(""),
  m_updateRoiDescriptor(false)
{


  declareProperty("dRCore",                   m_dRCore                   = 0.1 );
  declareProperty("dRIso",                    m_dRIso                    = 0.3 );
  declareProperty("ptMin",                    m_ptMin                    = 1500.0); //in MeV
  declareProperty("ptSlowMax",                m_ptSlowMax                = 7500.0); //no idea...
  declareProperty("dZ0Max",                   m_dZ0Max                   = -1.0 );//disabled by default

  declareProperty("centerAroundLeadingTrack", m_centerAroundLeadingTrack = true );
  declareProperty("secondStep",               m_secondStep               = false);//2step disabled by default

  declareProperty("TrigTrackingAlgoLabel",    m_TrigTrackingAlgoLabel    = "");
  declareProperty("updateRoiDescriptor",      m_updateRoiDescriptor      = false, "option to update RoiDescriptor after execution");

  ///Monitored variables
  declareMonitoredVariable("NTrk",          m_nCoreTracks);
  declareMonitoredVariable("charge",        m_charge);
  declareMonitoredVariable("NTrkSlow",      m_nSlowTracks);
  declareMonitoredVariable("NTrkIso",       m_nIsoTracks);
  declareMonitoredVariable("SumPtCore",     m_sumPtCore);
  declareMonitoredVariable("SumPtIso",      m_sumPtIso);
  declareMonitoredVariable("sumPtRatio",    m_sumPtRatio);
  declareMonitoredVariable("PtMaxTrk",      m_maxPt);
  declareMonitoredVariable("TrkRadius",     m_trkRadius);
                                            
  declareMonitoredStdContainer("alldZ0",    m_mon_trk_alldZ0);
  declareMonitoredStdContainer("alloweddZ0",m_mon_trk_alloweddZ0);
}

T2IDTau::~T2IDTau(){
}

/*-------------------------------------------*/
HLT::ErrorCode T2IDTau::hltInitialize()
/*-------------------------------------------*/
{
  msg() << MSG::DEBUG << "REGTEST:  in initilize" << endreq;
  msg() << MSG::DEBUG << "REGTEST:  Core cone: dR < " <<  m_dRCore << endreq;
  msg() << MSG::DEBUG << "REGTEST:  Isolation annulus: "<< m_dRCore << " < dR < " <<  m_dRIso << endreq;
  msg() << MSG::DEBUG << "REGTEST:  Consider tracks with pt > " << m_ptMin << endreq;
  msg() << MSG::DEBUG << "REGTEST:  Slow tracks have pt < "<< m_ptSlowMax << endreq;

  if ( m_dZ0Max >= 0.0 ) msg() << MSG::DEBUG << "REGTEST:  track selection |DeltaZ0| wrt LdPtTrk <= "<< m_dZ0Max << endreq;
  else                   msg() << MSG::DEBUG << "REGTEST:  no track selection based on |DeltaZ0|" << endreq;

  if ( m_centerAroundLeadingTrack ) msg() << MSG::DEBUG << "REGTEST:  Isolation cones are centered around leading track direction" << endreq;
  else                              msg() << MSG::DEBUG << "REGTEST:  Isolation cones are centered ar RoI center " << endreq;

  if ( m_secondStep ) msg() << MSG::DEBUG << "REGTEST:  This algorithm is configured as second step, will find LdTrk from inputTE" << endreq; 
  else                msg() << MSG::DEBUG << "REGTEST:  This algorithm is configured as either single step or first step" << endreq;

  if ( m_TrigTrackingAlgoLabel != "" ) msg() << MSG::DEBUG << "REGTEST: take track Collection with label = " << m_TrigTrackingAlgoLabel << endreq;
  else                                 msg() << MSG::DEBUG << "REGTEST: take latest track Collection found in TE " << endreq;
  
  if ( m_updateRoiDescriptor ) msg() << MSG::DEBUG << "REGTEST: TrigRoiDescriptor will be updated " << endreq;
  else                         msg() << MSG::DEBUG << "REGTEST: TrigRoiDescriptor will NOT  be updated " << endreq;
  
  return HLT::OK;
}

/*-------------------------------------------*/
HLT::ErrorCode T2IDTau::hltFinalize()
/*-------------------------------------------*/
{

  msg() << MSG::DEBUG << "in finalize()" << endreq;
  return HLT::OK;
}

/*------------------------------------------------*/
HLT::ErrorCode T2IDTau::hltExecute(const HLT::TriggerElement* inputTE, //ccuenca: avoid compilation warning 
                                   HLT::TriggerElement* outputTE)
/*------------------------------------------------*/
{

  if (msgLvl() <= MSG::DEBUG){
    msg() << MSG::DEBUG << "in execute()" << endreq;
  }
  
  m_nCoreTracks = -1;
  m_nIsoTracks  = -1;
  m_nSlowTracks = -1;
  m_maxPt       = -1;
  m_sumPtRatio  = -1;
  m_charge      = -10;
  m_sumPtCore   = -99;
  m_sumPtIso    = -99;
  m_trkRadius   = -99.0;

  m_mon_trk_alldZ0.    clear();
  m_mon_trk_alloweddZ0.clear();




  //--------- 1. get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  
  HLT::ErrorCode hltStatus = getFeature(outputTE, roiDescriptor);
  
  if ( hltStatus == HLT::OK && roiDescriptor !=0 ) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: RoI id " 
				      << roiDescriptor->roiId()
				      << " word " << roiDescriptor->roiWord()
				      << " LVL1 id " << roiDescriptor->l1Id()
				      << " located at  eta=" << roiDescriptor->eta() 
				      << ", phi=" <<  roiDescriptor->phi() << endreq;
  } else {
    msg() <<  MSG::ERROR << " No RoI Descriptor for this Trigger Element! " << endreq;
    return hltStatus;
  }


  //-------- 2. Get vector of tracks from output
  const TrigInDetTrackCollection* tracks(0);

  hltStatus = getFeature(outputTE, tracks);
  
  if (msgLvl() <= MSG::DEBUG) {
    if(hltStatus== HLT::OK) msg() << MSG::DEBUG << "REGTEST: Got TrigInDetTrackCollection with label "<< m_TrigTrackingAlgoLabel << endreq;
    else                    msg() << MSG::DEBUG << "REGTEST: No TrigInDetTrackCollection is found in RoI! hltStatus = "<<hltStatus << endreq;
  }
  
  //-------- 3 Make new object
  if( tracks == 0 || tracks->size() == 0 ){
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "REGTEST: No  TrigInDetTrackCollection is found in RoI or it is empty! hltStatus =HLT::OK" << endreq;
    return HLT::OK;
  }


  TrigTauTracksInfo* pTrackInfo = new TrigTauTracksInfo();
  pTrackInfo->setRoiId((int)roiDescriptor->roiWord());
  pTrackInfo->setTrackCollection(tracks);
  pTrackInfo->setEta(roiDescriptor->eta()); // to be changed by T2TauEtaPhiTool
  pTrackInfo->setPhi(roiDescriptor->phi());
  
  //------- 4: Find leading track in outputTE (or inputTE for second step)
  float eta = roiDescriptor->eta();
  float phi = roiDescriptor->phi();

  const TrigInDetTrackCollection* firstTracks(0);
  if ( m_secondStep ) { 
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: Finding leading track in 2nd step using tracks from 1st step" << endreq;;
    hltStatus = getFeature(inputTE, firstTracks);

    if(hltStatus== HLT::OK) { 
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: Got TrigInDetTrackCollection from inputTE "<< endreq;
    } else {
      msg() << MSG::ERROR << "No TrigInDetTrackCollection is found in inputTE! RETURNING" << endreq;
      return hltStatus;
    }
  } else {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: Finding leading track in outputTE" << endreq;
    firstTracks = tracks;
  }
  
  TrigInDetTrackCollection::const_iterator p1= firstTracks->end();
  float pt1(0), z01(-999.0);
  
  for(TrigInDetTrackCollection::const_iterator  itrack = firstTracks->begin(); itrack != firstTracks->end(); ++itrack ) {
    if( (*itrack)->param() == 0 ) continue;
    double pt =  fabs((*itrack)->param()->pT());
    
    if(  pt <  m_ptMin ) continue;
    if( p1 == firstTracks->end() || pt1 < pt ) {
      p1  = itrack;     
      pt1 = pt;
      z01 = (*itrack)->param()->z0();
    }
  } // end loop over tracks
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: second step: " << m_secondStep << ", leading track pt = " << pt1 << endreq;
  

  if( m_centerAroundLeadingTrack && p1 != firstTracks->end() ) {
    eta = (*p1)->param()->eta();
    phi = (*p1)->param()->phi0();      
  }
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Center of core area is at eta="<< eta << ", phi=" << phi << endreq;

  

  //------- 6: identify tracks in core and isolation area, apply DZ0 selection if needed
  TrigInDetTrackCollection::const_iterator p2 = tracks->end();
  TrigInDetTrackCollection::const_iterator p3 = tracks->end();
  float pt2(0), pt3(0);

  int   nCoreTracks = 0;
  int   nSlowTracks = 0;
  int   nIsoTracks  = 0;
  float charge     = 0;
  float scalarPtSumCore = 0;
  float scalarPtSumIso = 0;

  float dRPtWeighted = 0.0;
  

  float px = 0;
  float py = 0;
  float pz = 0;
  float totEnergy = 0;

  static const double pimassSqr = 19479.7849 ; //139.57**2

  float dRCore2 =  m_dRCore*m_dRCore; // we take square here, in case the original parameters are changed after initialization
  float dRIso2  = m_dRIso*m_dRIso;
  
  for(TrigInDetTrackCollection::const_iterator  itrack = tracks->begin(); itrack != tracks->end(); ++itrack ) {

    if( (*itrack)->param() == 0 ) {
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Found TrigInDetTrack with undefined parameters" << endreq;
      continue;
    }
    

    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Found track pT = " << (*itrack)->param()->pT()
                                        << " eta = " << (*itrack)->param()->eta() 
                                        << " phi0 = " << (*itrack)->param()->phi0()
                                        << " AlgId = " << int((*itrack)->algorithmId()) << endreq;
    

    //min Pt selection
    double pt =  fabs((*itrack)->param()->pT());      
    if ( pt < m_ptMin ) continue;
    

    //deltaZ selection
    float dz0  = z01 - (*itrack)->param()->z0();
    float absdz0 = fabs(dz0);
    m_mon_trk_alldZ0.push_back(dz0);
    if ( m_dZ0Max >= 0.0  &&  absdz0 > m_dZ0Max ) continue;
    m_mon_trk_alloweddZ0.push_back(dz0);


    double dPhi = (*itrack)->param()->phi0() - phi; while (dPhi>=M_PI) dPhi -= 2.0*M_PI; while (dPhi < -1.0*M_PI) dPhi += 2.0*M_PI;
    double dEta = (*itrack)->param()->eta() - eta;
    double dR   = dPhi*dPhi + dEta*dEta;
    
    if( dR < dRCore2 ){
      if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG <<  "             added to core " << endreq;
      double tpx = pt*cos((*itrack)->param()->phi0());
      double tpy = pt*sin((*itrack)->param()->phi0());
      double tpz = pt/tan(2.*atan(exp(-(*itrack)->param()->eta())));
      double te  = sqrt(tpx*tpx + tpy*tpy+tpz*tpz + pimassSqr );
      
      px += tpx;
      py += tpy;
      pz += tpz;
      totEnergy += te;
      
      //notice that dR is actualle dR*dR
      dRPtWeighted    += pt * sqrt(dR);
      scalarPtSumCore += pt;
      nCoreTracks++;
      if( (*itrack)->param()->pT() > 0 ) charge += 1;
      else                               charge -= 1;
      if( pt < m_ptSlowMax ) nSlowTracks++;
      
      if( itrack != p1 ){
        if (p2 == tracks->end() || pt2 < pt ) {
          p3 = p2; if( p3 != tracks->end() ) pt3 = fabs((*p3)->param()->pT()); 
          p2 =  itrack;    pt2 = pt;
        }else if (p3 == tracks->end() || pt3 < pt ) {
          p3 =  itrack;  pt3 = pt;
        }
      } // found p2 and p3
      
      
    }else if( dR < dRIso2 ) {
      if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG <<  "             added to isolation area " << endreq;
      nIsoTracks++;
      scalarPtSumIso += pt;
    }
    
  } // end loop over tracks
  
  if (scalarPtSumCore > 0.01 ) m_trkRadius = dRPtWeighted / scalarPtSumCore;
  
  pTrackInfo->setNCoreTracks(nCoreTracks);
  pTrackInfo->setNSlowTracks(nSlowTracks);
  pTrackInfo->setNIsoTracks(nIsoTracks);
  pTrackInfo->setCharge(charge);
  if( p1 != firstTracks->end() ) pTrackInfo->setLeadingTrackPt(pt1);
  pTrackInfo->setScalarPtSumCore(scalarPtSumCore);
  pTrackInfo->setScalarPtSumIso(scalarPtSumIso);
  
  if( phi > M_PI )  phi -= 2*M_PI;
  if( phi < -M_PI ) phi += 2*M_PI;

  if( totEnergy > 1.e-10) // not zero
    pTrackInfo->set4Mom(P4PxPyPzE(px,py,pz, totEnergy));
  else{
    pTrackInfo->setPt(1); // never set 0, as it will revert 4vec in space
    pTrackInfo->setM(139.57);
    pTrackInfo->setEta(eta);
    pTrackInfo->setPhi(phi);

  }

  // ---- sum 3 fastest tracks
  px = 0; py = 0; pz = 0; totEnergy = 0;
  if( p1 != firstTracks->end() ) {      
    double tpx = pt1*cos((*p1)->param()->phi0());
    double tpy = pt1*sin((*p1)->param()->phi0());
    double tpz = pt1/tan(2.*atan(exp(-(*p1)->param()->eta())));
    px += tpx; py += tpy; pz += tpz;
    totEnergy += sqrt(tpx*tpx + tpy*tpy+tpz*tpz + pimassSqr );      
    }
  if( p2 != tracks->end() ) {      
    double tpx = pt2*cos((*p2)->param()->phi0());
    double tpy = pt2*sin((*p2)->param()->phi0());
    double tpz = pt2/tan(2.*atan(exp(-(*p2)->param()->eta())));
    px += tpx; py += tpy; pz += tpz;
    totEnergy += sqrt(tpx*tpx + tpy*tpy+tpz*tpz + pimassSqr );      
  }
  if( p3 != tracks->end() ) {      
    double tpx = pt3*cos((*p3)->param()->phi0());
    double tpy = pt3*sin((*p3)->param()->phi0());
    double tpz = pt3/tan(2.*atan(exp(-(*p3)->param()->eta())));
    px += tpx; py += tpy; pz += tpz;
    totEnergy += sqrt(tpx*tpx + tpy*tpy+tpz*tpz + pimassSqr );      
  }

  if( totEnergy > 1.e-10) pTrackInfo->set3fastestPxPyPzE(px,py,pz, totEnergy);
  else                    pTrackInfo->set3fastestPtEtaPhiM(1,eta,phi,139.57);
   
  
  hltStatus = attachFeature(outputTE, pTrackInfo );
  if (hltStatus != HLT::OK ){
    msg() << MSG::ERROR << "Write of TrigTauTracksInfo into outputTE failed" << endreq;
    return hltStatus;
  }
  
  if(  m_updateRoiDescriptor ) {
    TrigRoiDescriptor* newRoiDescriptor =
      new TrigRoiDescriptor(roiDescriptor->roiWord(),   
			    roiDescriptor->l1Id(), 
			    roiDescriptor->roiId(),
			    pTrackInfo->eta(),pTrackInfo->eta()-m_dRIso,pTrackInfo->eta()+m_dRIso,
			    pTrackInfo->phi(),pTrackInfo->phi()-m_dRIso,pTrackInfo->phi()+m_dRIso,
			    roiDescriptor->zed(), roiDescriptor->zedMinus(), roiDescriptor->zedPlus());
    
    hltStatus = attachFeature(outputTE, newRoiDescriptor,m_TrigTrackingAlgoLabel); // use same label as tracking
    if (hltStatus != HLT::OK ){
      msg() << MSG::ERROR << "Can not attach new RoI descriptor" << endreq;
      return hltStatus;
    }
    
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG  << "REGTEST: Recorded an RoiDescriptor " << roiDescriptor->roiWord() 
                                     << ", eta=" << newRoiDescriptor->eta() 
                                     << ", phi=" << newRoiDescriptor->phi() << endreq;
  }
  if(msgLvl() <= MSG::DEBUG){
    msg() << MSG::DEBUG << "REGTEST: TrigTauTrackInfo::pt,eta,phi = " << pTrackInfo->pt() <<", " << pTrackInfo->eta() <<", " << pTrackInfo->phi() << endreq;
    msg() << MSG::DEBUG << "REGTEST: leading Pt " << pTrackInfo->leadingTrackPt() << endreq;
    msg() << MSG::DEBUG << "REGTEST: 3 fastests tracks pt, eta, phi = " << pTrackInfo->threeFastestTracks().pt()
          <<", " << pTrackInfo->threeFastestTracks().eta() <<", " << pTrackInfo->threeFastestTracks().phi() << endreq;
    msg() << MSG::DEBUG << "REGTEST: NCore= " << pTrackInfo->nCoreTracks() << ", NSlow= " << pTrackInfo->nSlowTracks() << ", NIso= " << pTrackInfo->nIsoTracks() 
	  <<", Charge="<<pTrackInfo->charge() << endreq;
    msg() << MSG::DEBUG << "REGTEST: sumPtCore=" << pTrackInfo->scalarPtSumCore() << ", sumPtIso="<< pTrackInfo->scalarPtSumIso() 
          << ", ptBalance="<<pTrackInfo->ptBalance() << endreq;
  }
  

  // This is for Monitoring 
  // Get L1 RoiDescriptor
//   const TrigRoiDescriptor* roiL1Descriptor = 0;
//   HLT::ErrorCode tmpStatus = getFeature(inputTE, roiL1Descriptor,"initialRoI");

//   if(roiL1Descriptor){
//     m_EtaL1 = roiL1Descriptor->eta0();
//     m_PhiL1 = roiL1Descriptor->phi0();
//   }
  m_nCoreTracks = pTrackInfo->nCoreTracks();
  m_nIsoTracks  = pTrackInfo->nIsoTracks(); 
  m_charge      = static_cast<int> (charge);
  m_nSlowTracks = pTrackInfo->nSlowTracks();
  m_maxPt       = pTrackInfo->leadingTrackPt();
  m_sumPtIso    = pTrackInfo->scalarPtSumIso();
  m_sumPtCore   = pTrackInfo->scalarPtSumCore();

  if(m_nCoreTracks>0) m_sumPtRatio = pTrackInfo->scalarPtSumIso()/pTrackInfo->scalarPtSumCore();
  else                m_sumPtRatio = pTrackInfo->scalarPtSumIso()*(1./40000.); // assume 40 GeV is average candidate.
  
  // Reach this point successfully   
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: always accept RoI " << endreq;
  
  return HLT::OK;
}

HLT::ErrorCode T2IDTau::getPrmVtxCollection(const TrigVertexCollection*& pointerToL2PrmVtxCollections, const HLT::TriggerElement* outputTE) {

  std::vector<const TrigVertexCollection*> vectorOfL2PrmVtxCollections;

  HLT::ErrorCode status = getFeatures(outputTE, vectorOfL2PrmVtxCollections, "");
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get TrigVertexCollection from the trigger element" << endreq;
  } else if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Got " << vectorOfL2PrmVtxCollections.size() << " TrigVertexCollection" << endreq;

  std::vector<const TrigVertexCollection*>::iterator pPrmVtxColl    = vectorOfL2PrmVtxCollections.begin();
  std::vector<const TrigVertexCollection*>::iterator lastPrmVtxColl = vectorOfL2PrmVtxCollections.end();

  if (msgLvl() <= MSG::VERBOSE) {

    for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) {

      if ((*pPrmVtxColl)->size() != 0)
        msg() << MSG::VERBOSE << "TrigVertexCollection with label " << (*pPrmVtxColl)->front()->algorithmId() << endreq;
    }

    pPrmVtxColl = vectorOfL2PrmVtxCollections.begin();
  }

  for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) {

    if ((*pPrmVtxColl)->size() != 0) {

      // if (m_instance == "L2") {
    //  if (m_algo == 1) {
 //     if ((*pPrmVtxColl)->front()->algorithmId() == 3) {
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Selected collection with TrigT2HistoPrmVtx label " << (*pPrmVtxColl)->front()->algorithmId() << endreq;
        break;
   //   }
        //  } 
 //       else if (m_algo == 2) {
//        if ((*pPrmVtxColl)->front()->algorithmId() == 10) {
//          if (msgLvl() <= MSG::DEBUG)
//            msg() << MSG::DEBUG << "Selected collection with TrigT2HistoPrmVtx label " << (*pPrmVtxColl)->front()->algorithmId() << endreq;
//          break;
                }
 //        }
//        }
      /*      else if (m_instance == "EF") {
              if ((*pPrmVtxColl)->front()->algorithmId() == 11) {
              if (msgLvl() <= MSG::DEBUG)
              msg() << MSG::DEBUG << "Selected collection with TrigT2HistoPrmVtx label " << (*pPrmVtxColl)->front()->algorithmId() << endreq;
              break;
              }
              }
      */

    //}
  }

  if (pPrmVtxColl == lastPrmVtxColl) {
    pointerToL2PrmVtxCollections = 0;
    return HLT::ERROR;
  } else {
    pointerToL2PrmVtxCollections = *pPrmVtxColl;
    return HLT::OK;
  }
}

 //** ----------------------------------------------------------------------------------------------------------------- **//


// float T2IDTau::getIP1DErr(float param, float errParam) {
//
//   float sd0=errParam;
//
//   if (m_algo == 1) {
//     float eta = fabs(param);
//     float sd0 = 0.173 + 8.43e-3*std::pow(eta,4);
//
//     return sd0;
//   }
//
//   return sd0;
// }


 //** ----------------------------------------------------------------------------------------------------------------- **//


// float T2IDTau::getIP2DErr(float param, float errParam ) {
//
//   float sd0=errParam;
//
//   if (m_algo == 1) {
//     float pt = fabs(param);
//     float p0 = 0.023, p1=27, p2=1.48;
//     float sd014 = sqrt(p0*p0 + std::pow((float)(p1*(1./14000.)),p2));
//
//     sd0 = sqrt(p0*p0 + std::pow((float)(p1/pt),p2));
//
//     if (pt>14000)
//       sd0 = sd014;
//
//     return sd0*1.5;
//   }
//
//   return sd0;
// }
     
