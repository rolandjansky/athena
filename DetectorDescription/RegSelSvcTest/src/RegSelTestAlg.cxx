/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "RegSelTestAlg.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"


RegSelTestAlg::RegSelTestAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ) {
  //declareProperty( "Property", m_nProperty );

}

RegSelTestAlg::~RegSelTestAlg()
{}

StatusCode RegSelTestAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  CHECK( m_regSelSvc.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode RegSelTestAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode RegSelTestAlg::execute_r(const EventContext& /*context*/) const {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  if ( m_hammer ) 
    CHECK( hammer() );
  if ( m_dump ) 
    CHECK( dump() );
  
  return StatusCode::SUCCESS;
}

void generateRoIs( std::vector<TrigRoiDescriptor>& rois, 
			 double etaRange, double phiRange, double step) {

  for ( double phi = -phiRange; phi < phiRange; phi += step ) 
    for ( double eta = -etaRange; eta < etaRange+0.01; eta += step )
      rois.emplace_back( TrigRoiDescriptor(eta, eta - step, eta + step,
					   phi, phi - step, eta - step ) );
  
}

StatusCode RegSelTestAlg::dump() const {
  // generate list of RoIs for ID 
  std::vector<TrigRoiDescriptor> id_rois;
  generateRoIs( id_rois, 2.5, 3.1415, 0.1 );


  for ( auto detector: {PIXEL, SCT, TRT, FTK} ) {
    for ( auto & roi: id_rois ) {
      ATH_MSG_DEBUG( "Detector " << detector << " and RoI " << roi );
      std::vector<IdentifierHash> hashes;
      m_regSelSvc->DetHashIDList( detector, roi, hashes );
      ATH_MSG_DEBUG( "Hashes " << hashes );
      
      std::vector<uint32_t> robs;
      m_regSelSvc->DetROBIDListUint( detector, roi, robs);
      ATH_MSG_DEBUG( "ROBs " << robs );
    }
  }

  return StatusCode::SUCCESS;
}




StatusCode RegSelTestAlg::hammer() const {
  return StatusCode::SUCCESS;
}
