/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include <chrono>
#include <thread>

#undef NDEBUG
#include <cassert>

#include "TestTools/initGaudi.h"
#include "TestTools/expect.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "CxxUtils/ubsan_suppress.h"

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TTree.h"
#include "THashList.h"
#include "TInterpreter.h"


const TH1* getHist( ITHistSvc* histSvc, const std::string& histName ) {
  TH1* h( nullptr );
  histSvc->getHist( histName, h );
  VALUE( h ) NOT_EXPECTED( ( TH1* )nullptr );
  return h;
}

TTree* getTree( ITHistSvc* histSvc, const std::string& treeName ) {
  TTree* t( nullptr );
  histSvc->getTree( treeName, t );
  VALUE( t ) NOT_EXPECTED( ( TTree* )nullptr );
  return t;
}

void resetHist( ITHistSvc* histSvc, const std::string& histName ) {
  TH1* h ATLAS_THREAD_SAFE = const_cast<TH1*>(getHist( histSvc, histName ));
  h->Reset();
}

void resetHists( ITHistSvc* histSvc ) {
  for (const std::string& name : histSvc->getHists()) {
    resetHist( histSvc, name );
  }
  for (const std::string& name : histSvc->getTrees()) {
    getTree( histSvc, name )->Reset();
  }
}

double contentInBin1DHist( ITHistSvc* histSvc, const std::string& histName, int bin ) {
  const TH1* h = getHist( histSvc, histName );
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
  VALUE( h ) NOT_EXPECTED( nullptr );
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

/**
 * Launch `nthreads` each calling `func` `nfills` times.
 * @return Total number of fills performed
 */
template <typename F>
size_t fill_mt(const F& func)
{
  const size_t nthreads = 10;
  const size_t nfills = 1000;

  // Create threads
  std::vector<std::thread> threads;
  std::vector<size_t> fills(nthreads, 0);
  threads.reserve(nthreads);
  for (size_t i = 0; i < nthreads; ++i) {
    threads.push_back(std::thread([&, i]() {
      for (size_t j = 0; j < nfills; ++j) fills[i] += func();
    }));
  }
  // Launch and wait
  for (auto& t : threads) t.join();

  // Return total number of fills performed
  return std::accumulate(fills.begin(), fills.end(), 0);
}

bool fillFromScalarWorked( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {

  auto fill = [&]() {
    auto roiPhi = Monitored::Scalar( "Phi", -99.0 ); //deduced double
    auto roiEta = Monitored::Scalar<double>( "Eta", -99 ); //explicit double
    auto monitorIt = Monitored::Group( monTool, roiPhi, roiEta );
    roiPhi = 0.1;
    roiEta = -0.2;
    return 1;
  };

  auto check = [&](size_t N) {
    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 1 ) ) EXPECTED( 0 );
    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 2 ) ) EXPECTED( N );

    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 1 ) ) EXPECTED( N );
    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 0 );

    auto tree = getTree( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta_Tree" );
    VALUE( tree->GetEntries() ) EXPECTED( N );
    std::vector<float> tmpvec;
    Float_t tmp;
    tree->GetBranch("Phi")->SetObject(&tmpvec);
    tree->GetBranch("Eta")->SetAddress(&tmp);
    for (int i=0; i<tree->GetEntries(); ++i) {
      tree->GetEntry(i);
      VALUE( tmp ) EXPECTED ( -0.2 );
      VALUE( (const float&) tmpvec.at(0) ) EXPECTED( 0.1 );
    }
  };

  resetHists( histSvc ); check(fill());
  resetHists( histSvc ); check(fill_mt(fill));

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

  auto roiPhi = Monitored::Scalar( "Phi", -99.0 );
  auto roiEta = Monitored::Scalar( "Eta", -99.0 );

  auto fill = [&]() {
    auto monitorIt = Monitored::Group( monTool, roiEta, roiPhi );
    roiEta = 0.2;
    roiPhi = -0.1;
    return 1;
  };
  auto check = [&](size_t N) {
    VALUE( contentInBin2DHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta", 1, 1 ) ) EXPECTED( 0 );
    VALUE( contentInBin2DHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta", 1, 2 ) ) EXPECTED( 0 );
    VALUE( contentInBin2DHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta", 2, 1 ) ) EXPECTED( N );
    VALUE( contentInBin2DHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta", 2, 2 ) ) EXPECTED( 0 );
    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( N ); // counts also visible in 1 D
    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 1 ) ) EXPECTED( N );
  };

  resetHists( histSvc ); check(fill());
  resetHists( histSvc ); check(fill_mt(fill));

  // 2 D Hist fill should not affect 1 D
  resetHists( histSvc );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 0 );
  {
    auto monitorIt = Monitored::Group( monTool, roiEta );
    roiEta = 0.2;
    roiPhi = -0.1;
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 0 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 1 );

  {
    auto monitorIt = Monitored::Group( monTool, roiPhi );
    roiEta = 0.2;
    roiPhi = -0.1;
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 0 ); // still no entries as scope used above is not having both needed variables
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 1 ); // no increase of counts
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 1 ) ) EXPECTED( 1 );

  return true;
}

