/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/CscDataPreparator.h"

#include "GaudiKernel/ToolFactory.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "Identifier/IdentifierHash.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"
#include "xAODTrigMuon/TrigMuonDefs.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"


using namespace Muon;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_CscDataPreparator("IID_CscDataPreparator", 1, 0);
bool IsUnspoiled ( Muon::CscClusterStatus status );


const InterfaceID& TrigL2MuonSA::CscDataPreparator::interfaceID() { return IID_CscDataPreparator; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::CscDataPreparator::CscDataPreparator(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
   AthAlgTool(type,name,parent),
   m_storeGateSvc( "StoreGateSvc", name ),
   p_ActiveStore(0),
   m_regionSelector(0),
   m_cscPrepDataProvider("Muon::CscRdoToCscPrepDataTool/CscPrepDataProviderTool"),
   m_cscClusterProvider("CscThresholdClusterBuilderTool")
{
   declareInterface<TrigL2MuonSA::CscDataPreparator>(this);

   declareProperty("CscPrepDataProvider", m_cscPrepDataProvider);
   declareProperty("CscClusterProvider",  m_cscClusterProvider);
   declareProperty("CSCPrepDataContainer", m_cscPrepContainerKey = std::string("CSC_Clusters"), "CSC_Clusters to read in");
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


  ATH_MSG_DEBUG("Initializing CscDataPreparator - package version " << PACKAGE_VERSION);
   
   StatusCode sc;
   sc = AthAlgTool::initialize();
   if (!sc.isSuccess()) {
     ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
      return sc;
   }
   
   // Locate the StoreGateSvc
   sc =  m_storeGateSvc.retrieve();
   if (!sc.isSuccess()) {
     ATH_MSG_ERROR("Could not find StoreGateSvc");
      return sc;
   }

   // Retrieve ActiveStore
   sc = serviceLocator()->service("ActiveStoreSvc", p_ActiveStore);
   if( !sc.isSuccess() || 0 == p_ActiveStore ){
     ATH_MSG_ERROR(" Could not find ActiveStoreSvc ");
     return sc;
   }

   // prepdataprovider
   sc = m_cscPrepDataProvider.retrieve();
   if (sc.isSuccess()) {
     ATH_MSG_INFO("Retrieved " << m_cscPrepDataProvider);
   } else {
     ATH_MSG_FATAL("Could not get " << m_cscPrepDataProvider);
     return sc;
   }

   // clusterization tool
   sc = m_cscClusterProvider.retrieve();
   if (sc.isSuccess()) {
     ATH_MSG_INFO("Retrieved " << m_cscClusterProvider);
   } else {
     ATH_MSG_FATAL("Could not get " << m_cscClusterProvider);
     return sc;
   }

   // Detector Store
   StoreGateSvc* detStore;
   sc = serviceLocator()->service("DetectorStore", detStore);
   if( sc.isFailure() ){
     ATH_MSG_ERROR("Could not retrieve  DetectorStore.");
     return sc;
   }
   ATH_MSG_DEBUG("Retrieved DetectorStore.");

   // CSC ID helper
   sc = detStore->retrieve( m_muonMgr, "Muon" );
   if( sc.isFailure() ){
     ATH_MSG_ERROR(" Cannot retrieve MuonGeoModel ");
     return sc;
   }
   ATH_MSG_DEBUG("Retrieved GeoModel from DetectorStore.");
   m_cscIdHelper = m_muonMgr->cscIdHelper();

   //
   std::string serviceName;

   // Locate RegionSelector
   serviceName = "RegionSelector";
   sc = service("RegSelSvc", m_regionSelector);
   if(sc.isFailure()) {
     ATH_MSG_ERROR("Could not retrieve " << serviceName);
      return sc;
   }
   ATH_MSG_DEBUG("Retrieved service " << serviceName);

   ATH_CHECK(m_cscPrepContainerKey.initialize());

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
    ATH_MSG_DEBUG("Use RoI based data access");
    m_regionSelector->DetHashIDList( CSC, *iroi, cscHashIDs );
  } else {
    ATH_MSG_DEBUG("Use full data access");
    //    m_regionSelector->DetHashIDList( CSC, cscHashIDs ); full decoding is executed with an empty vector
  }
  ATH_MSG_DEBUG("cscHashIDs.size()=" << cscHashIDs.size());

  bool to_full_decode=( fabs(p_roids->etaMinus())>1.7 || fabs(p_roids->etaPlus())>1.7 ) && !m_use_RoIBasedDataAccess;

  // Decode
  std::vector<IdentifierHash> cscHashIDs_decode;
  cscHashIDs_decode.clear();
  if( !cscHashIDs.empty() || to_full_decode ){
    if( m_cscPrepDataProvider->decode( cscHashIDs, cscHashIDs_decode ).isFailure() ){
      ATH_MSG_WARNING("Problems when preparing CSC PrepData");
    }
    cscHashIDs.clear();
  }
  ATH_MSG_DEBUG("cscHashIDs_decode.size()=" << cscHashIDs_decode.size());

  // Clustering
  std::vector<IdentifierHash> cscHashIDs_cluster;
  cscHashIDs_cluster.clear();
  if(to_full_decode) cscHashIDs_decode.clear();
  if( !cscHashIDs_decode.empty() || to_full_decode ){
    if( m_cscClusterProvider->getClusters( cscHashIDs_decode, cscHashIDs_cluster ).isFailure() ){
      ATH_MSG_WARNING("Problems when preparing CSC Clusters");
    }
    cscHashIDs_decode.clear();
  }

  // Debug info
  ATH_MSG_DEBUG("CSC cluster #hash = " << cscHashIDs_cluster.size());

  // Clear the output
  cscHits.clear();

  // Get CSC container
  if( !cscHashIDs_cluster.empty() ){
    auto cscPrepContainerHandle = SG::makeHandle(m_cscPrepContainerKey);
    const CscPrepDataContainer* cscPrepContainer = cscPrepContainerHandle.cptr();
    if (!cscPrepContainerHandle.isValid()) {
      ATH_MSG_ERROR("Cannot retrieve CSC PRD Container key: " << m_cscPrepContainerKey.key());
      return StatusCode::FAILURE;
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
	//double rWidth = muonRoad.rWidth[chamber][0];
	double phiw = muonRoad.phi[4][0];//roi_descriptor->phi(); //muonRoad.phi[chamber][0];

	//cluster status
	bool isunspoiled = IsUnspoiled (prepData.status());


	// Create new digit
	TrigL2MuonSA::CscHitData cscHit;
	cscHit.StationName  = m_cscIdHelper->stationName( prepData.identify() );
	cscHit.StationEta   = m_cscIdHelper->stationEta( prepData.identify() );
	cscHit.StationPhi   = m_cscIdHelper->stationPhi( prepData.identify() );
	cscHit.ChamberLayer = (true==isunspoiled) ? 1 : 0;//m_cscIdHelper->chamberLayer( prepData.identify() );
	cscHit.WireLayer    = m_cscIdHelper->wireLayer( prepData.identify() );
	cscHit.MeasuresPhi  = m_cscIdHelper->measuresPhi( prepData.identify() );
	cscHit.Strip        = m_cscIdHelper->strip( prepData.identify() );
	cscHit.Chamber      = chamber;
	cscHit.StripId = (cscHit.StationName << 18)
	  | ((cscHit.StationEta + 2) << 16) | (cscHit.StationPhi << 12)
	  | (cscHit.WireLayer << 9) | (cscHit.MeasuresPhi << 8) | (cscHit.Strip);
	cscHit.eta = sqrt( prepData.localCovariance()(0,0) );
	cscHit.phi = prepData.globalPosition().phi();
	cscHit.r   = prepData.globalPosition().perp();
	cscHit.z   = prepData.globalPosition().z();
	cscHit.charge = prepData.charge();
	cscHit.time   = prepData.time();
	cscHit.Residual =  ( cscHit.MeasuresPhi==0 ) ? calc_residual( aw, bw, cscHit.z, cscHit.r ) : calc_residual_phi( aw,bw,phiw, cscHit.phi, cscHit.z);
	cscHit.isOutlier = 0;
	/*if( fabs(cscHit.Residual) > rWidth ) {
	  cscHit.isOutlier = 2;
	  }*/
	double width = (cscHit.MeasuresPhi) ? 250. : 100.;
	if( fabs(cscHit.Residual)>width ){
	  cscHit.isOutlier=1;
	  if( fabs(cscHit.Residual)>3.*width ){
	    cscHit.isOutlier=2;
	  }
	}
	
	cscHits.push_back( cscHit );
	
	// Debug print
       	ATH_MSG_DEBUG("CSC Hits: "
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
		      << "OL="  << cscHit.isOutlier);
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


double TrigL2MuonSA::CscDataPreparator::calc_residual_phi( double aw, double bw, double phiw, double hitphi, double hitz){

  double roadr = hitz*aw + bw;

  return roadr*( cos(phiw)*sin(hitphi)-sin(phiw)*cos(hitphi) );

}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::CscDataPreparator::finalize()
{
  ATH_MSG_DEBUG("Finalizing CscDataPreparator - package version " << PACKAGE_VERSION);
   
   StatusCode sc = AthAlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------


bool IsUnspoiled ( Muon::CscClusterStatus status ) {
  if (status == Muon::CscStatusUnspoiled || status == Muon::CscStatusSplitUnspoiled )
    return true;

  return false;
}
