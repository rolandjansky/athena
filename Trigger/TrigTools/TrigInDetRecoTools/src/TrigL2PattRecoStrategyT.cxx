/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigL2PattRecoStrategyT
// -------------------------------
// ATLAS Collaboration
//
// 15.03.2011 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: Dmitry.Emeliyanov@cern.ch
//
// implements TRT seeded backtracking strategy based on I.Gavrilenko's 
// offline reconstruction tools
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

#include "InDetRecToolInterfaces/ITRT_SeededTrackFinder.h" 

#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "InDetIdentifier/PixelID.h" 
#include "InDetIdentifier/SCT_ID.h" 
#include "InDetIdentifier/TRT_ID.h"

//#include "TrkSurfaces/PerigeeSurface.h" 
#include "TrkTrack/TrackCollection.h"
#include "SiSpacePoint/SCT_SpacePoint.h"
#include "SiSpacePoint/PixelSpacePoint.h"
#include "TrkSpacePoint/SpacePoint.h"  
//#include "TrkSpacePoint/SpacePointCollection.h"  
#include "TrkSpacePoint/SpacePointContainer.h"
//#include "InDetPrepRawData/SiClusterContainer.h" 
#include "InDetPrepRawData/PixelCluster.h" 
#include "InDetPrepRawData/SCT_Cluster.h" 
//#include "InDetPrepRawData/PixelClusterCollection.h" 
//#include "InDetPrepRawData/SCT_ClusterCollection.h"
//#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

//#include "DataModel/tools/IdentContIndex.h"

#include "TrkEventPrimitives/FitQuality.h" 
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSegment/SegmentCollection.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkParameters/TrackParameters.h" 

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "IRegionSelector/IRegSelSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 

#include "TrigInDetRecoTools/TrigL2PattRecoStrategyT.h"

TrigL2PattRecoStrategyT::TrigL2PattRecoStrategyT(const std::string& t, 
						 const std::string& n,
						 const IInterface*  p ): 
  AthAlgTool(t,n,p),

  m_nfreeCut(5),
  m_minTrtHits(10),
  m_trackmaker("InDet::TRT_SeededTrackFinderTool"),
  m_regionSelector("RegSelSvc", n),
  m_trtDataProvider("TrigTRT_DriftCircleProviderTool"), 
  m_robDataProvider("ROBDataProviderSvc",this->name()),
  m_trtFilteredContName ("TRT_FilteredDriftCircles"),
  m_sctOnlineSpacePointsName("TrigSCT_SpacePoints"), 
  m_pixelOnlineSpacePointsName("TrigPixelSpacePoints")
{
  declareInterface< ITrigL2SeededPattRecoStrategy >( this ); 

  declareProperty("TrackTool"         ,m_trackmaker    );
  declareProperty("FreeClustersCut"   ,m_nfreeCut      );
  declareProperty("MinTrtHitsCut"   ,  m_minTrtHits      );
  declareProperty( "TRT_FilteredContainerName",m_trtFilteredContName="TRT_FilteredDriftCircles");
  declareProperty( "TRT_ContainerName",m_trtContName="TRT_DriftCircle");
  declareProperty( "SegmentsMakerTool",m_segmentsmaker);

}

StatusCode TrigL2PattRecoStrategyT::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  // Get track-finding tool
  sc = m_trackmaker.retrieve();
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL << "Failed to retrieve tool " << m_trackmaker << endreq;
    return sc;
  } 
  else {
    athenaLog << MSG::INFO << "Retrieved tool " << m_trackmaker << endreq;
  }

  sc = m_regionSelector.retrieve();
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL<< "Unable to retrieve RegionSelector tool "<< m_regionSelector.type() << endreq;
    return sc;
  }

   sc = m_robDataProvider.retrieve();
  if(sc.isFailure()) {
    athenaLog << MSG::ERROR << "Unable to retrieve ROBDataProviderSvc" <<m_robDataProvider <<endreq;
    return StatusCode::FAILURE;
  }

  sc = m_trtDataProvider.retrieve();
  if(sc.isFailure()) {
    athenaLog << MSG::FATAL << "Unable to locate TRT data provider tool " <<m_trtDataProvider<< endreq;
    return sc;
  } 

  sc = m_segmentsmaker.retrieve();
  if( sc.isFailure() ) {
    athenaLog <<"Could not get TRT_TrackSegmentsMaker tool "<<m_segmentsmaker<<endreq;
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

  m_filteredDriftCircleContainer = new InDet::TRT_DriftCircleContainer(m_trtId->straw_layer_hash_max());
  m_filteredDriftCircleContainer->addRef();


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
    m_timer[0] = timerSvc->addItem("StrT_PattReco");
    m_timer[1] = timerSvc->addItem("Seed_NE");
    m_timer[2] = timerSvc->addItem("Seed_Prod");
    m_timer[3] = timerSvc->addItem("Seed_Ret");
    m_timer[3]->propName("Seed_Ret.nSeed");
    m_timer[4] = timerSvc->addItem("TMak_NE"); 
    m_timer[5] = timerSvc->addItem("TMak_Get"); 
    m_timer[5]->propName("TMak_Get.nSeed");
    m_timer[6] = timerSvc->addItem("TMak_Qm");
    m_timer[6]->propName("TMak_Qm.nTracks");
    m_timer[7] = timerSvc->addItem("TMak_EE");
    m_timer[8] = timerSvc->addItem("TrackFilt");
    m_timer[8]->propName("TrackFlt.nTracks");
    m_timer[9] = timerSvc->addItem("ZVMaker");

  }

  athenaLog << MSG::INFO << "TrigL2PattRecoStrategyT initialized "<< endreq;
  return sc;
}

