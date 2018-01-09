/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include "tbb/parallel_reduce.h"
#include "tbb/blocked_range.h"
#include "TestTools/expect.h"
#include "TestTools/ParallelCallTest.h"
#include "GaudiKernel/Property.h"
#include "TrigT2CaloCommon/LArCellCont.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TestCaloDataAccess.h"


#define DIFF(_name, _a, _b) if ( _a != _b ) \
    m_msg << MSG::WARNING << "Difference in " << _name << " " << _a << "  ref " << _b  << endmsg;  \
  else									\
    m_msg << MSG::DEBUG  << "Identical " << _name << " " << _a << "  ref " << _b  << endmsg; 


/**
 * @brief The test calls for RoI data access
 * for each RoI returned bunch of quantiries are checked, RoI et, actual RoI span, and cells count
 **/

class AskForRoI : public ParallelCallTest {
public:
  AskForRoI( const EventContext& context, const ServiceHandle<ITrigCaloDataAccessSvc>& svc,  	     
	     MsgStream& msg,
	     double eta, double phi, double width ) 
    : m_context( context ),
      m_svc( svc ),
      m_msg( msg ),
      m_eta( eta ),
      m_phi( phi ),
      m_width( width ) {}

  ITrigCaloDataAccessSvc::Status request( LArTT_Selector<LArCellCont>& sel ) const {
      TrigRoiDescriptor roi( m_eta, -m_width, m_width, // eta
			     m_phi, -m_width, m_width, // phi
			     0 ); // z            

      return m_svc->loadCollections( m_context, roi, sel );    
    }

  // calculate reference quantities in the first call
  void firstCall() override { 
    m_statusRef = request( m_selRef );

    for ( const LArCell* cell : m_selRef ) {
      m_etSumRef += cell->et();
      m_countRef ++;
      m_minEtaRef = std::min( m_minEtaRef, cell->eta() );
      m_maxEtaRef = std::max( m_maxEtaRef, cell->eta() );
      m_minPhiRef = std::min( m_minPhiRef, cell->phi() );
      m_maxPhiRef = std::max( m_maxPhiRef, cell->phi() );
    }
  }
  
  bool callAndCompare() const override {

    LArTT_Selector<LArCellCont> sel;
    ITrigCaloDataAccessSvc::Status status = request( sel );      
    double etSum  = 0;      
    size_t count = 0;
    double minEta = 100;
    double maxEta = -100;
    double minPhi = 100;
    double maxPhi = -100;

    for ( const LArCell* cell : sel ) {
      etSum  += cell->et();
      count ++;
      minEta  = std::min( minEta, cell->eta() );
      maxEta  = std::max( maxEta, cell->eta() );
      minPhi  = std::min( minPhi, cell->phi() );
      maxPhi  = std::max( maxPhi, cell->phi() );
    }


    DIFF( "RoI mask", status.mask(), m_statusRef.mask() );    
    DIFF( "RoI count ", count , m_countRef );
    DIFF( "RoI etSum ", etSum , m_etSumRef );
    DIFF( "RoI minEta", minEta, m_minEtaRef );
    DIFF( "RoI maxEta", maxEta, m_maxEtaRef );
    DIFF( "RoI minPhi", minPhi, m_minPhiRef );
    DIFF( "RoI maxPhi", maxPhi, m_maxPhiRef );
    
    bool checkStatus = m_statusRef.mask() == status.mask()
      and m_countRef == count
      and m_etSumRef == etSum 
      and m_minEtaRef == minEta
      and m_maxEtaRef == maxEta
      and m_minPhiRef == minPhi
      and m_maxPhiRef == maxPhi;

    if ( checkStatus == false ) {
      
      // iterate over two slectors and compare cell by cell
      for ( LArTT_Selector<LArCellCont>::const_iterator refIter = m_selRef.begin(), thisIter = sel.begin(); 
	    refIter != m_selRef.end() and thisIter != sel.end(); ++refIter, ++thisIter ) {	
	const LArCell* refCell = *refIter;
	const LArCell* thisCell = *thisIter;
	if ( thisCell->et() != refCell->et() ) {
	  m_msg << MSG::WARNING <<"eta/phi/et Reference cell " << refCell->eta() << "/" << refCell->phi() << "/" << refCell->et() 
		<<    " differ from the one in this request " << thisCell->eta() << "/" << thisCell->phi() << "/" << thisCell->et() << endmsg;
	}
      }
    }

    return checkStatus;
  }

private:
  const EventContext& m_context;
  const ServiceHandle<ITrigCaloDataAccessSvc>& m_svc;
  MsgStream& m_msg;
  MsgStream& msg(){ return m_msg; }
  const double m_eta;
  const double m_phi;
  const double m_width;