bool fillExplicitlyWorked( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {
  resetHists( histSvc );
  auto roiPhi = Monitored::Scalar( "Phi", -99.0 );
  auto roiEta = Monitored::Scalar( "Eta", -99.0 );

  // Check disabling of filling
  {
    auto monitorIt = Monitored::Group( monTool, roiEta, roiPhi );
    monitorIt.setAutoFill( false );
    roiEta = 0.2;
    roiPhi = -0.1;
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 0 ); //  auto filling was disabled so no entries
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta" )->GetEntries() ) EXPECTED( 0 ); //  auto filling was disabled so no entries
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi" )->GetEntries() ) EXPECTED( 0 ); //  auto filling was disabled so no entries

  // Check explicit fill in loops
  {
    auto monitorIt = Monitored::Group( monTool, roiEta, roiPhi );
    for ( size_t i = 0; i < 3; ++i ) {
      monitorIt.fill();   // this will fill and disable autoFill
    }
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 3 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta" )->GetEntries() ) EXPECTED( 3 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi" )->GetEntries() ) EXPECTED( 3 );

  // Check explicit one-time fill via temporary Group instance
  {
    Monitored::Group( monTool, roiEta, roiPhi ).fill();
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 4 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta" )->GetEntries() ) EXPECTED( 4 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi" )->GetEntries() ) EXPECTED( 4 );

  return true;
}

