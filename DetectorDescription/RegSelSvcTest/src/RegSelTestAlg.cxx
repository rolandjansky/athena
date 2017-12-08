/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TestTools/ParallelCallTest.h"
#include "RegSelTestAlg.h"



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
  if ( m_mt ) 
    CHECK( mt() );
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


StatusCode RegSelTestAlg::dumpOne( const std::string& message, double eta, const std::vector<DETID>& detectors ) const {

  std::vector<TrigRoiDescriptor> rois;
  generateRoIs( rois, eta, 3.1415, 0.1 );
  ATH_MSG_DEBUG("Dumping Regions selector response for: " << message << " detectors: " << detectors << " with eta: " << eta );

  for ( auto detector: detectors ) {
    for ( auto & roi: rois ) {
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


StatusCode RegSelTestAlg::dump() const {
  // generate list of RoIs for ID 
  
  CHECK( dumpOne("Inner Detector ", 2.5,  { PIXEL, SCT, TRT } ) ); 
  CHECK( dumpOne("Calorimeters ", 5., { LAR, TTEM, TTHEC, TILE, FCALEM, FCALHAD } ) );
  CHECK( dumpOne("Muons ", 5., { MDT, RPC, TGC, CSC } ) );
  return StatusCode::SUCCESS;
}

struct FullDetTest : public ParallelCallTest {
  FullDetTest( DETID detector, 
	      MsgStream& msg,
	      const ServiceHandle<IRegSelSvc>& svc ) 
    : m_det ( detector ), 
      m_msg( msg ),
      m_svc( svc ) {}

  void firstCall() override {
    m_svc->DetHashIDList( m_det, m_refHashes );             
    m_svc->DetROBIDListUint( m_det, m_refROBs );    
  }

  bool callAndCompare() const override {
    std::vector<IdentifierHash> hashes;
    std::vector<uint32_t> ROBs;
    
    m_svc->DetHashIDList( m_det, hashes );             

    if ( hashes != m_refHashes ) {
      m_msg << MSG::ERROR << "List of hashes different for detector ID " << m_det << endmsg;
      m_msg << MSG::ERROR << "Reference: " << m_refHashes << endmsg;
      m_msg << MSG::ERROR << "Obtained: " << hashes << endmsg;
      return false;
    }

    m_svc->DetROBIDListUint( m_det, ROBs );    
    if ( ROBs != m_refROBs ) {
      m_msg << MSG::ERROR << "List of ROBs different for detector ID " << m_det << endmsg;
      m_msg << MSG::ERROR << "Reference: " << m_refROBs << endmsg;
      m_msg << MSG::ERROR <<"Obtained: " << ROBs << endmsg;
      return false;
    }
    return true;
  }

  std::vector<IdentifierHash> m_refHashes;
  std::vector<uint32_t> m_refROBs;

  DETID m_det;
  MsgStream& m_msg;
  const ServiceHandle<IRegSelSvc>& m_svc;



};

struct RoIReqTest : public ParallelCallTest {
  RoIReqTest( const TrigRoiDescriptor& roi, DETID detector, 
	      MsgStream& msg,
	      const ServiceHandle<IRegSelSvc>& svc ) 
    : m_roi( roi ),
      m_det ( detector ), 
      m_msg( msg ),
      m_svc( svc ) {}

  void firstCall() override {
      m_svc->DetHashIDList( m_det, m_roi, m_refHashes );             
      m_svc->DetROBIDListUint( m_det, m_roi, m_refROBs );    
  }

  bool callAndCompare() const override {
    std::vector<IdentifierHash> hashes;
    std::vector<uint32_t> ROBs;
    
    m_svc->DetHashIDList( m_det, m_roi, hashes );             

    if ( hashes != m_refHashes ) {
      m_msg << MSG::ERROR << "List of hashes different for " << m_roi << " detector ID " << m_det << endmsg;
      m_msg << MSG::ERROR << "Reference: " << m_refHashes << endmsg;
      m_msg << MSG::ERROR << "Obtained: " << hashes << endmsg;
      return false;
    }

    m_svc->DetROBIDListUint( m_det, m_roi, ROBs );    
    if ( ROBs != m_refROBs ) {
      m_msg << MSG::ERROR << "List of ROBs different for " << m_roi << " detector ID " << m_det << endmsg;
      m_msg << MSG::ERROR << "Reference: " << m_refROBs << endmsg;
      m_msg << MSG::ERROR <<"Obtained: " << ROBs << endmsg;
      return false;
    }
    return true;
  }

  std::vector<IdentifierHash> m_refHashes;
  std::vector<uint32_t> m_refROBs;


  TrigRoiDescriptor m_roi;
  DETID m_det;
  MsgStream& m_msg;
  const ServiceHandle<IRegSelSvc>& m_svc;
};




StatusCode RegSelTestAlg::mt() const {
  
  // generate whole array of tests
  std::vector<TrigRoiDescriptor> rois;
  generateRoIs( rois, 5, 3.1415, 0.1 );
  
  std::vector<ParallelCallTest*>  tests;

  
  for ( auto detector: {PIXEL, SCT, TRT, LAR, TTEM, TTHEC, TILE, MDT, RPC, TGC, CSC, FCALEM, FCALHAD, FTK} ) {
    for ( auto& roi : rois ) {
      tests.push_back( new RoIReqTest( roi, detector, msg(), m_regSelSvc ) );
    }
    tests.push_back( new FullDetTest( detector, msg(), m_regSelSvc ) );
  }




#ifdef NDEBUG
  int nrepeat = 100;
#else
  int nrepeat = 10;
#endif
  bool status = ParallelCallTest::launchTests( nrepeat, tests );
  
  for ( auto t: tests ) 
    delete t;

  
  if ( status == false ) {
    ATH_MSG_ERROR("MT test failed, see messages above");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
