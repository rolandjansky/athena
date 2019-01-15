/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>

#undef NDEBUG
#include <cassert>


#include "TestTools/initGaudi.h"
#include "TestTools/expect.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "CxxUtils/ubsan_suppress.h"


#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/MonitoredGroup.h"
#include "AthenaMonitoring/MonitoredScalar.h"
#include "AthenaMonitoring/MonitoredTimer.h"


TH1* getHist( ITHistSvc* histSvc, const std::string& histName ) {
  TH1* h( nullptr );
  histSvc->getHist( histName, h );
  VALUE( h ) NOT_EXPECTED( ( TH1* )nullptr );
  return h;
}


void resetHist( ITHistSvc* histSvc, const std::string& histName ) {
  getHist( histSvc, histName )->Reset();
}


void resetHists( ITHistSvc* histSvc ) {
  resetHist( histSvc, "/EXPERT/TestGroup/Eta_vs_Phi" );
  resetHist( histSvc, "/EXPERT/TestGroup/Eta" );
  resetHist( histSvc, "/EXPERT/TestGroup/Phi" );
}



double contentInBin1DHist( ITHistSvc* histSvc, const std::string& histName, int bin ) {
  TH1* h = getHist( histSvc, histName );
  // this are in fact securing basic correctness of the tests
  VALUE( h )   NOT_EXPECTED( nullptr );  
  VALUE( bin >= 1 ) EXPECTED( true );
  VALUE( bin <= h->GetXaxis()->GetNbins()+1 ) EXPECTED( true );
  return h->GetBinContent( bin );
}

double contentInBin2DHist( ITHistSvc* histSvc, const std::string& histName, int bin1, int bin2 ) {
  TH2* h( nullptr );
  histSvc->getHist( histName, h );
  // this are in fact securing basic correctness of the tests
  VALUE( h )   NOT_EXPECTED( nullptr );  
  VALUE( bin1 >= 1 ) EXPECTED( true );
  VALUE( bin1 <= h->GetXaxis()->GetNbins()+1 ) EXPECTED( true );
  VALUE( bin2 >= 1 ) EXPECTED( true );
  VALUE( bin2 <= h->GetYaxis()->GetNbins()+1 ) EXPECTED( true );
  return h->GetBinContent( bin1, bin2 );
}


bool noToolBehaviourCorrect( ToolHandle<GenericMonitoringTool>& monTool ) {
  auto x = Monitored::Scalar( "x", -99.0 );
  auto monitorIt = Monitored::Group( monTool, x );
  return true;
}


bool fillFromScalarWorked( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {
  resetHists( histSvc );
  {
    auto roiPhi = Monitored::Scalar( "Phi", -99.0 ); //deduced double
    auto roiEta = Monitored::Scalar<double>( "Eta", -99 ); //explicit double
    auto monitorIt = Monitored::Group( monTool, roiPhi, roiEta );
    roiPhi = 0.1;
    roiEta = -0.2;    
  }
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 1 ) ) EXPECTED( 0 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 2 ) ) EXPECTED( 1 );
  
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 1 ) ) EXPECTED( 1 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 0 );

  return true;
}

bool fillFromScalarIndependentScopesWorked( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {
  resetHists( histSvc ); 
  
  auto roiPhi = Monitored::Scalar( "Phi", -99.0 );
  auto roiEta = Monitored::Scalar<double>( "Eta", -99 );
  
  for ( size_t i =0; i < 3; ++i ) { 
    auto monitorIt = Monitored::Group( monTool, roiPhi );
    roiPhi = 0.1;
    roiEta = -0.2;    
  }
  
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 1 ) ) EXPECTED( 0 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 2 ) ) EXPECTED( 3 );
  
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 1 ) ) EXPECTED( 0 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 0 );

  for ( size_t i =0; i < 10; ++i ) { 
    auto monitorIt = Monitored::Group( monTool, roiEta );
    roiPhi = 0.1;
    roiEta = -0.2;    
  }

  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 1 ) ) EXPECTED( 0 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 2 ) ) EXPECTED( 3 );
  
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 1 ) ) EXPECTED( 10 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 0 );

  return true;
}