bool fillWithCutMaskWorked( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {

  auto fill1 = [&]() {
    for (int ctr = 0; ctr < 10; ++ctr) {
      auto roiEta = Monitored::Scalar<double>( "Eta", -99 ); //explicit double
      auto cutMask = Monitored::Scalar<bool>( "CutMask", (ctr % 2) == 0);
      auto monitorIt = Monitored::Group( monTool, roiEta, cutMask );
      roiEta = -0.2;
      monitorIt.fill();
    }
    return 5;
  };
  auto check1 = [&](size_t N) {
    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta_CutMask", 1 ) ) EXPECTED( N );
    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta_CutMask", 2 ) ) EXPECTED( 0 );
  };
  resetHists( histSvc ); check1(fill1());
  resetHists( histSvc ); check1(fill_mt(fill1));

  auto fill2 = [&]() {
    std::vector<float> etaVec{-0.2, 0.2, -0.4, 0.4, -0.6};
    auto roiEta = Monitored::Collection( "Eta", etaVec );
    std::vector<char> cutMaskVec =  { 0, 1, 1, 1, 0 };
    auto cutMask = Monitored::Collection( "CutMask", cutMaskVec );
    auto monitorIt = Monitored::Group( monTool, roiEta, cutMask );
    return 1;
  };
  auto check2 = [&](size_t N) {
    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta_CutMask", 1 ) ) EXPECTED( N );
    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta_CutMask", 2 ) ) EXPECTED( 2*N );
  };
  resetHists( histSvc ); check2(fill2());
  resetHists( histSvc ); check2(fill_mt(fill2));

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
    int testVal = 0;
    auto eta = Monitored::Scalar<int>( "Eta", [&](){ return testVal; } ); // some generator
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
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 2 );

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

  auto t1 = Monitored::Timer( "TIME_t1" );  // default is microseconds
  auto t2 = Monitored::Timer<std::chrono::milliseconds>( "TIME_t2" );
  {
    auto monitorIt = Monitored::Group( monTool, t1, t2 );
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  // There should be one entry in the histogram with roughly 10ms
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/TIME_t1" )->GetEntries() ) EXPECTED( 1 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/TIME_t2" )->GetEntries() ) EXPECTED( 1 );
  double t1_value = getHist( histSvc, "/EXPERT/TestGroup/TIME_t1" )->GetMean();
  double t2_value = getHist( histSvc, "/EXPERT/TestGroup/TIME_t2" )->GetMean();
  assert( 9000 < t1_value && t1_value < 11000 );
  assert( 8 < t2_value && t2_value < 12 );

  // Test scoped timer
  auto t3 = Monitored::Timer<std::chrono::milliseconds>( "TIME_t3" );
  {
    auto monitorIt = Monitored::Group( monTool, t3 );
    {
      Monitored::ScopedTimer timeit(t3);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    {
      // code that should not be included in timer
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/TIME_t3" )->GetEntries() ) EXPECTED( 1 );
  double t3_value = getHist( histSvc, "/EXPERT/TestGroup/TIME_t3" )->GetMean();
  // This is not robust --- user code could be blocked for arbitrarily long.
  //assert( 8 < t3_value && t3_value < 12 );
  assert( 8 < t3_value );

  return true;
}


bool stringFillingWorked(ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc) {

  auto fill = [&]() {
    auto count = Monitored::Scalar<std::string>( "DetID", "SCT" );
    Monitored::Group(monTool, count);
    Monitored::Group(monTool, count);
    return 2;
  };
  auto check = [&](size_t N) {
    const TH1* h = getHist( histSvc, "/EXPERT/TestGroup/DetID" );
    VALUE( h->GetEntries() ) EXPECTED( N );
    VALUE( h->GetXaxis()->GetLabels()->GetEntries() ) EXPECTED( 1 );
    const int sctBin = h->GetXaxis()->FindFixBin("SCT");
    VALUE( sctBin ) EXPECTED( 1 );
    VALUE( h->GetBinContent( sctBin ) ) EXPECTED( N );
  };

  resetHists( histSvc ); check(fill());
  resetHists( histSvc ); check(fill_mt(fill));

  return true;
}

bool string2DFillingWorked(ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc) {

  auto fill = [&]() {
    auto countID = Monitored::Scalar<std::string>( "DetID", "SCT" );
    std::vector<std::string> caloLabels( { "LAr", "LAr", "Tile" } );
    auto countCalo = Monitored::Collection<std::vector<std::string>>( "DetCalo", caloLabels );
    auto x = Monitored::Scalar("x", 1.2 );
    std::vector<double> yvalues({0.2, 2.1, 1.3});
    auto y = Monitored::Collection("y", yvalues );
    {
      // this should fill like this
      // SCT, LAr
      // SCT, LAr
      // SCT, Tile
      Monitored::Group(monTool, countID, countCalo);
    }
    {
      // this should fill like this
      // LAr,  0.2
      // LAr,  2.1
      // Tile, 1.3
      Monitored::Group(monTool, countCalo, y);
    }
    {
      // this should fill like this
      // LAr, 1.2
      // LAr, 1.2
      // Tile, 1.2
      Monitored::Group(monTool, countCalo, x);
    }
    return 1;
  };
  auto check = [&](size_t N) {
    const TH1* h = getHist( histSvc, "/EXPERT/TestGroup/DetID_vs_DetCalo" );
    int larBin = h->GetXaxis()->FindFixBin("LAr");
    int sctBin = h->GetYaxis()->FindFixBin("SCT");
    VALUE( h->GetBinContent( larBin, sctBin ) ) EXPECTED( 2*N );

    h = getHist( histSvc, "/EXPERT/TestGroup/y_vs_DetCalo" );
    larBin = h->GetXaxis()->FindFixBin("LAr");
    int tileBin = h->GetXaxis()->FindFixBin("Tile");
    VALUE( h->GetBinContent( larBin, 1 ) ) EXPECTED( N );
    VALUE( h->GetBinContent( larBin, 2 ) ) EXPECTED( 0 );
    VALUE( h->GetBinContent( larBin, 3 ) ) EXPECTED( N );
    VALUE( h->GetBinContent( tileBin, 1 ) ) EXPECTED( 0 );
    VALUE( h->GetBinContent( tileBin, 2 ) ) EXPECTED( N );

    h = getHist( histSvc, "/EXPERT/TestGroup/DetCalo_vs_x" );
    larBin = h->GetYaxis()->FindFixBin("LAr");
    tileBin = h->GetYaxis()->FindFixBin("Tile");
    VALUE( h->GetBinContent( 1, larBin) ) EXPECTED( 0 );
    VALUE( h->GetBinContent( 2, larBin) ) EXPECTED( 2*N );
    VALUE( h->GetBinContent( 1, tileBin) ) EXPECTED( 0 );
    VALUE( h->GetBinContent( 2, tileBin) ) EXPECTED( N );
  };

  resetHists( histSvc ); check(fill());
  resetHists( histSvc ); check(fill_mt(fill));

  return true;
}


int main() {
  CxxUtils::ubsan_suppress ( []() { TInterpreter::Instance(); } );
  ISvcLocator* pSvcLoc;
  if ( !Athena_test::initGaudi( "GenericMon.txt",  pSvcLoc ) ) {
    std::cerr << "ERROR This test can not be run" << std::endl;
    return -1;
  }
  MsgStream log( Athena::getMessageSvc(), "GenericMonFilling_test" );
  log.setLevel(0);

  ITHistSvc* histSvc;
  if( pSvcLoc->service( "THistSvc", histSvc, true ).isFailure()  ) {
    log << MSG::ERROR << "THistSvc not available " << endmsg;
    return -1;
  }

  ISvcManager* svcmgr = dynamic_cast<ISvcManager*>( pSvcLoc );
  svcmgr->startServices().ignore();

  // we need to test what happens to the monitoring when tool is not valid
  ToolHandle<GenericMonitoringTool> emptyMon("");
  VALUE( emptyMon.isEnabled() ) EXPECTED( false ); // self test
  log << MSG::DEBUG << " mon tool validity " << emptyMon.isValid() << endmsg;

  ToolHandle<GenericMonitoringTool> validMon( "GenericMonitoringTool/MonTool" );
  if ( validMon.retrieve().isFailure() ) {
    log << MSG::ERROR << "Failed to acquire the MonTool tools via the ToolHandle" << endmsg;
    return -1;
  }

  log << MSG::DEBUG << "Histograms defined: " << histSvc->getHists() << endmsg;
  log << MSG::DEBUG << "fillFromScalarWorked" << endmsg;
  assert( fillFromScalarWorked( validMon, histSvc ) );
  log << MSG::DEBUG << "noToolBehaviourCorrect" << endmsg;
  assert( noToolBehaviourCorrect( emptyMon ) );
  log << MSG::DEBUG << "fillFromScalarIndependentScopesWorked" << endmsg;
  assert( fillFromScalarIndependentScopesWorked( validMon, histSvc ) );
  log << MSG::DEBUG << "fill2DWorked" << endmsg;
  assert( fill2DWorked( validMon, histSvc ) );
  log << MSG::DEBUG << "fillExplicitlyWorked" << endmsg;
  assert( fillExplicitlyWorked( validMon, histSvc ) );
  log << MSG::DEBUG << "fillWithCutMaskWorked" << endmsg;
  assert( fillWithCutMaskWorked( validMon, histSvc ) );
  log << MSG::DEBUG << "assignWorked" << endmsg;
  assert( assignWorked() );
  log << MSG::DEBUG << "operatorsWorked" << endmsg;
  assert( operatorsWorked() );
  log << MSG::DEBUG << "timerFillingWorked" << endmsg;
  assert( timerFillingWorked( validMon, histSvc ) );
  log << MSG::DEBUG << "stringFillingWorked" << endmsg;
  assert( stringFillingWorked( validMon, histSvc ) );
  log << MSG::DEBUG << "string2DFillingWorked" << endmsg;
  assert( string2DFillingWorked( validMon, histSvc ) );
  log << MSG::DEBUG << "All OK"  << endmsg;

  // Make sure that THistSvc gets finalized.
  // Otherwise, the output file will get closed while global dtors are running,
  // which can lead to crashes.
  svcmgr->stopServices().ignore();
  svcmgr->finalizeServices().ignore();

  return 0;
}
