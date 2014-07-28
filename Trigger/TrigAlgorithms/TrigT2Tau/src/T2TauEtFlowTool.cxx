/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2TauEnergyTool.cxx
// PACKAGE:  Trigger/TrigAlgoritms/TrigT2Tau
// 
// AUTHOR:   O.Igonkina
// 
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
 
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigParticle/TrigTau.h"

#include "TrigT2Tau/T2TauEtFlowTool.h"
#include "FourMom/P4PxPyPzE.h"

T2TauEtFlowTool::T2TauEtFlowTool(const std::string & type, const std::string & name, 
                   const IInterface* parent): AlgTool(type, name, parent){
declareInterface<IAlgToolTau>(this);
}
 
T2TauEtFlowTool::~T2TauEtFlowTool(){
}
 
StatusCode T2TauEtFlowTool::initialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in initialize()" << endreq;
  return StatusCode::SUCCESS;
}
 
StatusCode T2TauEtFlowTool::finalize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode T2TauEtFlowTool::execute(const TrigTauCluster *pTrigTauCluster,
				    const TrigTauTracksInfo* pTracksInfo,
				    TrigTau& pTrigTau){
  MsgStream log(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel(name());
  
  //static const double pimassSqr = 19479.7849 ; //139.57**2

  float px = 0;
  float py = 0;
  float pz = 0;
  float e = 0;

  if( pTracksInfo != 0 ) {
    px += pTracksInfo->threeFastestTracks().px();
    py += pTracksInfo->threeFastestTracks().py();
    pz += pTracksInfo->threeFastestTracks().pz();
    e  += pTracksInfo->threeFastestTracks().e();
    if( e < 0 ) {
      e = 1 ; // if we keep it negative then values will be reverted afterwards.
      log << MSG::WARNING << " found TrigTauTracksInfo with negative energy " << e << " set it to 1 MeV"<< endreq;
    }

    if (outputLevel <= MSG::DEBUG) {
      log << MSG::DEBUG << " adding 3(2) fastest core tracks with  eta=" <<  pTracksInfo->threeFastestTracks().eta()
	  << ", phi="<< pTracksInfo->threeFastestTracks().phi() 
	  << ", pt= " << pTracksInfo->threeFastestTracks().pt() 
	  << ", m="<< pTracksInfo->threeFastestTracks().m() << endreq;
    }
  }

  if( pTrigTauCluster != 0 && std::abs(pTrigTauCluster->eta())<10  ) // combained info
    {

     double eFlow = pTrigTauCluster->EMenergyWide(0) + pTrigTauCluster->EMenergyWide(1) 
	+ pTrigTauCluster->EMenergyWide(2);

      if( eFlow < 0 ) {
	eFlow = 1 ; // if we keep it negative then values will be reverted afterwards.
	if (outputLevel <= MSG::DEBUG) 
	  log << MSG::DEBUG << "EM[0,2] is negative " << eFlow << " set it to 1 MeV"<< endreq;
      }
      double etFlow = eFlow/ cosh(pTrigTauCluster->eta() ) ;


      px += etFlow*cos( pTrigTauCluster->phi() );
      py += etFlow*sin( pTrigTauCluster->phi() );
      pz += etFlow*sinh(pTrigTauCluster->eta() );

      e += eFlow;
      if (outputLevel <= MSG::DEBUG) 	
	log << MSG::DEBUG << " Cluster position eta=" <<pTrigTauCluster->eta()
	    << ", phi="<< pTrigTauCluster->phi()
	    << " Et="<<pTrigTauCluster->et() << " EtFlow="<< eFlow << endreq;

    }

  P4PxPyPzE simpleFlow(px,py,pz,e );
  
  pTrigTau.setSimpleEtFlow(simpleFlow.pt());

  if (outputLevel <= MSG::DEBUG) 
    log << MSG::DEBUG << "pt (trks+EM[0-2])=" << pTrigTau.pt() 
	<< " eta/phi (trks+clus) = " << pTrigTau.eta() << "/" << pTrigTau.phi() << endreq;

  if(  pTrigTauCluster == 0 && pTracksInfo==0 && outputLevel <= MSG::DEBUG) 
    log << MSG::WARNING << " No track neither cluster information is present" << endreq;
    

  return true;
}