  LArTT_Selector<LArCellCont> m_selRef;
  ITrigCaloDataAccessSvc::Status m_statusRef;
  double m_etSumRef = 0;
  size_t m_countRef = 0;
  double m_minEtaRef = 100;
  double m_maxEtaRef = -100;
  double m_minPhiRef = 100;
  double m_maxPhiRef = -100;
};

/**
 * @brief This test reads entire LAr subdetectors and checks if repeated calls point to the same cells (i.e. by total energy sum)
 **/
class TestFullLAr : public ParallelCallTest {
public:
  TestFullLAr( const EventContext& context, const ServiceHandle<ITrigCaloDataAccessSvc>& svc,  	     
	       MsgStream& msg, DETID detid )
    : m_context( context ),
      m_svc( svc ),
      m_msg( msg ),
      m_detid( detid ){}

  void firstCall() override { 

    LArTT_Selector<LArCellCont>::const_iterator i;
    LArTT_Selector<LArCellCont>::const_iterator end;
    m_statusRef = m_svc->loadFullCollections( m_context, m_detid, i, end);

    for ( ; i != end; ++i ) {
      const LArCell* cell =  *i;
      m_etSumRef += cell->et();
      m_countRef ++;      
    }
  }
  
  
  bool callAndCompare() const override {
    LArTT_Selector<LArCellCont>::const_iterator i;
    LArTT_Selector<LArCellCont>::const_iterator end;
    ITrigCaloDataAccessSvc::Status status = m_svc->loadFullCollections( m_context, m_detid, i, end);
    double etSum = 0;
    size_t count = 0;

    for ( ; i != end; ++i ) {
      const LArCell* cell = *i;
      etSum += cell->et();
      count ++;      
    }

    DIFF( "loadFull mask", status.mask(), m_statusRef.mask() );    
    DIFF( "loadFull count ", count , m_countRef );
    DIFF( "loadFull etSum ", etSum , m_etSumRef );

    return status.mask() == m_statusRef.mask() 
      and count == m_countRef
      and etSum == m_etSumRef;    
  }

    private:
  const EventContext& m_context;
  const ServiceHandle<ITrigCaloDataAccessSvc>& m_svc;
  MsgStream& m_msg;
  const DETID m_detid;

  // reference values
  ITrigCaloDataAccessSvc::Status m_statusRef;
  double m_etSumRef = 0;
  size_t m_countRef = 0;  
};



TestCaloDataAccess::TestCaloDataAccess( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ),
  m_dataAccessSvc( "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc", name )
{}

TestCaloDataAccess::~TestCaloDataAccess() {}

StatusCode TestCaloDataAccess::initialize() {
  CHECK( m_dataAccessSvc.retrieve() );
  return StatusCode::SUCCESS;
}



StatusCode TestCaloDataAccess::execute_r( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  
  AskForRoI t1( context, m_dataAccessSvc, msg(), 0.7, 0.1, 0.1);
  AskForRoI t2( context, m_dataAccessSvc, msg(), 0.7, 0.1, 0.1);  // second identical request
  AskForRoI t3( context, m_dataAccessSvc, msg(), 0.8, 0.2, 0.2);  // overlaping request
  AskForRoI t4( context, m_dataAccessSvc, msg(), -1.7, 2.1, 0.4);  // bigger request
  AskForRoI t5( context, m_dataAccessSvc, msg(), -1., 1.1, 1.4);  // large request with overlap

  TestFullLAr f1( context, m_dataAccessSvc, msg(), TTEM );
  TestFullLAr f2( context, m_dataAccessSvc, msg(), TTEM ); // ask twice for this part
  TestFullLAr f3( context, m_dataAccessSvc, msg(), TTHEC ); 
  TestFullLAr f4( context, m_dataAccessSvc, msg(), FCALEM ); 
  TestFullLAr f5( context, m_dataAccessSvc, msg(), FCALHAD ); 
  TestFullLAr f6( context, m_dataAccessSvc, msg(), FCALHAD ); 

    
  bool result = ParallelCallTest::launchTests( 100, 
					       { &t1, &t2, &t3, &t4, &t5,
						   &f1, &f2, &f3, &f4, &f5, &f6
						   } 
					       );
  
  if ( result == false ) {
    ATH_MSG_ERROR( "Test of data access failed " );
    return StatusCode::FAILURE;
  } 


  return StatusCode::SUCCESS;
}


