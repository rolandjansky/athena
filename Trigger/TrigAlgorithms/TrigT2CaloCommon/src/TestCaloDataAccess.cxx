/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include "tbb/parallel_reduce.h"
#include "tbb/blocked_range.h"
#include "TestTools/expect.h"
#include "TestTools/ParallelCallTest.h"
#include "GaudiKernel/Property.h"
#include "TrigT2CaloCommon/LArCellCont.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "TestCaloDataAccess.h"
#include <sys/time.h>

// little code to generate random numbers
// in gaussian form instead of uniform distribution
// used to generate RoIs. mu=mediam; sigma;
double randn (double mu, double sigma)
{
  double U1, U2, W, mult;
  static double X1, X2;
  static int call = 0;
 
  if (call == 1)
    {
      call = !call;
      return (mu + sigma * (double) X2);
    }
 
  do
    {
      U1 = -1 + ((double) rand () / RAND_MAX) * 2;
      U2 = -1 + ((double) rand () / RAND_MAX) * 2;
      W = pow (U1, 2) + pow (U2, 2);
    }
  while (W >= 1 || W == 0);
 
  mult = sqrt ((-2 * log (W)) / W);
  X1 = U1 * mult;
  X2 = U2 * mult;
 
  call = !call;
 
  return (mu + sigma * (double) X1);
}


#define DIFF(_name, _a, _b) if ( _a != _b )				\
    m_msg << MSG::WARNING << "Difference in " << _name << " " << _a << "  ref " << _b  << endmsg; \
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
	     const TrigRoiDescriptor& roi ) 
    : m_context( context ),
      m_svc( svc ),
      m_msg( msg ),
      m_roi ( roi ) {
    m_colRef = new ConstDataVector<CaloCellContainer>(SG::VIEW_ELEMENTS);
  }
  ~AskForRoI() {
    if ( m_colRef ) { m_colRef->clear(); delete m_colRef; }
  }

  StatusCode request( LArTT_Selector<LArCellCont>& sel ) const {
    if ( m_roi.isFullscan() ){
      std::cout << "wrong RoI descriptor used for RoI" << std::endl;
      return StatusCode::FAILURE;
    }
    else{
      // keep this for test reasons
      //usleep (5000);
      return m_svc->loadCollections( m_context, m_roi, TTEM, 2, sel );    
    }
  }

  StatusCode request( ConstDataVector<CaloCellContainer>& c ) const {
    if ( m_roi.isFullscan() ){
      return m_svc->loadFullCollections( m_context, c );
    }
    else{
      std::cout << "wrong RoI descriptor used for FS" << std::endl;
      return StatusCode::FAILURE;
    }
  }

  // calculate reference quantities in the first call
  void firstCall() override { 
    if ( m_roi.isFullscan() ) {
      struct timeval t1,t2;
      gettimeofday(&t1,NULL);
      m_statusRef = request( *m_colRef );
      m_statusRef.ignore(); 
      gettimeofday(&t2,NULL);
      
      for ( const auto cell : *m_colRef ) {
	if ( !cell ) continue;
	m_etSumRef += cell->et();
	m_countRef ++;
	m_minEtaRef = std::min( m_minEtaRef, cell->eta() );
	m_maxEtaRef = std::max( m_maxEtaRef, cell->eta() );
	m_minPhiRef = std::min( m_minPhiRef, cell->phi() );
	m_maxPhiRef = std::max( m_maxPhiRef, cell->phi() );
      }
      std::cout << "t lFC : " << m_context << " " << m_etSumRef << " " << t1.tv_sec << " " << t1.tv_usec << " " << t2.tv_sec << " " << t2.tv_usec << " " << ((t2.tv_sec-t1.tv_sec)*1e6+(t2.tv_usec-t1.tv_usec) )*1e-6 << std::endl;
      
    } else {
      
      struct timeval t1,t2;
      gettimeofday(&t1,NULL);
      m_statusRef = request( m_selRef );
      m_statusRef.ignore();
      gettimeofday(&t2,NULL);
      
      for ( const auto cell : m_selRef ) {
	m_etSumRef += cell->et();
	m_countRef ++;
	m_minEtaRef = std::min( m_minEtaRef, cell->eta() );
	m_maxEtaRef = std::max( m_maxEtaRef, cell->eta() );
	m_minPhiRef = std::min( m_minPhiRef, cell->phi() );
	m_maxPhiRef = std::max( m_maxPhiRef, cell->phi() );
      }
      std::cout << "t RoI : " << m_context << " " << m_etSumRef << " " << t1.tv_sec << " " << t1.tv_usec << " " << t2.tv_sec << " " << t2.tv_usec << " " << ((t2.tv_sec-t1.tv_sec)*1e6+(t2.tv_usec-t1.tv_usec) )*1e-6 << std::endl;
    }
  }
  
  bool callAndCompare() const override {

    LArTT_Selector<LArCellCont> sel;
    ConstDataVector<CaloCellContainer> col(SG::VIEW_ELEMENTS);
    double etSum  = 0;      
    size_t count = 0;
    double minEta = 100;
    double maxEta = -100;
    double minPhi = 100;
    double maxPhi = -100;
    StatusCode status;
    if ( m_roi.isFullscan() ) {
      status = request( col );      
      status.ignore();
      
      for ( const auto cell : col ) {
	if ( !cell ) continue;
	etSum  += cell->et();
	count ++;
	minEta  = std::min( minEta, cell->eta() );
	maxEta  = std::max( maxEta, cell->eta() );
	minPhi  = std::min( minPhi, cell->phi() );
	maxPhi  = std::max( maxPhi, cell->phi() );
      }
    } else {
      
      status = request( sel );      
      status.ignore();
      
      for ( const auto cell : sel ) {
	etSum  += cell->et();
	count ++;
	minEta  = std::min( minEta, cell->eta() );
	maxEta  = std::max( maxEta, cell->eta() );
	minPhi  = std::min( minPhi, cell->phi() );
	maxPhi  = std::max( maxPhi, cell->phi() );
      }
      std::cout << "callAndCompare : " << m_context << " " << count << " " << etSum << " " << minEta << " " << maxEta << " " << minPhi << " " << maxPhi << " " << " " << m_minEtaRef << " " << m_maxEtaRef << " " << m_minPhiRef << " " << m_maxPhiRef << " " << m_etSumRef << " " << m_countRef << std::endl;
    }

    DIFF( "RoI mask", status.getCode(), m_statusRef.getCode() );
    DIFF( "RoI count ", count , m_countRef );
    DIFF( "RoI etSum ", etSum , m_etSumRef );
    DIFF( "RoI minEta", minEta, m_minEtaRef );
    DIFF( "RoI maxEta", maxEta, m_maxEtaRef );
    DIFF( "RoI minPhi", minPhi, m_minPhiRef );
    DIFF( "RoI maxPhi", maxPhi, m_maxPhiRef );
    
    bool checkStatus = m_statusRef == status
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
  const TrigRoiDescriptor m_roi;

  LArTT_Selector<LArCellCont> m_selRef;
  ConstDataVector<CaloCellContainer>* m_colRef;
  StatusCode m_statusRef;
  double m_etSumRef = 0;
  size_t m_countRef = 0;
  double m_minEtaRef = 100;
  double m_maxEtaRef = -100;
  double m_minPhiRef = 100;
  double m_maxPhiRef = -100;
};

