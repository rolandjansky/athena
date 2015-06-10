/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigL2PattRecoStrategyF
// -------------------------------
// ATLAS Collaboration
//
// 16.01.2011 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: Dmitry.Emeliyanov@cern.ch
//
// implements a track-reading strategy that retrieves and  
// selects FTK tracks
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <list>
//#include "GaudiKernel/ToolFactory.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrigInDetRecoTools/ITrigL2PattRecoStrategy.h"

#include "IRegionSelector/IRoiDescriptor.h"

#include "SiSpacePoint/SCT_SpacePoint.h"
#include "SiSpacePoint/PixelSpacePoint.h"
#include "InDetIdentifier/PixelID.h" 
#include "InDetIdentifier/SCT_ID.h" 
#include "InDetPrepRawData/SiClusterContainer.h" 
#include "InDetPrepRawData/PixelClusterCollection.h" 
#include "InDetPrepRawData/SCT_ClusterCollection.h" 
//#include "DataModel/tools/IdentContIndex.h"
 
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "IRegionSelector/IRegSelSvc.h"

#include "TrigInDetRecoTools/TrigL2PattRecoStrategyF.h"
//#include "TrigInDetRecoTools/TrigL2DataConversionUtils.h"

TrigL2PattRecoStrategyF::TrigL2PattRecoStrategyF(const std::string& t, 
						 const std::string& n,
						 const IInterface*  p ): 
  AthAlgTool(t,n,p),

  m_regionSelector("RegSelSvc", n),
  m_ftkTracksName ("FTK_LVL2_Tracks"), 
  m_ptCut (1.0),
  m_chi2Cut(1000.0)
{
  declareInterface< ITrigL2PattRecoStrategy >( this ); 

  declareProperty("FTK_Tracks_Location"         ,m_ftkTracksName   );
  declareProperty("pTCut"       ,m_ptCut  );
  declareProperty("Chi2Cut"         ,m_chi2Cut    );
}

StatusCode TrigL2PattRecoStrategyF::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  sc = m_regionSelector.retrieve();
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL<< "Unable to retrieve RegionSelector tool "<< m_regionSelector.type() << endreq;
    return sc;
  }

  // Get SCT & pixel Identifier helpers

  if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) { 
     athenaLog << MSG::FATAL << "Could not get Pixel ID helper" << endreq;
     return StatusCode::FAILURE;
  }  

  if (detStore()->retrieve(m_sctId, "SCT_ID").isFailure()) {
    athenaLog << MSG::FATAL << "Could not get SCT ID helper" << endreq;
    return StatusCode::FAILURE;  
  }

  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    athenaLog << MSG::INFO<< "Unable to locate Service TrigTimerSvc " << endreq;
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("StrF_PattReco");

  }

  athenaLog << MSG::INFO << "TrigL2PattRecoStrategyF initialized "<< endreq;
  return sc;
}

