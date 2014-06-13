/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/ReadSiClusters.h"

#include <fstream>

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGate.h"

//#include "InDetRecInput/InDetRecInputCollection.h"
//#include "InDetRecInput/InDetRecInputContainer.h"
//#include "InDetRecInput/InDetRecInputCLASS_DEF.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "InDetPrepRawData/SiWidth.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "GaudiKernel/AlgFactory.h"

// Create a factory for this algorithm.
// static const AlgFactory<ReadSiClusters> s_factory;
// const IAlgFactory& ReadSiClusters_AlgFactory = s_factory; 

ReadSiClusters::ReadSiClusters(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),   
  m_nDet(1),m_StoreGate(0),p_ntuple(0)
{  
  // Declare the properties
  declareProperty("SiClusterContainerName", 
         m_SiClustersName="PixelClusters");
  declareProperty("NtupleDirName",
         m_ntupleDirName="PixelClusters");
  declareProperty("ReadSiClusters",m_readSiClusters=true);
}

StatusCode ReadSiClusters::initialize() {
  // Get the message service.
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << "In initialize()" << endreq;

  //  log << MSG::DEBUG << "nTestingDetectors = " << m_nDet << endreq;

  // Get StoreGate service
  //  IService* svc;
  //  StatusCode sc = serviceLocator()->getService("StoreGateSvc", svc);  
  //  if (!sc.isSuccess()) {
  //    log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
  //    return sc;                  
  //  }

  //  m_StoreGate = dynamic_cast<StoreGateSvc*> (svc); 
  //  if(m_StoreGate == 0) {
  //    log<<MSG::ERROR<< "SiClusterContByteStreamCnv: Cant cast to  StoreGateSvc" <<endreq; 
  //    return StatusCode::FAILURE ;
  //  }

  StatusCode sc=service("StoreGateSvc",m_StoreGate);
  if (sc.isFailure()) {
    log << MSG::FATAL << "StoreGate service not found !" << endreq;
    return StatusCode::FAILURE;
  } else {}

  if(m_readSiClusters) sc = BookSiNtuple();  
  return sc;
}

StatusCode ReadSiClusters::BookSiNtuple(){

  MsgStream log(msgSvc(), name());
  NTuple::Directory *p_directory=0;
  std::string dirName="/NTUPLES/FILE1";
  NTupleFilePtr ntupleFile(ntupleSvc(),dirName);
  dirName+="/"+m_ntupleDirName;
  if (0!=ntupleFile) {
    p_directory=ntupleSvc()->createDirectory(dirName);
    if (!p_directory) {
      log << MSG::FATAL << "Cannot create ntuple directory !" << endreq;
      return StatusCode::FAILURE;
    } else {}
  } else {
    log << MSG::FATAL << "Cannot create ntuple file !" << endreq;
    return StatusCode::FAILURE;
  }

  NTuplePtr ntuple(ntupleSvc(),dirName+"/50");
  if (0!=ntuple) {
    log << MSG::FATAL << "Ntuple already booked !" << endreq;
    return StatusCode::FAILURE;
  } else {}

  p_ntuple=ntupleSvc()->book(p_directory,50,
			     CLID_RowWiseTuple,m_ntupleDirName);
  if (!p_ntuple) {
    log << MSG::FATAL << "Cannot book ntuple !" << endreq;
    return StatusCode::FAILURE;
  } else {}
  
  p_ntuple->addItem("r",m_r);
  m_r=0;
  p_ntuple->addItem("phi",m_phi);
  m_phi=0;
  p_ntuple->addItem("z",m_z);
  m_z=0;
  p_ntuple->addItem("RowWidth",m_rowWidth);
  m_rowWidth=0;
  p_ntuple->addItem("ColWidth",m_colWidth);
  m_colWidth=0;
  p_ntuple->addItem("EtaWidth",m_etaWidth);
  m_etaWidth=0;
  p_ntuple->addItem("PhiWidth",m_phiWidth);
  m_phiWidth=0;
  p_ntuple->addItem("ErrPhi",m_errPhi);
  m_errPhi=0;
  p_ntuple->addItem("ErrZ",m_errZ);
  m_errZ=0;

  return StatusCode::SUCCESS;
}

StatusCode ReadSiClusters::finalize() {
  // Get the message service.
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "In finalize()" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode ReadSiClusters::execute() {

  StatusCode sc;
  if(m_readSiClusters){
    sc = RetrieveSiClusters();
    if(sc == StatusCode::FAILURE) return sc;
    sc = FillSiNtuple();
    if(sc == StatusCode::FAILURE) return sc;
  }
  return StatusCode::SUCCESS;
}

StatusCode ReadSiClusters::RetrieveSiClusters(){

    MsgStream log(msgSvc(), name());
    const DataHandle<EventInfo> eventInfo;
    StatusCode sc;
    sc = (m_StoreGate->retrieve(eventInfo));
    if (sc.isFailure()){
	log<<MSG::FATAL << "could not get event"<<endreq;
	return StatusCode::FAILURE;
    }

    m_eventNumber= eventInfo->event_ID()->event_number();
    log<< MSG::DEBUG<< "event number " << m_eventNumber<<endreq;
    m_numberOfEvents++;

    // get space points from TDS
    sc = m_StoreGate->retrieve(m_clusters, m_SiClustersName);
    if (sc.isFailure()){
      log << MSG::ERROR <<"Si cluster container not found"<< endreq;
    }
    else{ 
      log <<MSG::DEBUG <<"Si Cluster container found" <<endreq; 
    }
    return sc;
    
}

StatusCode ReadSiClusters::FillSiNtuple(){

  //const DataHandle<SiClusterCollection> colNext(m_clusters->begin());
  //const DataHandle<SiClusterCollection> colEnd(m_clusters->end());
  InDet::SiClusterContainer::const_iterator colNext = m_clusters->begin();
  InDet::SiClusterContainer::const_iterator colEnd = m_clusters->end();

  int collectionCount(0);
  int clusterCount(0);
  StatusCode sc;
  for (; colNext!= colEnd; ++colNext)
    {

      InDet::SiClusterCollection::const_iterator nextCluster = (*colNext)->begin();
      InDet::SiClusterCollection::const_iterator lastCluster = (*colNext)->end();
  
      for (; nextCluster!=lastCluster; nextCluster++)
	{
	  const SiCluster& cluster = **nextCluster;
	  m_r = cluster.globalPosition().perp();
	  m_phi = cluster.globalPosition().phi();
	  m_z = cluster.globalPosition().z();
	  const InDet::SiWidth siWidth = cluster.width();
	  //            const CLHEP::Hep2Vector PhiRZwidth = siWidth.widthPhiRZ();
	  const Amg::Vector2D& ColRowWidth = siWidth.colRow();
	  const Amg::MatrixX& cov = cluster.localCovariance();
	  m_phiWidth = siWidth.phiR();
	  m_etaWidth = siWidth.z();
	  m_rowWidth = ColRowWidth.x();
	  m_colWidth = ColRowWidth.y();
	  m_errPhi = Amg::error(cov, Trk::locRPhi);
	  m_errZ = Amg::error(cov, Trk::locZ);
	  StatusCode nsc=ntupleSvc()->writeRecord(p_ntuple); 
	  /*
	  if (nsc==StatusCode::FAILURE) 
	    {
	      log << MSG::ERROR << "Cannot record ntuple !" << endreq; 
	      sc = StatusCode::FAILURE; 
	    }
	  */
	  clusterCount++;
	}
      collectionCount++;
    }
   return sc;

}

