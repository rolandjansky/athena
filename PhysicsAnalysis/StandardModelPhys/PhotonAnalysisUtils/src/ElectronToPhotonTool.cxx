/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// PAUphotonSelector.cxx, (c) ATLAS Detector software 2007
///////////////////////////////////////////////////////////////////////////////
// author: Marcello Fanti, marcello.fanti@mi.infn.it
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////

#include "StoreGate/StoreGateSvc.h"
#include "PhotonAnalysisUtils/ElectronToPhotonTool.h"
#include "AthenaKernel/errorcheck.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/Photon.h"

#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"
#include "egammaEvent/EMConvert.h"
#include "VxVertex/VxCandidate.h"

#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "VxVertex/VxTrackAtVertex.h"	

ElectronToPhotonTool::ElectronToPhotonTool(const std::string& t, const std::string& n, const IInterface* p) 
  : AthAlgTool(t,n,p),
    m_clusterTool("egammaTools/EMClusterTool"),
    m_fourMomBuilderTool("EMFourMomBuilder/egfourmom"),
    m_emshowerBuilderTool("EMShowerBuilder/emshower"),
    m_singleTrkConvTool("InDet::SingleTrackConversionTool")
{
  declareInterface<IElectronToPhotonTool>(this);

  declareProperty("ClusterTool",                 m_clusterTool        );
  declareProperty("FourMomentumTool",            m_fourMomBuilderTool );
  declareProperty("EMShowerBuilderTool",         m_emshowerBuilderTool);
  declareProperty("SingleTrackConversionTool",   m_singleTrkConvTool  );

  declareProperty("TrackParticleContainerName",  m_trackParticleContainerName = "TrackParticleCandidate");
}

ElectronToPhotonTool::~ElectronToPhotonTool(){}