StatusCode TrigL2PattRecoStrategyT::finalize() {

  m_filteredDriftCircleContainer->cleanup();
  m_filteredDriftCircleContainer->release();

  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

TrigL2PattRecoStrategyT::~TrigL2PattRecoStrategyT() {

}

HLT::ErrorCode TrigL2PattRecoStrategyT::findTracks(const TrigInDetTrackCollection&  inputTrackColl,
						   const std::vector<const TrigSiSpacePoint*>& //vsp
						   , 
						   const IRoiDescriptor& roi,
						   TrigInDetTrackCollection*  trackColl
						   ) {

  m_pattRecoTiming=0.0;
  HLT::ErrorCode rc=HLT::OK;

  m_zVertices.clear();

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::DEBUG) 
    athenaLog<< MSG::DEBUG<<"TrigL2PattRecoStrategyT called in RoI-based mode "<<endreq;


  StatusCode sc = getIDCs();
  if(sc.isFailure()) {
    athenaLog<<MSG::WARNING<<"Cannot retrieve SCT and Pixel SP IDCs "<<endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  std::vector<IdentifierHash>  listOfFilteredTrtIds;

  sc = getFilteredTRT_Hits(inputTrackColl,roi,listOfFilteredTrtIds);

  if(sc.isFailure()) {
    athenaLog<<MSG::WARNING<<"Failure in TRT hit filtering "<<endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  m_segmentsmaker->newRegion(listOfFilteredTrtIds);
  m_segmentsmaker->find(); 

  Trk::Segment* segment = 0;
  int m_nsegments = 0;
  
  // Loop through all segments and reconsrtucted segments collection preparation
  
  Trk::SegmentCollection* foundSegments  = new Trk::SegmentCollection;
  
  while((segment = m_segmentsmaker->next())) {
    ++m_nsegments; foundSegments->push_back(segment);
  }
  
  m_segmentsmaker->endEvent();

  if (outputLevel <= MSG::INFO) 
    athenaLog << MSG::INFO << "REGTEST: Found " << m_nsegments << " TRT segments" << endreq;

  if(m_nsegments==0) 
    return rc;  

  std::vector<IdentifierHash>  listOfSCTIds; 
  std::vector<IdentifierHash>  listOfPixIds; 
  
  m_regionSelector->DetHashIDList(SCT, roi, listOfSCTIds );
  m_regionSelector->DetHashIDList(PIXEL, roi, listOfPixIds);

  m_trackmaker->newRegion(listOfPixIds,listOfSCTIds); //RoI-based reconstruction

  TrackCollection* tempTracks = new TrackCollection;           //Temporary track collection per event
  
  int m_nTrtSegGood=0;
  
  for(Trk::SegmentCollection::iterator segIt=foundSegments->begin();segIt!=foundSegments->end();++segIt) {

    const Trk::Segment* trtSegment = (*segIt);

    const Trk::TrackSegment* trtTrack = dynamic_cast<const Trk::TrackSegment*>(trtSegment);

    int nTrtHits = (*segIt)->numberOfMeasurementBases();

    if(outputLevel <= MSG::INFO) 
      athenaLog << MSG::INFO << "Number Of Trt hits " << nTrtHits << endreq;
    if(nTrtHits<m_minTrtHits) {
      if (outputLevel <= MSG::INFO) 
	athenaLog << MSG::INFO << "Found segment with too few TRT hits =  " << nTrtHits << endreq;
      continue;
    }

    m_nTrtSegGood++;
    std::list<Trk::Track*> trackSi = m_trackmaker->getTrack(*trtTrack); //Get the possible Si extensions
      
    if(trackSi.size()==0){
      if (outputLevel <= MSG::INFO) 
	athenaLog << MSG::INFO << "No Si track candidates associated to the TRT track " << endreq;
      continue;
    }

    if (outputLevel <= MSG::INFO) 
      athenaLog << MSG::INFO << "REGTEST: Found " << (trackSi.size()) << " Si tracks associated to the TRT track " 
		<< endreq;
	
    for (std::list<Trk::Track*>::const_iterator itt = trackSi.begin(); itt != trackSi.end(); ++itt){

      tempTracks->push_back((*itt));
	  
      const DataVector<const Trk::TrackStateOnSurface>* temptsos = (*itt)->trackStateOnSurfaces();
	  
      if(!temptsos) continue;

      createTriggerTrack(**itt,*trtTrack,trackColl);
    }
  }
  
  delete tempTracks;
  delete foundSegments;

  return rc;

}


HLT::ErrorCode TrigL2PattRecoStrategyT::findTracks(const TrigInDetTrackCollection&  //inputTrackColl
						   ,
						   const std::vector<const TrigSiSpacePoint*>& //vsp
						   ,
						   TrigInDetTrackCollection* //trackColl
						   ) {

  m_pattRecoTiming=0.0;
  m_zVertices.clear();

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  HLT::ErrorCode rc=HLT::OK;
  if (outputLevel <= MSG::DEBUG) 
    athenaLog<< MSG::DEBUG<<"TrigL2PattRecoStrategyT called in full-scan mode "<<endreq;

  /*
  StatusCode sc=convertInputData();

  if(sc.isFailure()) {
    athenaLog<< MSG::WARNING<<"Failed to convert input data into offline spacepoints "<<endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  //initialize monitoring variables
  m_nseeds = 0;
  m_ntracks= 0;

  // Test is sct clusters collection for given event
  //
  bool PIX = true;
  bool SCT = true;

  std::list<Trk::Vertex> VZ; 

  if(m_timers) m_timer[0]->start();
  if( m_useZvertexTool ) {    

    if(m_timers) m_timer[9]->start();
    m_zvertexmaker->newEvent(); 
    VZ = m_zvertexmaker->getVertices();
    std::for_each(VZ.begin(),VZ.end(),ZVertexCopyFunctor(m_zVertices));
    if(m_timers) m_timer[9]->stop();   
    if (outputLevel <= MSG::DEBUG) 
      athenaLog << MSG::DEBUG << "REGTEST: Number of zvertex found = " << VZ.size() << endreq;
  }
  else{
    if(m_timers) m_timer[1]->start();
    m_seedsmaker->newEvent();    
    if(m_timers) m_timer[1]->stop();
  }

  if(m_timers) m_timer[2]->start();
  m_seedsmaker->find3Sp(VZ);
  if(m_timers) m_timer[2]->stop();

  if(m_timers) m_timer[4]->start();
  m_trackmaker->newTrigEvent(PIX,SCT);
  if(m_timers) m_timer[4]->stop();

  // Loop through all seeds and reconsrtucted tracks collection preparation
  //

  TrackCollection* foundTracks = new TrackCollection;
  std::multimap<double,Trk::Track*>    qualityTrack;

  m_nseeds  = 0;
  m_ntracks = 0;

  const InDet::SiSpacePointsSeed* seed = 0;

  if(m_timers) { 
    m_timer[3]->start();m_timer[3]->pause();
    m_timer[5]->start();m_timer[5]->pause();
    m_timer[6]->start();m_timer[6]->pause();
  }

  while(true) {
    if(m_timers) m_timer[3]->resume();
    seed = m_seedsmaker->next();
    if(m_timers) m_timer[3]->pause();

    if(seed==0) break;

    ++m_nseeds;
    if(m_timers) m_timer[5]->resume();
    const std::list<Trk::Track*>& T = m_trackmaker->getTracks(seed->spacePoints()); 
    if(m_timers) m_timer[5]->pause();
    if(m_timers) m_timer[6]->resume();
    for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
      if((*t)) 
	{
	  ++m_ntracks;
	  qualityTrack.insert(std::make_pair(-trackQuality((*t)),(*t)));
	}
    }
    if(m_timers) m_timer[6]->pause();
  }
  if(m_timers) { 
    m_timer[3]->propVal(m_nseeds);
    m_timer[3]->stop();
    m_timer[5]->propVal(m_nseeds);
    m_timer[5]->stop();
    m_timer[6]->propVal(m_ntracks);
    m_timer[6]->stop();
  }
  

  if(m_timers) m_timer[7]->start();
  m_trackmaker->endEvent();
  if(m_timers) m_timer[7]->stop();

  // Remove shared tracks with worse quality
  //

  if(m_timers) m_timer[8]->start();
  filterSharedTracks(qualityTrack);
  m_timer[8]->propVal(m_ntracks);
  if(m_timers) m_timer[8]->stop();

  std::multimap<double,Trk::Track*>::iterator 
    q = qualityTrack.begin(), qe =qualityTrack.end(); 

  m_ntracks = 0;

  for(; q!=qe; ++q) {
    foundTracks->push_back((*q).second); ++m_ntracks; 
  }

  if(m_timers) {
    m_timer[0]->stop();
    m_pattRecoTiming=m_timer[0]->elapsed();
  }

  if (outputLevel <= MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "REGTEST: Number of seeds found = " << m_nseeds << endreq;
    athenaLog << MSG::DEBUG << "REGTEST: Investigated "
	      << std::setw(7) <<  m_nseeds 
	      << " seeds and found "
	      << std::setw(5) << foundTracks->size() <<" tracks"
	      << endreq;
  }

  for (size_t i=0; i<foundTracks->size() ; i++){
    TrigInDetTrack* p = convertOutputTracks(foundTracks->at(i));
    if(p!=NULL) {
      trackColl->push_back(p);
    }
  }

  delete foundTracks;
  */

  return rc;
}


///////////////////////////////////////////////////////////////////
// Track quality calculation
///////////////////////////////////////////////////////////////////
/*
double TrigL2PattRecoStrategyT::trackQuality(const Trk::Track* Tr)
{
 
 DataVector<const Trk::TrackStateOnSurface>::const_iterator  
    m  = Tr->trackStateOnSurfaces()->begin(), 
    me = Tr->trackStateOnSurfaces()->end  ();

 double quality = 0. ;
 double W       = 17.;

 for(; m!=me; ++m) {
   const Trk::FitQualityOnSurface* fq =  (*m)->fitQualityOnSurface();
   if(!fq) continue;

   double x2 = fq->chiSquared();
   double q;
   if(fq->numberDoF() == 2) q = (1.2*(W-x2*.5)); 
   else                     q =      (W-x2    );
   if(q < 0.) q = 0.; quality+=q;
 }
 return quality;
}

///////////////////////////////////////////////////////////////////
// Filter shared tracks
///////////////////////////////////////////////////////////////////

void TrigL2PattRecoStrategyT::filterSharedTracks(std::multimap<double,Trk::Track*>& QT) {

  std::set<const Trk::PrepRawData*> clusters;
  std::multimap<double,Trk::Track*>::iterator q = QT.begin();
  
  const Trk::PrepRawData* prd[100]; 
  
  while(q!=QT.end()) {
    
    std::set<const Trk::PrepRawData*>::iterator fe = clusters.end();

    DataVector<const Trk::MeasurementBase>::const_iterator 
      m  = (*q).second->measurementsOnTrack()->begin(), 
      me = (*q).second->measurementsOnTrack()->end  ();
 
    int nf = 0, nc = 0; 
    for(; m!=me; ++m) {

      const Trk::PrepRawData* pr = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
      if(pr) {
	++nc; if(clusters.find(pr)==fe) {prd[nf++]=pr; if(nf==100) break;}
      }
    }

    if(nf >= m_nfreeCut|| nf == nc) {
      for(int n=0; n!=nf; ++n) clusters.insert(prd[n]); ++q;
    } 
    else  {
      delete (*q).second; QT.erase(q++);
    }
  }
}
*/

void TrigL2PattRecoStrategyT::createTriggerTrack(const Trk::Track& siTrack
						 , 
						 const Trk::TrackSegment& trtTrack
						 ,
						 TrigInDetTrackCollection* trackColl
						 ) {

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  TrigInDetTrack* pTrack=NULL;

  //converting the silicon part

  std::vector<const TrigSiSpacePoint*>* pvsp = new std::vector<const TrigSiSpacePoint*>;
  pvsp->clear();

  std::vector<const TrigSiSpacePoint*> vpsctSP;
  vpsctSP.clear();
  std::vector<const InDet::SCT_Cluster*> vpClust;
  vpClust.clear();

  DataVector<const Trk::MeasurementBase>::const_iterator mIt = siTrack.measurementsOnTrack()->begin();
  for(; mIt!=siTrack.measurementsOnTrack()->end();++mIt) {
    const Trk::PrepRawData* pPRD = ((const Trk::RIO_OnTrack*)(*mIt))->prepRawData();
    if(pPRD) {
      const InDet::SCT_Cluster* pSCT = dynamic_cast<const InDet::SCT_Cluster*>(pPRD);
      if(pSCT) {
	const IdentContIndex ici = pSCT->getHashAndIndex();
	//	std::cout<<"SCT cluster hashId="<<ici.collHash()<<std::endl; 
	TrigSiSpacePointContainer::const_iterator spCollIt = m_sctOnlineSpacePointsContainer->indexFind(ici.collHash());
	if(spCollIt!=m_sctOnlineSpacePointsContainer->end()) {
	  //  std::cout<<"Found ! iterating now looking for "<<pSCT->identify()<<std::endl;
	  for(TrigSiSpacePointCollection::const_iterator spIt=(*spCollIt)->begin();spIt!=(*spCollIt)->end();++spIt) {
	    if(pSCT->identify()==(*spIt)->clusters().first->identify()) {
	      //   std::cout<<"Phi cluster detected, corresponding stereo id="<<(*spIt)->clusters().second->identify()<<std::endl;
	      vpsctSP.push_back((*spIt));
	    }
	  }
	}
	else {
	  //	  std::cout<<"Not found, could be a stereo cluster, id="<<pSCT->identify()<<std::endl;
	  vpClust.push_back(pSCT);
	}
      }
      else {
	const InDet::PixelCluster* pPIX = dynamic_cast<const InDet::PixelCluster*>(pPRD);
	if(pPIX) {
	  const IdentContIndex ici = pPIX->getHashAndIndex();
	  //  std::cout<<"Pixel cluster hashId="<<ici.collHash()<<std::endl; 
	  TrigSiSpacePointContainer::const_iterator spCollIt = m_pixelOnlineSpacePointsContainer->indexFind(ici.collHash());
	  if(spCollIt!=m_pixelOnlineSpacePointsContainer->end()) {
	    //   std::cout<<"Found ! iterating now"<<std::endl;
	    for(TrigSiSpacePointCollection::const_iterator spIt=(*spCollIt)->begin();spIt!=(*spCollIt)->end();++spIt) {
	      if(pPIX->identify()==(*spIt)->clusters().first->identify()) {
		//		std::cout<<"Pixel cluster detected"<<std::endl;
		pvsp->push_back((*spIt));
		//	std::cout<<"sp stored"<<std::endl;
	      }
	    }
	  }
	  else {
	    std::cout<<"ERROR PIXEL CL NOT FOUND IN SP id="<<pPIX->identify()<<std::endl;
	  }
	}
      }
    }
  }

  // resolving stereo clusters
  int nClust = vpClust.size();
  for(std::vector<const TrigSiSpacePoint*>::const_iterator spIt=vpsctSP.begin();spIt!=vpsctSP.end();++spIt) {
    for(std::vector<const InDet::SCT_Cluster*>::const_iterator clIt=vpClust.begin();clIt!=vpClust.end();++clIt) {
      if((*spIt)->clusters().second->identify()==(*clIt)->identify()) {
	pvsp->push_back(*spIt);
	--nClust;
      }
    }
  }
  if (outputLevel <= MSG::DEBUG) {
    athenaLog << MSG::DEBUG <<nClust<<" SCT clusters left unassigned "<<endreq;
  }

  if(pvsp->empty()) {
    delete pvsp;
    return;
  }

  if (outputLevel <= MSG::DEBUG) {
    athenaLog << MSG::DEBUG <<"created vector with "<<pvsp->size()<<" spacepoints"<<endreq;
  }

  // converting the TRT part

  int nTR=0,nTime=0,nHits=0;
  bool validDT=false;

  std::vector<const Trk::MeasurementBase*>::const_iterator s  = trtTrack.containedMeasurements().begin(),
    se = trtTrack.containedMeasurements().end();
  
  std::vector<const InDet::TRT_DriftCircle*>* pvDC=new std::vector<const InDet::TRT_DriftCircle*>;
    
  for(++s; s!=se; ++s) {
    if (!(dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(*s))) continue;
    const InDet::TRT_DriftCircle* dc = (dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(*s))->prepRawData();
    if(dc!=0){
      nHits++;
      pvDC->push_back(dc);
      if(dc->highLevel()) nTR++;
      dc->driftTime(validDT);
      if(validDT) nTime++;
      }
  }

  pTrack->StrawHits(nHits);
  pTrack->StrawTime(nTime);
  pTrack->TRHits(nTR);

  pTrack->trtDriftCircles(pvDC); 

  // now track parameters;

  const Trk::TrackParameters* trackPars;
  trackPars = siTrack.perigeeParameters();

  double D0 = trackPars->parameters()[Trk::d0]; 
  double Z0 = trackPars->parameters()[Trk::z0]; 
  double Phi0 = trackPars->parameters()[Trk::phi0]; 
  double Theta = trackPars->parameters()[Trk::theta]; 
  if(Phi0>M_PI) Phi0-=2*M_PI; 
  if(Phi0<-M_PI) Phi0+=2*M_PI; 
  double Eta = -log(sin(0.5*Theta)/cos(0.5*Theta)); 
    
  double qOverP = trackPars->parameters()[Trk::qOverP]; 
  double Pt=sin(Theta)/qOverP; 

  const TrigInDetTrackFitPar* tidtfp = new TrigInDetTrackFitPar(D0,Phi0,Z0,Eta,Pt,NULL); 
  
  pTrack = new TrigInDetTrack(pvsp,tidtfp);
  pTrack->algorithmId(TrigInDetTrack::STRATEGY_C_ID);

  trackColl->push_back(pTrack);
}

StatusCode TrigL2PattRecoStrategyT::getFilteredTRT_Hits(const TrigInDetTrackCollection& inputTrackColl, 
							const IRoiDescriptor& roi,
							std::vector<IdentifierHash>&  listOfFilteredTrtIds) {

  StatusCode sc(StatusCode::SUCCESS);

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  std::vector<IdentifierHash>  listOfTrtIds; 

  //1. Collect used TRT hits

  m_filteredDriftCircleContainer->cleanup();
  if(!evtStore()->contains<InDet::TRT_DriftCircleContainer>(m_trtFilteredContName))
    {
      StatusCode sc=evtStore()->record(m_filteredDriftCircleContainer,m_trtFilteredContName,false);
      if(sc.isFailure())
	{
	  athenaLog << MSG::WARNING << "TRT DriftCircleContainer " << m_trtFilteredContName <<
	    " cannot be recorded into StoreGate! "<<endreq;
	  return sc;
	} 
      else 
	{ 
	  if (outputLevel <= MSG::INFO) 
	    athenaLog << MSG::INFO<<"TRT DriftCircleContainer " << m_trtFilteredContName <<
	      "  recorded into StoreGate! "<<endreq;
	}
    } 

  std::set<Identifier> m_usedHits;
  m_usedHits.clear();
  if(!inputTrackColl.empty()) {

    //1a. loop over input tracks, build a set of used TRT hits

    if (outputLevel <= MSG::INFO) 
      athenaLog << MSG::INFO << "Total " << inputTrackColl.size()<<" input tracks..."<< endreq;

    TrigInDetTrackCollection::const_iterator trIt,lastIt;

    trIt = inputTrackColl.begin();lastIt = inputTrackColl.end();
    for(; trIt !=lastIt; trIt++) {
      if((*trIt)->trtDriftCircles()==NULL) continue;
      std::vector<const InDet::TRT_DriftCircle*>::iterator hitIt=(*trIt)->trtDriftCircles()->begin(),
	hitItEnd=(*trIt)->trtDriftCircles()->end();
      for(;hitIt!=hitItEnd;++hitIt)
	m_usedHits.insert((*hitIt)->identify());
    }
  }

  //2. retrieve TRT hits from a given RoI

  m_regionSelector->DetHashIDList(TRT, roi, listOfTrtIds);
  
  if (outputLevel <= MSG::INFO) 
    athenaLog << MSG::INFO<< listOfTrtIds.size() << " REGTEST: TRT Dets in region" << endreq;
  
  std::vector<unsigned int> uIntListOfTRTRobs;

  m_regionSelector->DetROBIDListUint(TRT, uIntListOfTRTRobs);
  m_robDataProvider->addROBData(uIntListOfTRTRobs);
  m_vROBF.clear();
  m_robDataProvider->getROBData(uIntListOfTRTRobs,m_vROBF);
  //  m_trtDataErrors.clear();
  
  sc = m_trtDataProvider->fillCollections(listOfTrtIds,m_vROBF);

 
  if(sc.isRecoverable())
    {
      if(outputLevel <= MSG::INFO)
	athenaLog<<MSG::INFO<<"Recoverable errors during TRT BS conversion  "<<endreq;
      /*
      const std::vector<int>* errVect = m_trtDataProvider->fillTRT_DataErrors();
      std::copy(errVect->begin(),errVect->end(),std::back_inserter(m_trtDataErrors));
      */
    }
  else if(sc.isFailure())
    {
      athenaLog << MSG::WARNING << " TRT Data provider failed" << endreq;
      return sc;
    }

  // 3. Retrieve IDC

  const InDet::TRT_DriftCircleContainer* trtContainer;
  sc=evtStore()->retrieve(trtContainer,m_trtDataProvider->trtContainerName());
  if(sc.isFailure())
    {
      athenaLog<<MSG::WARNING<<"TRT DriftCircle container is not found: name "
	       <<m_trtDataProvider->trtContainerName()<<
	" pointer="<<trtContainer<< endreq;
      return sc;
    }
  if(outputLevel <= MSG::INFO)
    athenaLog<<MSG::INFO<<"TRT DriftCircle container retrieved"<<endreq;

  int nTotalHits=0;
  int nFilteredHits=0;

  // 4. collect unused TRT hits

  listOfFilteredTrtIds.clear();


  for(std::vector<IdentifierHash>::iterator idIt=listOfTrtIds.begin();idIt!=listOfTrtIds.end();++idIt){

      InDet::TRT_DriftCircleContainer::const_iterator collIt(trtContainer->indexFind(*idIt));
      if(collIt==trtContainer->end()) continue;

      const InDet::TRT_DriftCircleCollection* coll=&(**collIt);
      if(coll->begin()==coll->end()) continue;

      InDet::TRT_DriftCircleCollection* pNewColl=NULL;
      pNewColl = new InDet::TRT_DriftCircleCollection(coll->identifyHash());
      pNewColl->setIdentifier(coll->identify());

      for(InDet::TRT_DriftCircleCollection::const_iterator dcIt=coll->begin();dcIt!=coll->end();++dcIt) {
	Identifier dcId = (*dcIt)->identify();
	nTotalHits++;
	if(m_usedHits.find(dcId)!=m_usedHits.end()) continue;

	InDet::TRT_DriftCircle* pRIO = new InDet::TRT_DriftCircle(**dcIt);
		
	pRIO->setHashAndIndex(pNewColl->identifyHash(), pNewColl->size());
	/*
	std::cout<<"Old DC:"<<std::endl;
	std::cout<<(**dcIt)<<std::endl;
	std::cout<<"Copy DC:"<<std::endl;
	std::cout<<(*pRIO)<<std::endl;
	*/
	pNewColl->push_back(pRIO);
	nFilteredHits++;
      }
      StatusCode scColl = m_filteredDriftCircleContainer->addCollection(pNewColl,pNewColl->identifyHash());
      listOfFilteredTrtIds.push_back(pNewColl->identifyHash());
      if(scColl.isFailure()) {
	athenaLog<<MSG::WARNING<<"Cannot store collection in IDC "<< endreq;
      }
  }

  if (outputLevel <= MSG::INFO) 
    athenaLog << MSG::INFO << "Total " << nTotalHits <<" TRT hits in RoI, "<<nFilteredHits<<" left after filtering"<< endreq;

  return sc;
}


StatusCode TrigL2PattRecoStrategyT::getFilteredTRT_Hits(const TrigInDetTrackCollection& inputTrackColl,
							std::vector<IdentifierHash>&  listOfFilteredTrtIds) {
  
  StatusCode sc(StatusCode::SUCCESS);

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  std::vector<IdentifierHash>  listOfTrtIds; 

  //1. Collect used TRT hits

  m_filteredDriftCircleContainer->cleanup();
  if(!evtStore()->contains<InDet::TRT_DriftCircleContainer>(m_trtFilteredContName))
    {
      StatusCode sc=evtStore()->record(m_filteredDriftCircleContainer,m_trtFilteredContName,false);
      if(sc.isFailure())
	{
	  athenaLog << MSG::WARNING << "TRT DriftCircleContainer " << m_trtFilteredContName <<
	    " cannot be recorded into StoreGate! "<<endreq;
	  return sc;
	} 
      else 
	{ 
	  if (outputLevel <= MSG::INFO) 
	    athenaLog << MSG::INFO<<"TRT DriftCircleContainer " << m_trtFilteredContName <<
	      "  recorded into StoreGate! "<<endreq;
	}
    } 

  std::set<Identifier> m_usedHits;
  m_usedHits.clear();
  if(!inputTrackColl.empty()) {

    //1a. loop over input tracks, build a set of used TRT hits

    if (outputLevel <= MSG::INFO) 
      athenaLog << MSG::INFO << "Total " << inputTrackColl.size()<<" input tracks..."<< endreq;

    TrigInDetTrackCollection::const_iterator trIt,lastIt;

    trIt = inputTrackColl.begin();lastIt = inputTrackColl.end();
    for(; trIt !=lastIt; trIt++) {
      if((*trIt)->trtDriftCircles()==NULL) continue;
      std::vector<const InDet::TRT_DriftCircle*>::iterator hitIt=(*trIt)->trtDriftCircles()->begin(),
	hitItEnd=(*trIt)->trtDriftCircles()->end();
      for(;hitIt!=hitItEnd;++hitIt)
	m_usedHits.insert((*hitIt)->identify());
    }
  }

  //2. retrieve TRT hits

  m_regionSelector->DetHashIDList(TRT, listOfTrtIds);
  
  if (outputLevel <= MSG::INFO) 
    athenaLog << MSG::INFO<< listOfTrtIds.size() << " REGTEST: TRT Dets in region" << endreq;
  
  std::vector<unsigned int> uIntListOfTRTRobs;

  m_regionSelector->DetROBIDListUint(TRT,uIntListOfTRTRobs);
  m_robDataProvider->addROBData(uIntListOfTRTRobs);
  m_vROBF.clear();
  m_robDataProvider->getROBData(uIntListOfTRTRobs,m_vROBF);
  // m_trtDataErrors.clear();
  
  sc = m_trtDataProvider->fillCollections(listOfTrtIds,m_vROBF);

  if(sc.isRecoverable())
    {
      if(outputLevel <= MSG::INFO)
	athenaLog<<MSG::INFO<<"Recoverable errors during TRT BS conversion  "<<endreq;
      /*
      const std::vector<int>* errVect = m_trtDataProvider->fillTRT_DataErrors();
      std::copy(errVect->begin(),errVect->end(),std::back_inserter(m_trtDataErrors));
      */
    }
  else if(sc.isFailure())
    {
      athenaLog << MSG::WARNING << " TRT Data provider failed" << endreq;
      return sc;
    }

  // 3. Retrieve IDC

  const InDet::TRT_DriftCircleContainer* trtContainer;
  sc=evtStore()->retrieve(trtContainer,m_trtDataProvider->trtContainerName());
  if(sc.isFailure())
    {
      athenaLog<<MSG::WARNING<<"TRT DriftCircle container is not found: name "
	       <<m_trtDataProvider->trtContainerName()<<
	" pointer="<<trtContainer<< endreq;
      return sc;
    }
  if(outputLevel <= MSG::INFO)
    athenaLog<<MSG::INFO<<"TRT DriftCircle container retrieved"<<endreq;

  int nTotalHits=0;
  int nFilteredHits=0;

  // 4. collect unused TRT hits

  listOfFilteredTrtIds.clear();

  for(std::vector<IdentifierHash>::iterator idIt=listOfTrtIds.begin();idIt!=listOfTrtIds.end();++idIt){

      InDet::TRT_DriftCircleContainer::const_iterator collIt(trtContainer->indexFind(*idIt));
      if(collIt==trtContainer->end()) continue;

      const InDet::TRT_DriftCircleCollection* coll=&(**collIt);
      if(coll->begin()==coll->end()) continue;

      InDet::TRT_DriftCircleCollection* pNewColl=NULL;
      pNewColl = new InDet::TRT_DriftCircleCollection(coll->identifyHash());
      pNewColl->setIdentifier(coll->identify());

      for(InDet::TRT_DriftCircleCollection::const_iterator dcIt=coll->begin();dcIt!=coll->end();++dcIt) {
	Identifier dcId = (*dcIt)->identify();
	nTotalHits++;
	if(m_usedHits.find(dcId)!=m_usedHits.end()) continue;

	InDet::TRT_DriftCircle* pRIO = new InDet::TRT_DriftCircle(**dcIt);
		
	pRIO->setHashAndIndex(pNewColl->identifyHash(), pNewColl->size());
	/*
	std::cout<<"Old DC:"<<std::endl;
	std::cout<<(**dcIt)<<std::endl;
	std::cout<<"Copy DC:"<<std::endl;
	std::cout<<(*pRIO)<<std::endl;
	*/
	pNewColl->push_back(pRIO);
	nFilteredHits++;
      }
      StatusCode scColl = m_filteredDriftCircleContainer->addCollection(pNewColl,pNewColl->identifyHash());
      listOfFilteredTrtIds.push_back(pNewColl->identifyHash());
      if(scColl.isFailure()) {
	athenaLog<<MSG::WARNING<<"Cannot store collection in IDC "<< endreq;
      }
  }

  if (outputLevel <= MSG::INFO) 
    athenaLog << MSG::INFO << "Total " << nTotalHits <<" TRT hits in RoI, "<<nFilteredHits<<" left after filtering"<< endreq;

   return sc;
 }


 
StatusCode TrigL2PattRecoStrategyT::getIDCs() {

  MsgStream athenaLog(msgSvc(), name());
  
  StatusCode sc(StatusCode::SUCCESS);

  sc = evtStore()->retrieve(m_pixelOnlineSpacePointsContainer, m_pixelOnlineSpacePointsName); 
  if (sc.isFailure()) { 
    athenaLog << MSG::ERROR << "Failed to get Pixel SpacePoints Container " <<m_pixelOnlineSpacePointsName<< endreq; 
    return sc; 
  } 
  else {
    /*  
    athenaLog << MSG::DEBUG << "Got Pixel SpacePoints Container from TDS: " 
	      << m_pixelOnlineClustersName << endreq; 
    */
  } 

  sc = evtStore()->retrieve(m_sctOnlineSpacePointsContainer, m_sctOnlineSpacePointsName); 
  if (sc.isFailure()) { 
    athenaLog << MSG::ERROR << "Failed to get SCT SpacePoints Container " <<m_sctOnlineSpacePointsName<< endreq; 
    return sc; 
  } 
  else {
    /*  
    athenaLog << MSG::DEBUG << "Got SCT SpacePoints Container from TDS: " 
	      << m_pixelOnlineClustersName << endreq; 
    */
  }
  return sc;
}

