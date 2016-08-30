/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/CscDataPreparator.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "Identifier/IdentifierHash.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"
#include "xAODTrigMuon/TrigMuonDefs.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

using namespace Muon;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_CscDataPreparator("IID_CscDataPreparator", 1, 0);

const InterfaceID& TrigL2MuonSA::CscDataPreparator::interfaceID() { return IID_CscDataPreparator; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::CscDataPreparator::CscDataPreparator(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
   AthAlgTool(type,name,parent),
   m_msg(0),
   m_storeGateSvc( "StoreGateSvc", name ),
   p_ActiveStore(0),
   m_regionSelector(0),
   m_cscPrepDataProvider("Muon::CscRdoToCscPrepDataTool/CscPrepDataProviderTool"),
   m_cscClusterProvider("CscThresholdClusterBuilderTool")
{
   declareInterface<TrigL2MuonSA::CscDataPreparator>(this);

   declareProperty("CscPrepDataProvider", m_cscPrepDataProvider);
   declareProperty("CscClusterProvider",  m_cscClusterProvider);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::CscDataPreparator::~CscDataPreparator() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::CscDataPreparator::initialize()
{

   // Get a message stream instance
   m_msg = new MsgStream( msgSvc(), name() );
   msg().setLevel(MSG::INFO);
   msg() << MSG::DEBUG << "Initializing CscDataPreparator - package version " << PACKAGE_VERSION << endreq ;
   
   StatusCode sc;
   sc = AthAlgTool::initialize();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not initialize the AthAlgTool base class." << endreq;
      return sc;
   }
   
   // Locate the StoreGateSvc
   sc =  m_storeGateSvc.retrieve();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      return sc;
   }

   // Retrieve ActiveStore
   sc = serviceLocator()->service("ActiveStoreSvc", p_ActiveStore);
   if( !sc.isSuccess() || 0 == p_ActiveStore ){
     msg() << MSG::ERROR << " Could not find ActiveStoreSvc " << endreq;
     return sc;
   }

   // prepdataprovider
   sc = m_cscPrepDataProvider.retrieve();
   if (sc.isSuccess()) {
     msg() << MSG::INFO << "Retrieved " << m_cscPrepDataProvider << endreq;
   } else {
     msg() << MSG::FATAL << "Could not get " << m_cscPrepDataProvider << endreq;
     return sc;
   }

   // clusterization tool
   sc = m_cscClusterProvider.retrieve();
   if (sc.isSuccess()) {
     msg() << MSG::INFO << "Retrieved " << m_cscClusterProvider << endreq;
   } else {
     msg() << MSG::FATAL << "Could not get " << m_cscClusterProvider << endreq;
     return sc;
   }

   // Detector Store
   StoreGateSvc* detStore;
   sc = serviceLocator()->service("DetectorStore", detStore);
   if( sc.isFailure() ){
     msg() << MSG::ERROR << "Could not retrieve  DetectorStore." << endreq;
     return sc;
   }
   msg() << MSG::DEBUG << "Retrieved DetectorStore." << endreq;

   // CSC ID helper
   sc = detStore->retrieve( m_muonMgr, "Muon" );
   if( sc.isFailure() ){
     msg() << MSG::ERROR << " Cannot retrieve MuonGeoModel " << endreq;
     return sc;
   }
   msg() << MSG::DEBUG << "Retrieved GeoModel from DetectorStore." << endreq;
   m_cscIdHelper = m_muonMgr->cscIdHelper();

   //
   std::string serviceName;

   // Locate RegionSelector
   serviceName = "RegionSelector";
   sc = service("RegSelSvc", m_regionSelector);
   if(sc.isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve " << serviceName << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved service " << serviceName << endreq;

   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::CscDataPreparator::setRoIBasedDataAccess(bool use_RoIBasedDataAccess)
{
  m_use_RoIBasedDataAccess = use_RoIBasedDataAccess;
  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::CscDataPreparator::prepareData(const TrigRoiDescriptor* p_roids,
							TrigL2MuonSA::MuonRoad& muonRoad,
							TrigL2MuonSA::CscHits&  cscHits)
{
  const IRoiDescriptor* iroi = (IRoiDescriptor*) p_roids;

  // Select RoI hits
  std::vector<IdentifierHash> cscHashIDs;
  cscHashIDs.clear();
  if (m_use_RoIBasedDataAccess) {
    msg() << MSG::DEBUG << "Use RoI based data access" << endreq;
    m_regionSelector->DetHashIDList( CSC, *iroi, cscHashIDs );
  } else {
    msg() << MSG::DEBUG << "Use full data access" << endreq;
    //    m_regionSelector->DetHashIDList( CSC, cscHashIDs ); full decoding is executed with an empty vector
  }
  msg() << MSG::DEBUG << "cscHashIDs.size()=" << cscHashIDs.size() << endreq;

  bool to_full_decode=( fabs(p_roids->etaMinus())>1.7 || fabs(p_roids->etaPlus())>1.7 ) && !m_use_RoIBasedDataAccess;

  // Decode
  std::vector<IdentifierHash> cscHashIDs_decode;
  cscHashIDs_decode.clear();
  if( !cscHashIDs.empty() || to_full_decode ){
    if( m_cscPrepDataProvider->decode( cscHashIDs, cscHashIDs_decode ).isFailure() ){
      msg() << MSG::WARNING << "Problems when preparing CSC PrepData" << endreq;
    }
    cscHashIDs.clear();
  }
  msg() << MSG::DEBUG << "cscHashIDs_decode.size()=" << cscHashIDs_decode.size() << endreq;

  // Clustering
  std::vector<IdentifierHash> cscHashIDs_cluster;
  cscHashIDs_cluster.clear();
  if(to_full_decode) cscHashIDs_decode.clear();
  if( !cscHashIDs_decode.empty() || to_full_decode ){
    if( m_cscClusterProvider->getClusters( cscHashIDs_decode, cscHashIDs_cluster ).isFailure() ){
      msg() << MSG::WARNING << "Problems when preparing CSC Clusters" << endreq;
    }
    cscHashIDs_decode.clear();
  }

  // Debug info
  msg() << MSG::DEBUG << "CSC cluster #hash = " << cscHashIDs_cluster.size() << endreq;

  // Clear the output
  cscHits.clear();

  // Get CSC container
  if( !cscHashIDs_cluster.empty() ){
    const CscPrepDataContainer* cscPrepContainer = 0;
    StatusCode sc = (*p_ActiveStore)->retrieve( cscPrepContainer, "CSC_Clusters" );
    if( sc.isFailure() ){
      msg() << MSG::ERROR << " Cannot retrieve CSC PRD Container " << endreq;
      return sc;
    }
    
    // Loop over collections
    CscPrepDataContainer::const_iterator it = cscPrepContainer->begin();
    CscPrepDataContainer::const_iterator it_end = cscPrepContainer->end();
    for( ; it != it_end; ++it ){
      const Muon::CscPrepDataCollection* col = *it;
      if( !col ) continue;

      // Loop over data in the collection
      Muon::CscPrepDataCollection::const_iterator cit = col->begin();
      Muon::CscPrepDataCollection::const_iterator cit_end = col->end();
      for( ; cit != cit_end; ++cit ){
	if( !*cit ) continue;

	// Data in the collection
	const Muon::CscPrepData& prepData = **cit;

	// Road info
	int chamber = xAOD::L2MuonParameters::Chamber::CSC;
	double aw = muonRoad.aw[chamber][0];
	double bw = muonRoad.bw[chamber][0];
	double rWidth = muonRoad.rWidth[chamber][0];

	// Create new digit
	TrigL2MuonSA::CscHitData cscHit;
	cscHit.StationName  = m_cscIdHelper->stationName( prepData.identify() );
	cscHit.StationEta   = m_cscIdHelper->stationEta( prepData.identify() );
	cscHit.StationPhi   = m_cscIdHelper->stationPhi( prepData.identify() );
	cscHit.ChamberLayer = m_cscIdHelper->chamberLayer( prepData.identify() );
	cscHit.WireLayer    = m_cscIdHelper->wireLayer( prepData.identify() );
	cscHit.MeasuresPhi  = m_cscIdHelper->measuresPhi( prepData.identify() );
	cscHit.Strip        = m_cscIdHelper->strip( prepData.identify() );
	cscHit.Chamber      = chamber;
	cscHit.StripId = (cscHit.StationName << 18)
	  | ((cscHit.StationEta + 2) << 16) | (cscHit.StationPhi << 12)
	  | (cscHit.WireLayer << 9) | (cscHit.MeasuresPhi << 8) | (cscHit.Strip);
	cscHit.eta = prepData.globalPosition().eta();
	cscHit.phi = prepData.globalPosition().phi();
	cscHit.r   = prepData.globalPosition().perp();
	cscHit.z   = prepData.globalPosition().z();
	cscHit.charge = prepData.charge();
	cscHit.time   = prepData.time();
	cscHit.Residual = calc_residual( aw, bw, cscHit.z, cscHit.r );
	cscHit.isOutlier = 0;
	if( fabs(cscHit.Residual) > rWidth ) {
	  cscHit.isOutlier = 2;
	}

	cscHits.push_back( cscHit );

	// Debug print
       	msg() << MSG::DEBUG << "CSC Hits: "
	      << "SN="  << cscHit.StationName << ","
	      << "SE="  << cscHit.StationEta << ","
	      << "SP="  << cscHit.StationPhi << ","
	      << "CL="  << cscHit.ChamberLayer << ","
	      << "WL="  << cscHit.WireLayer << ","
	      << "MP="  << cscHit.MeasuresPhi << ","
	      << "St="  << cscHit.Strip << ","
	      << "ID="  << cscHit.StripId << ","
	      << "eta=" << cscHit.eta << ","
	      << "phi=" << cscHit.phi << ","
	      << "r="   << cscHit.r << ","
	      << "z="   << cscHit.z << ","
	      << "q="   << cscHit.charge << ","
	      << "t="   << cscHit.time << ","
	      << "Rs="  << cscHit.Residual << ","
	      << "OL="  << cscHit.isOutlier << endreq;
      }
    }
  }
  
  //
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::CscDataPreparator::calc_residual(double aw,double bw,double x,double y)
{
  const double ZERO_LIMIT = 1e-4;
  if( fabs(aw) < ZERO_LIMIT ) return y-bw;
  double ia  = 1/aw;
  double iaq = ia*ia;
  double dz  = x - (y-bw)*ia;
  return dz/sqrt(1.+iaq);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::CscDataPreparator::finalize()
{
   msg() << MSG::DEBUG << "Finalizing CscDataPreparator - package version " << PACKAGE_VERSION << endreq;
   
   // delete message stream
   if ( m_msg ) delete m_msg;
   
   StatusCode sc = AthAlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