TestCaloDataAccess::TestCaloDataAccess( const std::string& name, 
					ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ),
  m_dataAccessSvc( "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc", name ),
  m_emulateRoIs ( true ),
  m_emulateFixedRoIs (false)
{
  declareProperty("nFixedRoIs", m_nFixedRoIs = 1);
  declareProperty("emulateRoIs", m_emulateRoIs);
  declareProperty("emulateFixedRoIs", m_emulateFixedRoIs);

}

TestCaloDataAccess::~TestCaloDataAccess() {}

StatusCode TestCaloDataAccess::initialize() {
  CHECK( m_dataAccessSvc.retrieve() );
  return StatusCode::SUCCESS;
}

void TestCaloDataAccess::emulateRoIs( const EventContext& context, std::vector<ParallelCallTest*>& allRoIs ) const{

  double RoI_phi1 = M_PI*(-1.0 + ((double) rand () / RAND_MAX) * 2);
  double RoI_eta1 = randn(0,1.7);
  if ( RoI_eta1 < -2.5 ) RoI_eta1 = -2.5;
  if ( RoI_eta1 >  2.5 ) RoI_eta1 = 2.5;
  double chance = ((double) rand () / RAND_MAX);
  double width = 0.1;
  TrigRoiDescriptor roi( RoI_eta1, RoI_eta1-width, RoI_eta1+width, // eta
			 RoI_phi1, RoI_phi1-width, RoI_phi1+width, // phi
			 0 );
  AskForRoI* afr = new AskForRoI( context, m_dataAccessSvc, msg(), roi );
  allRoIs.push_back( afr );

  chance = ((double) rand () / RAND_MAX);
  if ( chance > 0.6 ) {
    double RoI_eta2 = -RoI_eta1 + randn(0,0.2);
    double RoI_phi2 = -RoI_phi1 + randn(0,0.2);
    if ( RoI_eta2 < -2.5 ) RoI_eta2 = -2.5;
    if ( RoI_eta2 >  2.5 ) RoI_eta2 = 2.5;
    TrigRoiDescriptor roi( RoI_eta2, RoI_eta2-width, RoI_eta2+width, // eta
			   RoI_phi2, RoI_phi2-width, RoI_phi2+width, // phi
			   0 );
    AskForRoI* afr = new AskForRoI( context, m_dataAccessSvc, msg(), roi );
    allRoIs.push_back( afr );
  }

  for(int i=0;i<10;i++){
    chance = ((double) rand () / RAND_MAX);
    if ( chance > 0.75 ) {
      double RoI_phi3 = M_PI*(-1.0 + ((double) rand () / RAND_MAX) * 2);
      double RoI_eta3 = randn(0,1.7);
      if ( RoI_eta3 < -2.5 ) RoI_eta3 = -2.5;
      if ( RoI_eta3 >  2.5 ) RoI_eta3 = 2.5;
      width = 0.1;
      if ( chance > 0.8 ) width=0.3;
      TrigRoiDescriptor roi( RoI_eta3, RoI_eta3-width, RoI_eta3+width, // eta
                             RoI_phi3, RoI_phi3-width, RoI_phi3+width, // phi
                             0 );
      AskForRoI* afr = new AskForRoI( context, m_dataAccessSvc, msg(), roi );
      allRoIs.push_back( afr );
    }
  }

  chance = ((double) rand () / RAND_MAX);
  if ( chance > 0.6 ) {
    TrigRoiDescriptor roi( true );
    AskForRoI* afr = new AskForRoI( context, m_dataAccessSvc, msg(), roi );
    allRoIs.push_back( afr );
  }

}