StatusCode ElectronToPhotonTool::initialize() {
  MsgStream log(msgSvc(), name());

  CHECK( m_clusterTool.retrieve() );
  CHECK( m_fourMomBuilderTool.retrieve() );
  CHECK( m_emshowerBuilderTool.retrieve() );
  CHECK( m_singleTrkConvTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode ElectronToPhotonTool::finalize() {
  MsgStream log(msgSvc(), name());
  log <<  MSG::VERBOSE  << "... in finalize() ..." << endreq ;
  return StatusCode::SUCCESS;
}

Analysis::Photon* ElectronToPhotonTool::ConvertElectronToPhoton(const egamma* eg, bool conv, const std::string& egDetailContainerName) {
  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "entering elecToPhoton, conv=" << conv << "\t&egamma=" << eg << endreq ;

  // check that the shower is there...
  const EMShower* shower = dynamic_cast<const EMShower*>(eg->detail("EMShower"));
  if ( shower ) {
    log << MSG::VERBOSE << "... &shower=" << std::hex << shower << "\tfound with key=" << egDetailContainerName << endreq ;
  } else {
    shower = eg->detail<EMShower>() ;
    if ( shower ) {
      log << MSG::VERBOSE << "... &shower=" << std::hex << shower << "\tfound with no key" << endreq ;
    } else{
      if( eg->author() == egammaParameters::AuthorElectron) 
	log << MSG::WARNING << "Shower not found! return null pointer!" << endreq ;
      return 0;
    }
  }

  // create the new photon
  Analysis::Photon* ph = new Analysis::Photon( egammaParameters::AuthorPhoton );

  // set up a conversion vertex 
  if ( conv ) {
    if ( ! makeupConversionVertex(ph,eg) ) {
      log << MSG::INFO << "Cannot set a conversion vertex (no track)! return null pointer!" << endreq ;
      delete ph;
      return 0;
    }
  }
  const EMConvert * the_emConversion = ph->detail<EMConvert>();
  const Trk::VxCandidate*  convVtx = 0;
  if (ph->conversion()) convVtx = ph->conversion()->clone();

  // reassign the cluster
  CaloClusterContainer* newClusContainer = gimeContainer<CaloClusterContainer>("ClonedClusters");
  CaloCluster* newClus(0) ;
  if ( conv && convVtx ) {
    log << MSG::ERROR << "ElectronToPhotonTool: makeNewCluster needs to be migrated to xAOD." << endreq;
    //newClus = const_cast<CaloCluster*>(m_clusterTool->makeNewCluster(eg->cluster(), convVtx->recVertex().position().perp(), "conversion"));
  } else {
    log << MSG::ERROR << "ElectronToPhotonTool: makeNewCluster needs to be migrated to xAOD." << endreq;
    //newClus = const_cast<CaloCluster*>(m_clusterTool->makeNewCluster(eg->cluster(), -1. ,"photon"));
  }

  if(convVtx) delete convVtx;

  if ( newClus ) {
    newClusContainer->push_back(newClus);
    ph->setCluster(newClusContainer,newClus);
    log << MSG::VERBOSE << "... New cluster assigned" << endreq ;
  } else {
    log << MSG::WARNING << "Could not assign new cluster! return null pointer!" << endreq ;
    delete ph;
    return 0;
  }

  //* run the egamma tools, where we do not want to have TrackParticle links on the photon before we transfer the electron related links
  //* EMShower
  // If we run on ESDs then we can rebuild the full shower shapes from scratch
  log << MSG::ERROR << "ElectronToPhotonTool: EM shower building needs to be migrated to xAOD." << endreq;
  if(evtStore()->contains<CaloCellContainer>("AllCalo")){
    /*
    if((m_emshowerBuilderTool->execute(ph)).isFailure()){
      log << MSG::ERROR << "Problem executing EMShowerBuilder on electron Container" << endreq;
    }
    */
  } else {  //If we run on AODs, then we have limited number of Calo cells, so we should copy over things like cluster isolation etc.that cannot be computed properly
    /*
    if((m_emshowerBuilderTool->anaExecute(ph,const_cast<egamma*>(eg))).isFailure()){
      log << MSG::DEBUG << "Problem anaExecuting EMShowerBuilder on electron Container" << endreq;
    }
    */
  }

  // ... then call PID builder
  const EMShower* the_shower = ph->detail<EMShower>();

  // ... call the 4momentum builder, only after everything else has been done.  Ask Thomas why this is the case.
  log << MSG::ERROR << "ElectronToPhotonTool: 4momentum building needs to be migrated to xAOD." << endreq;
  /*
  if ( m_fourMomBuilderTool->execute(ph) == StatusCode::FAILURE ) {
    log << MSG::ERROR << "4momentum builder failed! return null pointer!" << endreq ;
    delete newClus;
    delete ph;
    return 0;
  }
  */
  delete newClus;
  delete ph;
  return 0;


  //
  log << MSG::VERBOSE << "... final checks:" 
      << ( (conv) ? std::string("conv") : std::string("unconv") )
      << "\t&photon=" << std::hex << ph
      << "\t&cluster=" << std::hex << ph->cluster()
      << "\t&EMConvert=" << std::hex << the_emConversion
      << "\tconvTrackMatch=" << ( (the_emConversion) ? the_emConversion->convTrackMatch() : 0 ) 
      << "\t&EMShower=" << std::hex << the_shower
      << "\t&conversion=" << std::hex << ph->conversion()
      << endreq ;
  if ( ph->cluster()==0 )              log << MSG::WARNING << "cloned photon has no cluster" << endreq ;
  if ( conv && ph->conversion()==0 )   log << MSG::WARNING << "cloned photon has no conversion, but it should!" << endreq ;
  if ( !conv && ph->conversion()!=0 )  log << MSG::WARNING << "cloned photon has a conversion, but it shouldn't!" << endreq ;
  if ( ph->trackParticle()!=0 )        log << MSG::WARNING << "cloned photon has a track"    << endreq ;
  if ( ph->detail<EMShower>()==0 )     log << MSG::WARNING << "cloned photon has no shower"  << endreq ;

  log << MSG::VERBOSE << "... elecToPhoton finished!" << endreq ;
  return ph;
}

template <class T>
T* ElectronToPhotonTool::gimeContainer(const std::string& containerName) {
  MsgStream log( msgSvc(), name() );
  T* container ;
  StatusCode sc ;
  if ( evtStore()->contains<T>(containerName) ) {
    sc = evtStore()->retrieve(container,containerName) ;
    if ( sc.isSuccess() ) {
      log << MSG::VERBOSE << "Container " << containerName << " found in StoreGate" << endreq ;
      return container ;
    } else {
      log << MSG::ERROR << "StoreGate hosts container " << containerName << " but could not retrieve it!" << endreq ;
      return 0;
    }
  } else {
    container = new T;
    sc = evtStore()->record(container,containerName) ;
    if ( sc.isSuccess() ) {
      log << MSG::VERBOSE << "Container " << containerName << " created and recorded in StoreGate" << endreq ;
      return container ;
    } else {
      log << MSG::ERROR << "Could not record container " << containerName << " to StoreGate!" << endreq ;
      delete container ;
      return 0;
    }
  }
}

bool ElectronToPhotonTool::hasBadCovarianceMatrix(const Analysis::Electron* el){
  if(el){
    const Rec::TrackParticle* tp = el->trackParticle();
    if(tp && tp->measuredPerigee()->covariance()==0){
      return true;
    }
    return false;
  }
  return false;
}

bool ElectronToPhotonTool::makeupConversionVertex(Analysis::Photon* /*ph*/, const egamma* eg) {
  MsgStream log( msgSvc(), name() );
  const Rec::TrackParticle* egTP = eg->trackParticle();
  if ( egTP==0 )  return false ;

  const Rec::TrackParticleContainer* TrackParticleInputContainer;
  CHECK( evtStore()->retrieve(TrackParticleInputContainer, m_trackParticleContainerName) );

  if(hasBadCovarianceMatrix(dynamic_cast<const Analysis::Electron*>(eg))){
    log << MSG::DEBUG <<"no TrackParticleInputContainer found in TDS" << endreq;
    return false;
  }

  // Search for TRT only track if not already flagged as conversion
  log << MSG::ERROR << "ElectronToPhotonTool: makeupConversionVertex single-track conversion finding needs to be updated to xAOD" << endreq;
  return false;

  /*
  Trk::VxCandidate* myVxCandidate = m_singleTrkConvTool-> buildSingleTrackParticleConversion(egTP);   
  if(myVxCandidate) {
    const std::vector< Trk::VxTrackAtVertex * > & trackAtVertexVec = *(myVxCandidate->vxTrackAtVertex());
    
    for (unsigned int i = 0; i < trackAtVertexVec.size(); i++) {
      Trk::ITrackLink* trklink = trackAtVertexVec[i]->trackOrParticleLink();
      Trk::LinkToTrackParticleBase* linkToTrackPB = dynamic_cast<Trk::LinkToTrackParticleBase*>(trklink);
      if(!linkToTrackPB) continue;
      else linkToTrackPB->setStorableObject(*TrackParticleInputContainer);
    }
  }
  else{
    log << MSG::WARNING << "single track conversion tool did not find a VxCandidate" << endreq;
    return false;
  }
  VxContainer* newVtxContainer = gimeContainer<VxContainer>("ClonedVertices");
  newVtxContainer->push_back(myVxCandidate);
  ph->setConversion(newVtxContainer,myVxCandidate);

  egDetailContainer* newConvertContainer = gimeContainer<egDetailContainer>("ClonedConverts");
  static const bool overwrite = true ;
  EMConvert* newConvert = new EMConvert();
  newConvert->set_parameter(egammaParameters::convTrackMatch,(double)(true),overwrite); // needed to let PIDtool know that it is converted!
  newConvert->set_parameter(egammaParameters::convAngleMatch,(double)(true),overwrite); // not needed, but seems harmless...
  newConvert->fillVxDetails(myVxCandidate,0);
  //newConvert->set_parameter(egammaParameters::refittedVertex_x,vtx->recVertex().position().x(),overwrite);  // x position of vertex, 
  //newConvert->set_parameter(egammaParameters::refittedVertex_y,vtx->recVertex().position().y(),overwrite);  // y position of vertex, 
  //newConvert->set_parameter(egammaParameters::refittedVertex_z,vtx->recVertex().position().z(),overwrite);  // z position of vertex, 
  newConvertContainer->push_back(newConvert);
  ph->setDetail(newConvertContainer,newConvert);
  
  return true ;
  */
}