StatusCode TrigL2PattRecoStrategyF::finalize() {

  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

TrigL2PattRecoStrategyF::~TrigL2PattRecoStrategyF() {

}

HLT::ErrorCode TrigL2PattRecoStrategyF::findTracks(const std::vector<const TrigSiSpacePoint*>& /*vsp*/,
						   const IRoiDescriptor& roi,
						   TrigInDetTrackCollection*  trackColl) {

  m_pattRecoTiming=0.0;
  HLT::ErrorCode rc=HLT::OK;

  m_zVertices.clear();

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::DEBUG) 
    athenaLog<< MSG::DEBUG<<"TrigL2PattRecoStrategyF called in RoI-based mode "<<endreq;

  std::vector<IdentifierHash>  listOfFTKIds;



  double roiPhiMin = roi.phiMinus();
  double roiPhiMax = roi.phiPlus();
  double roiEtaMin = roi.etaMinus();
  double roiEtaMax = roi.etaPlus();

  // m_regionSelector->DetHashIDList(FTK, listOfFTKIds );


  StatusCode sc=getFTK_Tracks(listOfFTKIds);
  
  if(sc.isFailure()) {
    if (outputLevel <= MSG::DEBUG) 
      athenaLog<< MSG::DEBUG<<"Failed to retrieve FTK Tracks "<<endreq;
    return rc;// job can continue but the trackColl will be empty
  }

  if(m_timers) m_timer[0]->start();

  for(std::vector<const TrigInDetTrackCollection*>::iterator collIt = m_selectedFtkTracks.begin(); 
      collIt != m_selectedFtkTracks.end(); ++collIt) {
    const TrigInDetTrackCollection* pColl = (*collIt);
    if(pColl==NULL) continue;

    if (outputLevel <= MSG::DEBUG) 
      athenaLog<< MSG::DEBUG<<"FTK collection size="<<pColl->size()<<endreq;

    for(TrigInDetTrackCollection::const_iterator trIt = pColl->begin(); trIt!=pColl->end();++trIt) {
      const TrigInDetTrack* pTrack = (*trIt);
      if(pTrack==NULL) continue;

      if(pTrack->chi2()> m_chi2Cut) continue;
      const TrigInDetTrackFitPar* pPar = pTrack->param();

      if(pPar==NULL) continue;
      //      if(fabs(pPar->pT())<m_ptCut) continue;

      if((pPar->eta()<roiEtaMin) || (pPar->eta()>roiEtaMax)) continue;
      
      if(roiPhiMin<roiPhiMax) { // pi-boundary check
	if((pPar->phi0()<roiPhiMin) || (pPar->phi0() > roiPhiMax)) continue;
      }
      else {
	if((pPar->phi0()<roiPhiMax) && (pPar->phi0() > roiPhiMin)) continue;
      }

      TrigInDetTrack* p = createOutputTrack(pTrack);
      if(p==NULL) continue;

      trackColl->push_back(p);
      /*
      std::cout<<"Selected FTK track: d0="<<pPar->a0()<<" z0="<<pPar->z0()<<" phi0="<<pPar->phi0()<<" eta="<<pPar->eta()
	       <<" pT="<<pPar->pT()<<std::endl;
      */
    }
  }

  /*
  for (size_t i=0; i<foundTracks->size() ; i++){
    TrigInDetTrack* p = convertOutputTracks(foundTracks->at(i));
    if(p!=NULL) {
      trackColl->push_back(p);
    }
  }
  */

  if(m_timers) {
    m_timer[0]->stop();
    m_pattRecoTiming=m_timer[0]->elapsed();
  }

  return rc;
}


HLT::ErrorCode TrigL2PattRecoStrategyF::findTracks(const std::vector<const TrigSiSpacePoint*>& //vsp
						   ,
						   TrigInDetTrackCollection* trackColl) {

  m_pattRecoTiming=0.0;
  m_zVertices.clear();

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  HLT::ErrorCode rc=HLT::OK;
  if (outputLevel <= MSG::DEBUG) 
    athenaLog<< MSG::DEBUG<<"TrigL2PattRecoStrategyF called in full-scan mode "<<endreq;

  StatusCode sc=getFTK_Tracks();

  if(sc.isFailure()) {
    if (outputLevel <= MSG::DEBUG) 
      athenaLog<< MSG::DEBUG<<"Failed to retrieve FTK Tracks "<<endreq;
    return rc;// job can continue but the trackColl will be empty
  }

  if(m_timers) m_timer[0]->start();

  for(std::vector<const TrigInDetTrackCollection*>::iterator collIt = m_selectedFtkTracks.begin(); 
      collIt != m_selectedFtkTracks.end(); ++collIt) {
    const TrigInDetTrackCollection* pColl = (*collIt);
    if(pColl==NULL) continue;

    if (outputLevel <= MSG::DEBUG) 
      athenaLog<< MSG::DEBUG<<"FTK collection size="<<pColl->size()<<endreq;

    for(TrigInDetTrackCollection::const_iterator trIt = pColl->begin(); trIt!=pColl->end();++trIt) {
      const TrigInDetTrack* pTrack = (*trIt);
      if(pTrack==NULL) continue;

      if(pTrack->chi2()> m_chi2Cut) continue;
      const TrigInDetTrackFitPar* pPar = pTrack->param();

      if(pPar==NULL) continue;
      // if(fabs(pPar->pT())<m_ptCut) continue;

      TrigInDetTrack* p = createOutputTrack(pTrack);
      if(p==NULL) continue;

      trackColl->push_back(p);
      /*
      std::cout<<"Selected FTK track: d0="<<pPar->a0()<<" z0="<<pPar->z0()<<" phi0="<<pPar->phi0()<<" eta="<<pPar->eta()
	       <<" pT="<<pPar->pT()<<std::endl;
      */
    }
  }

  if(m_timers) {
    m_timer[0]->stop();
    m_pattRecoTiming=m_timer[0]->elapsed();
  }

  return rc;
}


