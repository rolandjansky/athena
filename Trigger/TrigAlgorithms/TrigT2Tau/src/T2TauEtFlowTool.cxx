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

#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigParticle/TrigTau.h"

#include "TrigT2Tau/T2TauEtFlowTool.h"
#include "FourMom/P4PxPyPzE.h"

T2TauEtFlowTool::T2TauEtFlowTool(const std::string & type, const std::string & name, 
                   const IInterface* parent): AthAlgTool(type, name, parent){
declareInterface<IAlgToolTau>(this);
}
 
T2TauEtFlowTool::~T2TauEtFlowTool(){
}
 
StatusCode T2TauEtFlowTool::initialize(){
  ATH_MSG_DEBUG( "in initialize()" );
  return StatusCode::SUCCESS;
}
 
StatusCode T2TauEtFlowTool::finalize(){
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}

StatusCode T2TauEtFlowTool::execute(const TrigTauCluster *pTrigTauCluster,
				    const TrigTauTracksInfo* pTracksInfo,
				    TrigTau& pTrigTau){
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
      ATH_MSG_WARNING( " found TrigTauTracksInfo with negative energy " << e << " set it to 1 MeV");
    }

    ATH_MSG_DEBUG( " adding 3(2) fastest core tracks with  eta=" <<  pTracksInfo->threeFastestTracks().eta()
                   << ", phi="<< pTracksInfo->threeFastestTracks().phi() 
                   << ", pt= " << pTracksInfo->threeFastestTracks().pt() 
                   << ", m="<< pTracksInfo->threeFastestTracks().m() );
  }

  if( pTrigTauCluster != 0 && std::abs(pTrigTauCluster->eta())<10  ) // combained info
    {

     double eFlow = pTrigTauCluster->EMenergyWide(0) + pTrigTauCluster->EMenergyWide(1) 
	+ pTrigTauCluster->EMenergyWide(2);

      if( eFlow < 0 ) {
	eFlow = 1 ; // if we keep it negative then values will be reverted afterwards.
        ATH_MSG_DEBUG( "EM[0,2] is negative " << eFlow << " set it to 1 MeV");
      }
      double etFlow = eFlow/ cosh(pTrigTauCluster->eta() ) ;


      px += etFlow*cos( pTrigTauCluster->phi() );
      py += etFlow*sin( pTrigTauCluster->phi() );
      pz += etFlow*sinh(pTrigTauCluster->eta() );

      e += eFlow;
      ATH_MSG_DEBUG( " Cluster position eta=" <<pTrigTauCluster->eta()
                     << ", phi="<< pTrigTauCluster->phi()
                     << " Et="<<pTrigTauCluster->et() << " EtFlow="<< eFlow );

    }

  P4PxPyPzE simpleFlow(px,py,pz,e );
  
  pTrigTau.setSimpleEtFlow(simpleFlow.pt());

  ATH_MSG_DEBUG( "pt (trks+EM[0-2])=" << pTrigTau.pt() 
                 << " eta/phi (trks+clus) = " << pTrigTau.eta() << "/" << pTrigTau.phi() );

  if(  pTrigTauCluster == 0 && pTracksInfo==0 ) 
    ATH_MSG_WARNING( " No track neither cluster information is present" );
    

  return StatusCode::SUCCESS;
}

