/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HIGlobalNSiCluster.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/HIGlobalNSiCluster.h"
#include "HIGlobal/EtaPhiBins.h"
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"

#include "InDetIdentifier/PixelID.h"


#include "TrkEventPrimitives/ParamDefs.h"
#include "InDetPrepRawData/SiWidth.h"

#include "TMath.h"



//================ Constructor =================================================

 HIGlobalNSiCluster::HIGlobalNSiCluster(const std::string& name, ISvcLocator* pSvcLocator)
  :
   AthAlgorithm(name,pSvcLocator),
   m_nDet(0),
   m_eventNumber(0),
   m_numberOfEvents(0),
   m_r(0),
   m_phi(0),
   m_z(0),
   m_rowWidth(0),
   m_colWidth(0),
   m_phiWidth(0),
   m_etaWidth(0),
   m_errPhi(0),
   m_errZ(0)
{
  //  template for property declaration
  //declareProperty("PropertyName", m_propertyName);
}

//================ Destructor =================================================

 HIGlobalNSiCluster::~HIGlobalNSiCluster()
{}


//================ Initialisation =================================================

StatusCode  HIGlobalNSiCluster::initialize()
{
  // Code entered here will be executed once at program start.
  
  ATH_MSG_DEBUG( name() << " initialize()" );
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode  HIGlobalNSiCluster::finalize()
{
  // Code entered here will be executed once at the end of the program run.

  ATH_MSG_DEBUG( name() << " finalize  called  !!!!!" );
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

    const DataHandle<EventInfo> eventInfo;

    ATH_CHECK( evtStore()->retrieve(eventInfo) );
    ATH_MSG_DEBUG( "HIG EventInfo retrieved!" );

    m_eventNumber= eventInfo->event_ID()->event_number();
    ATH_MSG_DEBUG( "event number " << m_eventNumber);
    m_numberOfEvents++;


    // get space points from TDS
    const DataHandle<InDet::SiClusterContainer> clusters;
    ATH_CHECK( evtStore()->retrieve(clusters, "PixelClusers") );
    ATH_MSG_DEBUG("HIG  Si Cluster container found" );

  const PixelID* pixelID = nullptr;
  //const InDetDD::SiDetectorManager* geoManager;
  ATH_CHECK( detStore()->retrieve(pixelID,"PixelID") );
  // pixelID = dynamic_cast<const PixelID *>(geoManager->getIdHelper());

  const VxContainer *vxc = nullptr;
  ATH_CHECK( evtStore()->retrieve( vxc, "VxPrimaryCandidate" ) );
  Trk::RecVertex thisVx;
  if (vxc->size() > 0) { thisVx = (*vxc)[0]->recVertex(); }
  //cout << " HIG   vertex position " << thisVx.position().x() << "  "
  //   << thisVx.position().y()  << "  "  << thisVx.position().z() << endl;

  InDet::SiClusterContainer::const_iterator colNext = clusters->begin();
  InDet::SiClusterContainer::const_iterator colEnd = clusters->end();

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
          const InDet::SiCluster& cluster = **nextCluster;
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

  ATH_CHECK( evtStore()->record(cleta0, "NSiClusterL0") );
  ATH_CHECK( evtStore()->record(cleta1, "NSiClusterL1") );
  ATH_CHECK( evtStore()->record(cleta2, "NSiClusterL2") );

  return StatusCode::SUCCESS;
}

//============================================================================