bool fill2DWorked( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {
  resetHists( histSvc );

  
  auto roiPhi = Monitored::Scalar( "Phi", -99.0 );
  auto roiEta = Monitored::Scalar( "Eta", -99.0 );
  {
    auto monitorIt = Monitored::Group( monTool, roiEta, roiPhi );
    roiEta = 0.2;
    roiPhi = -0.1;
  }
  VALUE( contentInBin2DHist( histSvc, "/EXPERT/TestGroup/Eta_vs_Phi", 1, 1 ) ) EXPECTED( 0 );
  VALUE( contentInBin2DHist( histSvc, "/EXPERT/TestGroup/Eta_vs_Phi", 1, 2 ) ) EXPECTED( 0 );
  VALUE( contentInBin2DHist( histSvc, "/EXPERT/TestGroup/Eta_vs_Phi", 2, 1 ) ) EXPECTED( 1 );
  VALUE( contentInBin2DHist( histSvc, "/EXPERT/TestGroup/Eta_vs_Phi", 2, 2 ) ) EXPECTED( 0 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 1 ); // counts also visible in 1 D
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 1 ) ) EXPECTED( 1 );
  

  
  // 2 D Hist fill should not affect 1 D
  resetHists( histSvc );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta_vs_Phi" )->GetEntries() ) EXPECTED( 0 );
  {
    auto monitorIt = Monitored::Group( monTool, roiEta );
    roiEta = 0.2;
    roiPhi = -0.1;
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta_vs_Phi" )->GetEntries() ) EXPECTED( 0 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 1 );
  
  {
    auto monitorIt = Monitored::Group( monTool, roiPhi );
    roiEta = 0.2;
    roiPhi = -0.1;
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta_vs_Phi" )->GetEntries() ) EXPECTED( 0 ); // still no entries as scope used above is not having both needed varaibles
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 1 ); // no increase of counts
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 1 ) ) EXPECTED( 1 );

  
  return true;
}  


bool fillExplcitelyWorked( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {
  resetHists( histSvc );
  auto roiPhi = Monitored::Scalar( "Phi", -99.0 );
  auto roiEta = Monitored::Scalar( "Eta", -99.0 );
  {
    auto monitorIt = Monitored::Group( monTool, roiEta, roiPhi );
    monitorIt.setAutoFill( false );
    roiEta = 0.2;
    roiPhi = -0.1;
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta_vs_Phi" )->GetEntries() ) EXPECTED( 0 ); //  auto filling was disabled so no entries
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta" )->GetEntries() ) EXPECTED( 0 ); //  auto filling was disabled so no entries
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi" )->GetEntries() ) EXPECTED( 0 ); //  auto filling was disabled so no entries  

  auto monitorIt = Monitored::Group( monTool, roiEta, roiPhi );
  monitorIt.setAutoFill( false );
  for ( size_t i = 0; i < 3; ++i ) {
    monitorIt.fill();
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta_vs_Phi" )->GetEntries() ) EXPECTED( 3 ); 
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta" )->GetEntries() ) EXPECTED( 3 ); 
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi" )->GetEntries() ) EXPECTED( 3 ); 
  
  return true;
}

class Scalar {
public:
    Scalar() : m_value( 0 ) { }
    Scalar( double value ) : m_value( value ) { }
    
    void operator=( double value ) { m_value = value; }
    operator double() const { return m_value; }
private:
    double m_value;
};

class Track {
public:
    Track() : m_eta( 0 ), m_phi( 0 ) {}
    Track( double e, double p ) : m_eta( e ), m_phi( p ) {}
    double eta() const { return m_eta; }
    double phi() const { return m_phi; }
private:
    double m_eta, m_phi;
};


bool fillFromNonTrivialSourcesWorked( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {
  resetHists( histSvc );
  {
    auto eta = Monitored::Scalar( "Eta", Scalar( 0.2 ) ); //class object convertable to number
    auto monitorIt = Monitored::Group( monTool, eta );
  }
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 1 ); 

  resetHists( histSvc );
  {
    std::vector<float> eta( {0.2, 0.1} );
    std::set<double> phi( {-1, 1} ) ;
    auto vectorT   = Monitored::Collection( "Eta", eta );
    auto setT      = Monitored::Collection( "Phi", phi );
    auto monitorIt = Monitored::Group( monTool, vectorT, setT );
  }
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 2 ); 

  resetHists( histSvc );
  {
    
    std::array<double, 2> eta( {{0.1, 0.7}} );
    double phi[2]={-2., -1.};
    auto arrayT = Monitored::Collection( "Eta", eta );
    auto rawArrayT   = Monitored::Collection( "Phi", phi );
    auto monitorIt = Monitored::Group( monTool, arrayT, rawArrayT );
  }
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 2 );

  resetHists( histSvc );
  {
    Track tracks[2];
    
    auto eta = Monitored::Collection( "Eta", tracks, &Track::eta );
    auto phi = Monitored::Collection( "Phi", tracks, []( const Track& t ) { return t.phi(); } );
    
    auto monitorIt = Monitored::Group( monTool, eta, phi ); // this is binding to histograms
    
    tracks[0] = Track( 0.1, 0.9 );
    tracks[1] = Track( 1.3, 1. );
  }
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 1 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 2 ) ) EXPECTED( 1 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta_vs_Phi" )->GetEntries() ) EXPECTED( 2 ); 
  
  return true;
}


