/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*---------------------------------------------------------
 * @file TRT_ActiveFractionSvc.cxx
 * @Service to provide active straw fraction in an eta-phi bin
 * @author Denver Whittington <Denver.Whittington@cern.ch>
 * @author Sasa Fratina <Sasa.Fratina@cern.ch>
 *///------------------------------------------------------

// Header Includes
#include "TRT_ActiveFractionSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

//////
/// Constructor
TRT_ActiveFractionSvc::TRT_ActiveFractionSvc( const std::string& name,
					      ISvcLocator* pSvcLocator ) :
  AthService( name, pSvcLocator ),
  m_incSvc("IncidentSvc",name),
  m_detStore("DetectorStore",name),
  m_deadStrawSvc("TRT_StrawStatusSummarySvc",name),
  m_nBinsPhi(96)
{
  // Get properties from job options
  declareProperty( "EtaBins", m_etaBins );
  declareProperty( "nBinsPhi", m_nBinsPhi );

  // Build the eta bin list (if not specified in job options)
  if ( m_etaBins.size() == 0 ) {
    // Default eta bins
    m_etaBins.push_back( std::make_pair(-2.1,-1.75) );
    m_etaBins.push_back( std::make_pair(-1.75,-1.3) );
    m_etaBins.push_back( std::make_pair(-1.3,-1.07) );
    m_etaBins.push_back( std::make_pair(-1.07,-0.65) );
    m_etaBins.push_back( std::make_pair(-0.65,-0.1) );
    m_etaBins.push_back( std::make_pair(-0.1,0.) );
    m_etaBins.push_back( std::make_pair(0.,0.1) );
    m_etaBins.push_back( std::make_pair(0.1,0.65) );
    m_etaBins.push_back( std::make_pair(0.65,1.07) );
    m_etaBins.push_back( std::make_pair(1.07,1.3) );
    m_etaBins.push_back( std::make_pair(1.3,1.75) );
    m_etaBins.push_back( std::make_pair(1.75,2.1) );
  }
}

//////
/// Destructor
TRT_ActiveFractionSvc::~TRT_ActiveFractionSvc() {}

//////
/// Initialize
StatusCode TRT_ActiveFractionSvc::initialize() {
  StatusCode sc(StatusCode::SUCCESS);

  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "TRT_ActiveFractionSvc:initialize()" << endmsg;

  // Get the TRT_StrawStatusSummarySvc
  sc = m_deadStrawSvc.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't get " << m_deadStrawSvc );
    return sc;
  }

  // Register a callback to create the active fraction table at "BeginRun"
  sc = m_incSvc.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't get " << m_incSvc );
    return sc;
  }
  m_incSvc->addListener( this, std::string("BeginRun") );

  // Build the phi bin list (equal bin widths)
  double phiEdgeLow = -1. * M_PI; // Check this to be sure...
  double deltaPhi = 2. * M_PI / (1. * m_nBinsPhi) ;
  for ( int i = 0; i < m_nBinsPhi; ++i ) {
    m_phiBins.push_back( std::make_pair( phiEdgeLow + i*deltaPhi, phiEdgeLow + (i+1)*deltaPhi ) );
  }
  // Initialize the table with 1.'s
  std::vector<double> dummyPhiVec( m_phiBins.size(), 1. );
  std::vector<std::vector<double> > dummyTable( m_etaBins.size(), dummyPhiVec );
  m_activeFracTable = dummyTable;

  return sc;
}