void TestCaloDataAccess::emulateFixedRoIs( const EventContext& context, std::vector<ParallelCallTest*>& allRoIs ) const{

  std::vector<TrigRoiDescriptor> rois;
  TrigRoiDescriptor roi1( 0.7, 0.7-0.1, 0.7+0.1, // eta
			  0.1, 0.1-0.1, 0.1+0.1, // phi
			  0 );
  TrigRoiDescriptor roi2( 0.8, 0.8-0.2, 0.7+0.2, // eta
			  0.2, 0.2-0.2, 0.2+0.2, // phi
			  0 );
  TrigRoiDescriptor roi3( -1.7, -1.7-0.4, -1.7+0.4, // eta
			  2.1, 2.1-0.4, 2.1+0.4, // phi
			  0 );
  TrigRoiDescriptor roi4( -1.0, -1.0-1.4, -1.0+1.4, // eta
			  1.1, 1.1-1.4, 1.1+1.4, // phi
			  0 );
  rois.push_back(roi1);
  rois.push_back(roi2);
  rois.push_back(roi3);
  rois.push_back(roi4);
  TrigRoiDescriptor roi5( true );
  for( int i=0;i<std::min(m_nFixedRoIs,4);++i) {
    AskForRoI* t1 = new AskForRoI( context, m_dataAccessSvc, msg(), rois[i]);
    allRoIs.push_back(t1);
  }
  AskForRoI* t6 = new AskForRoI( context, m_dataAccessSvc, msg(), roi5);  // FS
  allRoIs.push_back(t6);


}

StatusCode TestCaloDataAccess::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  
  std::vector<ParallelCallTest*> allRoIs;

  if ( m_emulateRoIs ) emulateRoIs ( context, allRoIs );
  if ( m_emulateFixedRoIs ) emulateFixedRoIs ( context, allRoIs );

    
  timeval ti1,ti2;
  gettimeofday(&ti1,NULL);
  bool result = ParallelCallTest::launchTests( 2, allRoIs);
  gettimeofday(&ti2,NULL);
  std::cout << ti1.tv_sec << " " << ti1.tv_usec << std::endl;
  std::cout << ti2.tv_sec << " " << ti2.tv_usec << std::endl;
  std::cout << name() << "; time : " << 1e-6*( 1e6*(ti2.tv_sec - ti1.tv_sec) + ( ti2.tv_usec - ti1.tv_usec ) ) << std::endl;

  if ( result == false ) {
    ATH_MSG_ERROR( "Test of data access failed " );
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}


