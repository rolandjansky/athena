/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HIGlobalNSiCluster.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/HIGlobalNSiCluster.h"
#include "StoreGate/StoreGateSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include <fstream>

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGate.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"

#include "InDetIdentifier/PixelID.h"


#include "TrkEventPrimitives/ParamDefs.h"
#include "InDetPrepRawData/SiWidth.h"

#include "GaudiKernel/AlgFactory.h"

#include "TMath.h"

#include <iostream>
using std::cout;using std::endl;


//================ Constructor =================================================

 HIGlobalNSiCluster::HIGlobalNSiCluster(const std::string& name, ISvcLocator* pSvcLocator)
  :
  Algorithm(name,pSvcLocator),
  m_log(msgSvc(),name)
{
  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);

  //cout << " HIGlobalNSiCluster::creator    !!!!! " << endl;

}

//================ Destructor =================================================

 HIGlobalNSiCluster::~HIGlobalNSiCluster()
{}


//================ Initialisation =================================================

StatusCode  HIGlobalNSiCluster::initialize()
{
  // Code entered here will be executed once at program start.
  
  m_log.setLevel(outputLevel());
  m_log << MSG::DEBUG << name() << " initialize()" << endreq;

  // retrieve the StoreGate Service (delete if not needed)

  StatusCode sc= service("StoreGateSvc",m_sgSvc);
  if (sc.isFailure()) {
    m_log << MSG::FATAL << "StoreGate service not found !" << endreq;
    return StatusCode::FAILURE;
  } else {}

  m_log << MSG::DEBUG << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode  HIGlobalNSiCluster::finalize()
{
  // Code entered here will be executed once at the end of the program run.

  m_log.setLevel(outputLevel());
  m_log << MSG::DEBUG << name() << " finalize  called  !!!!!" << endreq;
  //cout << " HIGlobalNSiCluster::finalize    !!!!! " << endl;
  //cout << " HIGlobalNSiCluster::finalize    !!!!! " << endl;
  //cout << " HIGlobalNSiCluster::finalize    !!!!! " << endl;

  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode  HIGlobalNSiCluster::execute()
{
  // Code entered here will be executed once per event

  //cout << " HIGlobalNSiCluster::execute() " << endl;
  //cout << " HIGlobalNSiCluster::execute    !!!!! " << endl;

    StatusCode sc = StatusCode::SUCCESS;

    const DataHandle<EventInfo> eventInfo;

    sc = (m_sgSvc->retrieve(eventInfo));
    if (sc.isFailure()){
        m_log << MSG::FATAL << "could not get event"<<endreq;
        return StatusCode::FAILURE;
    }
    m_log << MSG::DEBUG << "HIG EventInfo retrieved!" << endreq;
    //cout << "HIG EventInfo retrieved!" <<  endl;

    m_eventNumber= eventInfo->event_ID()->event_number();
    m_log<< MSG::DEBUG<< "event number " << m_eventNumber<<endreq;
    m_numberOfEvents++;


    // get space points from TDS
    m_SiClustersName="PixelClusters";
    sc = m_sgSvc->retrieve(m_clusters, m_SiClustersName);
    if (sc.isFailure()){
      m_log << MSG::ERROR <<"HIG Si cluster container not found"<< endreq;
    }
    else{
      //cout << " HIG   Si Cluster container found" << endl;
      m_log <<MSG::DEBUG <<"HIG  Si Cluster container found" <<endreq;
    }
    if(sc == StatusCode::FAILURE) return sc;

  // get the det store
  StoreGateSvc *detStore;
  sc = service("DetectorStore",detStore);
  if (sc.isFailure()) {
    m_log << MSG::FATAL << "Detector store not found !" << endreq;
    return StatusCode::FAILURE;
  } else {}

  const PixelID* pixelID;
  //const InDetDD::SiDetectorManager* geoManager;
  sc=detStore->retrieve(pixelID,"PixelID");
  // pixelID = dynamic_cast<const PixelID *>(geoManager->getIdHelper());

  const VxContainer *vxc;
  Trk::RecVertex thisVx;
  sc = m_sgSvc->retrieve( vxc, "VxPrimaryCandidate" );
  if (vxc->size() > 0) { thisVx = (*vxc)[0]->recVertex(); }
  //cout << " HIG   vertex position " << thisVx.position().x() << "  "
  //   << thisVx.position().y()  << "  "  << thisVx.position().z() << endl;

  InDet::SiClusterContainer::const_iterator colNext = m_clusters->begin();
  InDet::SiClusterContainer::const_iterator colEnd = m_clusters->end();

  float cx,cy,cz;
  float phi, theta, eta, r;
  int layer;

  EtaPhiBins* cleta0 = new EtaPhiBins("NSiClusterL0", -3.0, 3.0, 60, 16);
//  TH2D *hcleta0 = cleta0->GetH2();
  EtaPhiBins* cleta1 = new EtaPhiBins("NSiClusterL1", -3.0, 3.0, 60, 16);
//  TH2D *hcleta1 = cleta1->GetH2();
  EtaPhiBins* cleta2 = new EtaPhiBins("NSiClusterL2", -3.0, 3.0, 60, 16);
//  TH2D *hcleta2 = cleta2->GetH2();

  int collectionCount(0);
  int clusterCount(0);
  for (; colNext!= colEnd; ++colNext)
    {

      InDet::SiClusterCollection::const_iterator nextCluster = (*colNext)->begin();
      InDet::SiClusterCollection::const_iterator lastCluster = (*colNext)->end();

      //cout << " HIG collection = " << collectionCount << endl;

      for (; nextCluster!=lastCluster; nextCluster++)
        {
          const SiCluster& cluster = **nextCluster;
          cx = cluster.globalPosition().x() - thisVx.position().x();
          cy = cluster.globalPosition().y() - thisVx.position().y();
          cz = cluster.globalPosition().z() - thisVx.position().z();
          phi = atan2(cy,cx);
          r = sqrt(cx*cx+cy*cy);
          theta = atan2(r,cz);
          eta = -TMath::Log(TMath::Tan(theta/2));
          layer = pixelID->layer_disk(cluster.detectorElement()->identify());

          if(layer == 0)  cleta0->Fill(eta, phi);
          if(layer == 1)  cleta1->Fill(eta, phi);
          if(layer == 2)  cleta2->Fill(eta, phi);


	  // cout << " HIG cluster " << layer << "  " << r << "  " << phi << "  "
	  // << eta << "  " << cx << "  " << cy << "  " << cz << endl;

          clusterCount++;
        }
      collectionCount++;
    }

    sc = m_sgSvc->record(cleta0, "NSiClusterL0");
     if( sc.isFailure() ) m_log << MSG::ERROR << "Could not save NSiClusterL0" <<endreq;
    sc = m_sgSvc->record(cleta1, "NSiClusterL1");
     if( sc.isFailure() ) m_log << MSG::ERROR << "Could not save NSiClusterL1" <<endreq;
    sc = m_sgSvc->record(cleta2, "NSiClusterL2");
     if( sc.isFailure() ) m_log << MSG::ERROR << "Could not save NSiClusterL2" <<endreq;


     // cout << "GetBinContent   30   " << cleta0->GetBinContent(30, 1) <<
     //    "  " << cleta1->GetBinContent(30, 1) << "  " << cleta2->GetBinContent(30, 1) << endl;

     // cout << " HIG collectionCount = " << collectionCount << endl;

  return StatusCode::SUCCESS;
}

//============================================================================================