//////
/// IncidentSvc handler
void TRT_ActiveFractionSvc::handle( const Incident& inc ) {

  // BeginRun handler
  if ( inc.type() == "BeginRun" ) {
    /*
     * Sasa July 22, 2009
     */
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "TRT_ActiveFractionSvc: initialize TRT_ActiveFractionSvc table (BeginRun), N phi, eta bins: " 
                                 << m_phiBins.size() << ", " << m_etaBins.size() << endmsg;

    for (unsigned int i=0; i<m_etaBins.size(); i++) {
      double etaMin = m_etaBins[i].first;
      double etaMax = m_etaBins[i].second;
      int bin = findEtaBin( (etaMin+etaMax)/2. );
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT_ActiveFractionSvc: findEtaBin( "<< etaMin << ", " << etaMax << " ] = " << bin << endmsg;
    }

    for (unsigned int i=0; i<m_phiBins.size(); i++) {
      double phiMin = m_phiBins[i].first;
      double phiMax = m_phiBins[i].second;
      int bin = findPhiBin( (phiMin+phiMax)/2. );
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT_ActiveFractionSvc: findPhiBin( "<< phiMin << ", " << phiMax << " ] = " << bin << endmsg;
    }

    std::vector<int> dummyPhiVec( m_phiBins.size(), 0 );
    std::vector<std::vector<int> > dummyTableCountAll( m_etaBins.size(), dummyPhiVec );
    std::vector<std::vector<int> > dummyTableCountDead( m_etaBins.size(), dummyPhiVec );

    StatusCode sc = m_detStore.retrieve();
    if ( sc.isFailure() ) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "TRT_ActiveFractionSvc::handle: Unable to retrieve DetectorStore" << endmsg;
      return;
    }

    const InDetDD::TRT_DetectorManager *TRTDetMgr = 0;	
    sc = m_detStore->retrieve(TRTDetMgr, "TRT");
    if ( sc.isFailure() ) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "TRT_ActiveFractionSvc::handle: Unable to retrieve TRT Detector Manager " << endmsg;
      return;
    }	

    const TRT_ID *TRTHelper;
    sc = m_detStore->retrieve(TRTHelper, "TRT_ID");
    if ( sc.isFailure() ) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "TRT_ActiveFractionSvc::handle: Could not get TRT ID helper, TRT_ActiveFraction == 1" << endmsg;
      return;
    }  
	 
    double rMinEndcap = 617.; 
    double rMaxEndcap = 1106.;
    int countAll(0), countDead(0), countSaved(0), countPhiSkipped(0), countEtaSkipped(0), countInvalidEtaValues(0); 
    for (std::vector<Identifier>::const_iterator it = TRTHelper->straw_layer_begin(); it != TRTHelper->straw_layer_end(); it++  ) {
      int nStrawsInLayer = TRTHelper->straw_max(*it);
      for (int i=0; i<=nStrawsInLayer; i++) { 

        Identifier id = TRTHelper->straw_id(*it, i);	 
        bool status = m_deadStrawSvc->get_status(id);
        countAll++; if (status) countDead++;

        const Amg::Vector3D &strawPosition = TRTDetMgr->getElement( id )->center( id );
        double phi = atan2( strawPosition.y(), strawPosition.x() );
        int phiBin = findPhiBin( phi );
	if (phiBin<0) { if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT_ActiveFractionSvc::handle phiBin<0: " << phi << " " << phiBin << endmsg; countPhiSkipped++; continue; }

	// calculate etaMin, etaMax
	int side = TRTHelper->barrel_ec(id);
        double z = fabs( strawPosition.z() );
	double thetaMin(0.), thetaMax(0.);
	if (abs(side)==1) { // barrel
          double zRange = 360.; // straw length / 2  
	  if ( TRTHelper->layer_or_wheel(id) == 0 && TRTHelper->straw_layer(id) < 9 ) zRange = 160.;  // short straws
	  double r = sqrt( pow(strawPosition.x(), 2) + pow(strawPosition.y(), 2) );
	  thetaMin = atan( r / (z+zRange) );
	  thetaMax = ((z-zRange)>0.) ? atan( r / (z-zRange) ) : 1.57; // M_PI_2 - epsilon
	} else { // endcap
	  thetaMin = atan( rMinEndcap / z );		
	  thetaMax = atan( rMaxEndcap / z );	
	}
        if (side<0) { // theta -> M_PI - theta
          double thetaDummy = thetaMin;
          thetaMin = M_PI - thetaMax;
          thetaMax = M_PI - thetaDummy;
        }

        double thetaCheck[] = {thetaMax, thetaMin};
        double etaCheck[] = {0., 0.};
        for (int ti=0; ti<2; ti++) {
          if (thetaCheck[ti]<=0.||thetaCheck[ti]>=M_PI) if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT_ActiveFractionSvc: theta " << ti << " " << thetaCheck[ti] << endmsg;
          double tanTheta = tan(thetaCheck[ti]/2.);
          if (tanTheta<=0.) { if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT_ActiveFractionSvc: theta tan " << ti << " " << tanTheta << endmsg; countInvalidEtaValues++; continue; }
          etaCheck[ti] = -log( tanTheta );
        }
       double etaMin = etaCheck[0];
       double etaMax = etaCheck[1];
//	double etaMin =  -log( tan( thetaMax / 2.) ); // eta goes in the other ways as theta!! etaMIN = f(thetaMAX) 
//        double etaMax =  -log( tan( thetaMin / 2.) );
	int etaMinBin = findEtaBin( etaMin );
	int etaMaxBin = findEtaBin( etaMax ); 
        if (etaMin>=etaMax) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "TRT_ActiveFractionSvc::handle etaMaxBin<etaMinBin " << etaMin << " " << etaMax << " " << thetaMin << " " << thetaMax << endmsg;
	if (etaMinBin<0 && etaMaxBin<0) { 
          if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "TRT_ActiveFractionSvc::handle etaMaxBin<etaMinBin " << thetaMin << " " << thetaMax 
                                          << " " << etaMin << " " << etaMax << " " << etaMinBin << " " << etaMaxBin << ", side: " << side << endmsg;
          countEtaSkipped++; continue; 
        }
	if (etaMinBin<0) etaMinBin = 0;
	if (etaMaxBin<0) etaMaxBin = m_etaBins.size() - 1;
        if (etaMaxBin<etaMinBin) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "TRT_ActiveFractionSvc::handle etaMaxBin<etaMinBin " << etaMinBin << " " << etaMaxBin << ", side: " << side << endmsg;

        countSaved++; // now save straw info for these bins
	for (int iEta = etaMinBin; iEta <= etaMaxBin; iEta++) {
	  dummyTableCountAll[iEta][phiBin]++;
	  if (status) dummyTableCountDead[iEta][phiBin]++;
	}

      }
    } // end straw Identifier loop 	

    for (unsigned int i = 0; i < m_etaBins.size(); ++i) // fill the table
      for (unsigned int j = 0; j < m_phiBins.size(); ++j) {
        double deadFraction = 1. * dummyTableCountDead[i][j];
	if (dummyTableCountAll[i][j]>0) deadFraction /= (1. * dummyTableCountAll[i][j]);
        m_activeFracTable[i][j] = 1. - deadFraction;
        if ( msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "dummyTableCountDead: " << i << ", " << j << ", count " << dummyTableCountAll[i][j] << " dead " << deadFraction << endmsg;
      }

     double deadStrawFraction = (1.*countDead) / (1.*countAll);
     if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "TRT_ActiveFractionSvc: initialize TRT_ActiveFractionSvc table finished, count all TRT straws: " << countAll 
                                  << ", count dead straws: " << countDead << " (" << deadStrawFraction << "%), straws skipped due to invalid phi, eta range: " 
                                  << countPhiSkipped << " " << countEtaSkipped << endmsg;

     if (countInvalidEtaValues && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "TRT_ActiveFractionSvc: found invalid eta range, check: " << countInvalidEtaValues << endmsg;
  } // end if BeginRun

  return;
}

//////
/// Finalize
StatusCode TRT_ActiveFractionSvc::finalize() {
  return StatusCode::SUCCESS;
}
