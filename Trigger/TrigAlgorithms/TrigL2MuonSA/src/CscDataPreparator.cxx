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
   AlgTool(type,name,parent),
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
   msg() << MSG::DEBUG << "Initializing CscDataPreparator - package version " << PACKAGE_VERSION << endreq ;
   
   StatusCode sc;
   sc = AlgTool::initialize();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not initialize the AlgTool base class." << endreq;
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

StatusCode TrigL2MuonSA::CscDataPreparator::prepareData(const LVL1::RecMuonRoI* p_roi,
							//TrigL2MuonSA::MuonRoad& muonRoad,
							double aw, double bw, double rWidth,
							TrigL2MuonSA::CscHits&  cscHits)
{
  StatusCode sc;

  //
  // Region definer, like mdtRegion, is not used for now.
  //

  //sc = getCscHits(p_roi, muonRoad, cscHits);
  sc = getCscHits(p_roi, aw, bw, rWidth, cscHits);
  if( sc!= StatusCode::SUCCESS ) {
    msg() << MSG::ERROR << "Error in getting CSC hits" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::CscDataPreparator::getCscHits(const LVL1::RecMuonRoI* p_roi,
						       //TrigL2MuonSA::MuonRoad& muonRoad,
						       double aw, double bw, double rWidth,
						       TrigL2MuonSA::CscHits&  cscHits)
{
  // Prepare new RoI
  float roi_eta = p_roi->eta();
  float roi_phi = p_roi->phi();
  if (roi_phi < 0) roi_phi += 2.0 * CLHEP::pi;
  double etaMin = p_roi->eta() - 0.2;
  double etaMax = p_roi->eta() + 0.2;
  double phiMin = p_roi->phi() - 0.1;
  double phiMax = p_roi->phi() + 0.1;
  if( phiMin < 0 ) phiMin += 2*CLHEP::pi;
  if( phiMax < 0 ) phiMax += 2*CLHEP::pi;

  TrigRoiDescriptor* newRoI = 0;
  newRoI = new TrigRoiDescriptor(roi_eta, etaMin, etaMax, roi_phi, phiMin, phiMax);

  // Select RoI hits
  std::vector<IdentifierHash> cscHashIDs;
  cscHashIDs.clear();
  m_regionSelector->DetHashIDList( CSC, *newRoI, cscHashIDs );

  // Decode
  std::vector<IdentifierHash> cscHashIDs_decode;
  cscHashIDs_decode.clear();
  if( !cscHashIDs.empty() ){
    if( m_cscPrepDataProvider->decode( cscHashIDs, cscHashIDs_decode ).isFailure() ){
      msg() << MSG::WARNING << "Problems when preparing CSC PrepData" << endreq;
    }
    cscHashIDs.clear();
  }

  // Clustering
  std::vector<IdentifierHash> cscHashIDs_cluster;
  cscHashIDs_cluster.clear();
  if( !cscHashIDs_decode.empty() ){
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
	/*
	double aw = muonRoad.aw[0][0];
	double bw = muonRoad.bw[0][0];
	double rWidth = muonRoad.rWidth[0][0];
	*/

	// Create new digit
	TrigL2MuonSA::CscHitData lutDigit;
	lutDigit.StationName  = m_cscIdHelper->stationName( prepData.identify() );
	lutDigit.StationEta   = m_cscIdHelper->stationEta( prepData.identify() );
	lutDigit.StationPhi   = m_cscIdHelper->stationPhi( prepData.identify() );
	lutDigit.ChamberLayer = m_cscIdHelper->chamberLayer( prepData.identify() );
	lutDigit.WireLayer    = m_cscIdHelper->wireLayer( prepData.identify() );
	lutDigit.MeasuresPhi  = m_cscIdHelper->measuresPhi( prepData.identify() );
	lutDigit.Strip        = m_cscIdHelper->strip( prepData.identify() );
	lutDigit.Chamber      = xAOD::L2MuonParameters::Chamber::CSC;
	lutDigit.eta = prepData.globalPosition().eta();
	lutDigit.phi = prepData.globalPosition().phi();
	lutDigit.r   = prepData.globalPosition().perp();
	lutDigit.z   = prepData.globalPosition().z();
	lutDigit.charge = prepData.charge();
	lutDigit.time   = prepData.time();
	lutDigit.Residual = calc_residual( aw, bw, lutDigit.z, lutDigit.r );
	lutDigit.isOutlier = 0;
	if( fabs(lutDigit.Residual) > rWidth ) {
	  lutDigit.isOutlier = 2;
	}

	cscHits.push_back( lutDigit );

	// Debug print
       	msg() << MSG::DEBUG << "CSC Hits: "
	      << "SN=" << lutDigit.StationName << ","
	      << "SE=" << lutDigit.StationEta << ","
	      << "SP=" << lutDigit.StationPhi << ","
	      << "CL=" << lutDigit.ChamberLayer << ","
	      << "WL=" << lutDigit.WireLayer << ","
	      << "MP=" << lutDigit.MeasuresPhi << ","
	      << "St=" << lutDigit.Strip << ","
	      << "eta=" << lutDigit.eta << ","
	      << "phi=" << lutDigit.phi << ","
	      << "r=" << lutDigit.r << ","
	      << "z=" << lutDigit.z << ","
	      << "q=" << lutDigit.charge << ","
	      << "t=" << lutDigit.time << ","
	      << "Rs=" << lutDigit.Residual << ","
	      << "OL=" << lutDigit.isOutlier << endreq;
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
  if( fabs(aw) < ZERO_LIMIT ) return x-bw;
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
   
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
