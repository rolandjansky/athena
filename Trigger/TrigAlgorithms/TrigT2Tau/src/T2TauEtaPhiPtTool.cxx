/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2TauEtaPhiPtTool.cxx
// PACKAGE:  Trigger/TrigAlgoritms/TrigT2Tau
// 
// AUTHOR:   O.Igonkina
// 
// ********************************************************************

#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigParticle/TrigTau.h"

#include "TrigT2Tau/T2TauEtaPhiPtTool.h"
#include "FourMom/P4PxPyPzE.h"

T2TauEtaPhiPtTool::T2TauEtaPhiPtTool(const std::string & type, const std::string & name, 
                   const IInterface* parent): AthAlgTool(type, name, parent){
declareInterface<IAlgToolTau>(this);
}
 
T2TauEtaPhiPtTool::~T2TauEtaPhiPtTool(){
}
 
StatusCode T2TauEtaPhiPtTool::initialize(){
  ATH_MSG_DEBUG( "in initialize()" );
  return StatusCode::SUCCESS;
}
 
StatusCode T2TauEtaPhiPtTool::finalize(){
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}

StatusCode T2TauEtaPhiPtTool::execute(const TrigTauCluster *pTrigTauCluster,
				    const TrigTauTracksInfo* pTracksInfo,
				    TrigTau& pTrigTau){
  //static const double pimassSqr = 19479.7849 ; //139.57**2

  float px = 0;
  float py = 0;
  float pz = 0;
  float e = 0;

  float eta = -50;
  float phi = 0;

  if( pTracksInfo != 0 && pTracksInfo->nCoreTracks() > 0 ) {
    px += pTracksInfo->px();
    py += pTracksInfo->py();
    pz += pTracksInfo->pz();
    e  += pTracksInfo->e();
    if( e < 0 ) {
      e = 1 ; // if we keep it negative then values will be reverted afterwards.
      ATH_MSG_WARNING( " found TrigTauTracksInfo with negative energy " << e << " set it to 1 MeV");
    }

    ATH_MSG_DEBUG( " adding tracks with  eta=" <<  pTracksInfo->eta()
                   << ", phi="<< pTracksInfo->phi() 
                   << ", pt= " << pTracksInfo->pt() 
                   << ", m="<< pTracksInfo->m() );
    eta =  pTracksInfo->eta();
    phi =  pTracksInfo->phi();
  }

  double eFlow(0), etFlow(0);
  if( pTrigTauCluster != 0 && std::abs(pTrigTauCluster->eta())<10 ) // combained info
    {

      eFlow = pTrigTauCluster->EMenergyWide(0) + pTrigTauCluster->EMenergyWide(1) 
	+ pTrigTauCluster->EMenergyWide(2);

      if( eFlow < 0 ) {
	eFlow = 1 ; // if we keep it negative then values will be reverted afterwards.
        ATH_MSG_DEBUG( "EM[0,2] is negative " << eFlow << " set it to 1 MeV");
      }

      etFlow = eFlow/ cosh(pTrigTauCluster->eta() ) ;

      px += etFlow*cos( pTrigTauCluster->phi() );
      py += etFlow*sin( pTrigTauCluster->phi() );
      pz += etFlow*sinh(pTrigTauCluster->eta() );

      e += eFlow;
      ATH_MSG_DEBUG( " Cluster position eta=" <<pTrigTauCluster->eta()
                     << ", phi="<< pTrigTauCluster->phi()
                     << " Et="<<pTrigTauCluster->et() << " EM[0,2] Et="<< etFlow );

      //if et is 0 in both cases, then tracks must be missing, and perhaps calo calibration failed.
      // so take cluster position in this case, is only its can still make some sense..
      eta = pTrigTauCluster->eta();
      phi = pTrigTauCluster->phi();

    }
  if( fabs(e) > 1.e-5) // not zero
    {
      if(fabs(pz) >1.e-5 ){
	pTrigTau.set4Mom(P4PxPyPzE(px,py,pz,e ));
      }else{

	double pt2 = px*px+py*py;	
	double mass = 0;
	double e2 = e*e;
	if( e2>pt2) mass=sqrt(e2-pt2);
	if( pt2>0.) pTrigTau.setPt(sqrt(pt2));
	pTrigTau.setM(mass);
	pTrigTau.setEta(0.); 
	if( fabs(px)<1.e-5&&fabs(py)<1.e-5)
	  pTrigTau.setPhi(0);
	else
	  pTrigTau.setPhi( atan2(py,px));
      }
    }else{

    pTrigTau.setPt(1); // never set 0, it will revert tracks
    pTrigTau.setM(1776.99);
    pTrigTau.setEta(eta);
    pTrigTau.setPhi(phi);

    if( fabs(eta+100.)<1.e-10 && fabs(phi)<1.e-10) {
      ATH_MSG_ERROR( " found a L2 tau RoI with no tracks and with no cluster - what is going on??" );
      return StatusCode::FAILURE;
    }

  }

  float etOverPt = -111;
  if(  pTrigTauCluster != 0 && std::abs(pTrigTauCluster->eta())<10 )
    {

      float TotEt = (pTrigTauCluster->EMenergy() + pTrigTauCluster->HADenergy())/cosh(pTrigTauCluster->eta());
      
      if( pTracksInfo != 0 && pTracksInfo->leadingTrackPt() >0.1 ) 
	etOverPt = TotEt / pTracksInfo->leadingTrackPt() ;      
    }


  pTrigTau.setEtOverPtLeadTrk(etOverPt);




  ATH_MSG_DEBUG( "pt (trks+EM[0-2])=" << pTrigTau.pt() 
                 << " eta/phi (trks+clus) = " << pTrigTau.eta() << "/" << pTrigTau.phi() );
  ATH_MSG_DEBUG( "EtOverPtLeadTrk =" << pTrigTau.etOverPtLeadTrk() );
  if(  pTrigTauCluster == 0 && pTracksInfo==0) 
    ATH_MSG_WARNING( " No track neither cluster information is present" );

  return StatusCode::SUCCESS; 
}