StatusCode TrigL2PattRecoStrategyF::getFTK_Tracks(const std::vector<IdentifierHash>& //ftkIds
						  ) {

  StatusCode sc = retrieveIDC();

  if(sc.isFailure()) return sc;

  // TO-DO select only collections with given IDs ...

  m_selectedFtkTracks.clear();
  m_selectedFtkTracks.push_back(m_ftkTracksContainer);
  
  return sc;
}

StatusCode TrigL2PattRecoStrategyF::getFTK_Tracks() {
  
  StatusCode sc = retrieveIDC();
  if(sc.isFailure()) return sc;

  m_selectedFtkTracks.clear();
  m_selectedFtkTracks.push_back(m_ftkTracksContainer);// adding all tracks

  return sc;
}

StatusCode TrigL2PattRecoStrategyF::retrieveIDC() {

  MsgStream athenaLog(msgSvc(), name());  
  StatusCode sc(StatusCode::SUCCESS);

  m_ftkTracksContainer = NULL;

  int outputLevel = msgSvc()->outputLevel( name() );

  if( !evtStore()->transientContains<TrigInDetTrackCollection>(m_ftkTracksName)){  
    if (outputLevel <= MSG::DEBUG) 
      athenaLog<< MSG::DEBUG<< " FTK tracks  " << m_ftkTracksName << " not found in StoreGate !" << endreq;
    return sc;
  }  
  else { 
    sc = evtStore()->retrieve(m_ftkTracksContainer, m_ftkTracksName); 
    if (sc.isFailure()) { 
      athenaLog << MSG::ERROR << "Failed to get FTK Tracks Container" << endreq; 
      return sc; 
    }
  } 
  return sc;
}

TrigInDetTrack* TrigL2PattRecoStrategyF::createOutputTrack(const TrigInDetTrack* oldTrack) {

  TrigInDetTrack* pTrack = NULL;
  const TrigInDetTrackFitPar* pPar = oldTrack->param();

  double D0 = pPar->a0(); 
  double Z0 = pPar->z0(); 
  double Phi0 = pPar->phi0(); 
  double Eta = pPar->eta(); 
  if(Phi0>M_PI) Phi0-=2*M_PI; 
  if(Phi0<-M_PI) Phi0+=2*M_PI;
  double Pt=pPar->pT(); 

  const TrigInDetTrackFitPar* tidtfp = new TrigInDetTrackFitPar(D0,Phi0,Z0,Eta,Pt,NULL); 
  std::vector<const TrigSiSpacePoint*>* pvsp = new std::vector<const TrigSiSpacePoint*>;
  pvsp->clear();
  pTrack = new TrigInDetTrack(pvsp,tidtfp);
  pTrack->algorithmId(TrigInDetTrack::STRATEGY_F_ID);
  pTrack->chi2(oldTrack->chi2());

  return pTrack;
}