bool assignWorked() {
    auto eta = Monitored::Scalar( "Eta", -3. );
    eta = 0.6;
    VALUE ( double( eta ) ) EXPECTED ( 0.6 );
    auto etaBis = Monitored::Scalar( "EtaBis", 0. );
    etaBis = 0.4;
    VALUE( double( etaBis ) ) EXPECTED( 0.4 );
    etaBis = double( eta );    
    VALUE( double( etaBis ) ) EXPECTED( 0.6 );
    return true;
}

bool operatorsWorked() {
  auto count = Monitored::Scalar<float>( "Count", 0 );

  VALUE( count==count ) EXPECTED (true);

  count += 1;
  VALUE ( int(count) ) EXPECTED (1);
  count++;
  VALUE ( int(count) ) EXPECTED (2);
  --count;
  VALUE ( int(count) ) EXPECTED (1);
  count *= 3;
  VALUE ( int(count) ) EXPECTED (3);

  return true;
}

bool timerFillingWorked( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {

  auto t1 = Monitored::Timer( "TIME_t1" );
  auto t2 = Monitored::Timer( "TIME_t2" );
  {
    auto monitorIt = Monitored::Group( monTool, t1, t2 ); // this is binding to histograms
  
    t1.start();
    t1.stop();
  }
  VALUE( double( t1 ) <= double( t2 ) ) EXPECTED( true );  // timer is monotonic
  VALUE( double( t1 ) < 1e6 ) EXPECTED ( true ); // should be less than 1s unless the stop/start are wrong
  VALUE( double( t2 ) < 1e6 ) EXPECTED ( true ); // should be less than 1s unless the contr and op double are wrong
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/TIME_t1" )->GetEntries() ) EXPECTED( 1 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/TIME_t2" )->GetEntries() ) EXPECTED( 1 );
  
  return true;
  
}




int main() {
  //CxxUtils::ubsan_suppress ( []() { TInterpreter::Instance(); } );
  ISvcLocator* pSvcLoc;
  if ( !Athena_test::initGaudi( "GenericMon.txt",  pSvcLoc ) ) {
    std::cerr << "ERROR This test can not be run" << std::endl;
    return -1;
  }
  MsgStream log( Athena::getMessageSvc(), "GenericMonFilling_test" );

  ITHistSvc* histSvc;
  if( pSvcLoc->service( "THistSvc", histSvc, true ).isFailure()  ) {
    log << MSG::ERROR << "THistSvc not available " << endmsg;
    return -1;
  }
  
  // we need to test what happens to the monitoring when tool is not valid
  ToolHandle<GenericMonitoringTool> emptyMon("");
  VALUE( emptyMon.isEnabled() ) EXPECTED( false ); // self test
  log << MSG::DEBUG << " mon tool validity " << emptyMon.isValid() << endmsg;

    
  
  ToolHandle<GenericMonitoringTool> validMon( "GenericMonitoringTool/MonTool" );
  if ( validMon.retrieve().isFailure() ) {
    log << MSG::ERROR << "Failed to acquire the MonTool tools via the ToolHandle" << endmsg;
    return -1;
  }
  
  assert( fillFromScalarWorked( validMon, histSvc ) );
  assert( noToolBehaviourCorrect( emptyMon ) );
  assert( fillFromScalarIndependentScopesWorked( validMon, histSvc ) );
  assert( fill2DWorked( validMon, histSvc ) );
  assert( fillExplcitelyWorked( validMon, histSvc ) );
  assert( fillFromScalarIndependentScopesWorked( validMon, histSvc ) );
  assert( assignWorked() );
  assert( operatorsWorked() );
  assert( timerFillingWorked( validMon, histSvc ) );

  log << MSG::DEBUG << "All OK"  << endmsg;

  // Make sure that THistSvc gets finalized.
  // Otherwise, the output file will get closed while global dtors are running,
  // which can lead to crashes.
  if (ISvcManager* svcmgr = dynamic_cast<ISvcManager*>( pSvcLoc )) {
    svcmgr->finalizeServices().ignore();
  }
  return 0;
}
