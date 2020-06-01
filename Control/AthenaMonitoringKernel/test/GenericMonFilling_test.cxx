/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Unit tests and documentation examples
 *
 * Note that the snippets marked with:
 * //! [Example1]
 * ...
 * //! [Example1]
 * are used in the Doxygen documentation of the various classes.
 * If you change them make sure to check the resulting Doxygen.
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
  assert( histSvc->getHist( histName, h ).isSuccess() );
  VALUE( h ) NOT_EXPECTED( ( TH1* )nullptr );
  return h;
}

TTree* getTree( ITHistSvc* histSvc, const std::string& treeName ) {
  TTree* t( nullptr );
  assert( histSvc->getTree( treeName, t ).isSuccess() );
  VALUE( t ) NOT_EXPECTED( ( TTree* )nullptr );
  return t;
}

void resetHist( ITHistSvc* histSvc, const std::string& histName ) {
  TH1* h ATLAS_THREAD_SAFE = const_cast<TH1*>(getHist( histSvc, histName ));
  h->Reset();
  THashList* labels = h->GetXaxis()->GetLabels();
  if (labels) labels->Clear();
  labels = h->GetYaxis()->GetLabels();
  if (labels) labels->Clear();
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
  assert( histSvc->getHist( histName, h ).isSuccess() );
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
  auto group = Monitored::Group( monTool, x );
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

bool fillFromScalar( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {

  auto fill = [&]() {
    //! [fillFromScalar]
    auto eta = Monitored::Scalar<double>( "Eta" ); //explicit double
    auto phi = Monitored::Scalar( "Phi", -99.0 );  //deduced double
    auto group = Monitored::Group( monTool, eta, phi );
    phi = 0.1;
    eta = -0.2;
    //! [fillFromScalar]
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

bool fillFromScalarTrf( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {

  auto fill = [&]() {
    //! [fillFromScalarTrf]
    // Take absolute value before filling
    auto phi = Monitored::Scalar<double>( "Phi", -3.0, [](double d){ return fabs(d); } );
    //! [fillFromScalarTrf]
    auto group = Monitored::Group( monTool, phi );
    return 1;
  };
  auto check = [&](size_t N) {
    VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi" )->GetEntries() ) EXPECTED (N);
    VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi" )->GetMean() ) EXPECTED (3.0);
  };

  resetHists( histSvc ); check(fill());
  return true;
}

bool fillFromScalarIndependentScopes( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {
  resetHists( histSvc );
  //! [fillFromScalarIndependentScopes]
  // The variable are declared in an outer scope
  auto phi = Monitored::Scalar( "Phi", -99.0 );
  auto eta = Monitored::Scalar<double>( "Eta", -99 );

  for ( size_t i =0; i < 3; ++i ) {
    // Inside another scope the Group is instantiated and destroyed resulting in multiple histogram fills.
    auto group = Monitored::Group( monTool, phi );
    phi = 0.1;
    eta = -0.2;
  }
  //! [fillFromScalarIndependentScopes]

  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 1 ) ) EXPECTED( 0 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 2 ) ) EXPECTED( 3 );

  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 1 ) ) EXPECTED( 0 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 0 );

  for ( size_t i =0; i < 10; ++i ) {
    auto group = Monitored::Group( monTool, eta );
    phi = 0.1;
    eta = -0.2;
  }

  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 1 ) ) EXPECTED( 0 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 2 ) ) EXPECTED( 3 );

  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 1 ) ) EXPECTED( 10 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 0 );

  return true;
}

bool fill2D( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {

  auto fill = [&]() {
    //! [fill2D_correct]
    // For 2D histogram to be filled the two histogrammed variables need to be grouped.
    // This code will cause the 2D histogram fill
    auto phi = Monitored::Scalar( "Phi", -99.0 );
    auto eta = Monitored::Scalar( "Eta", -99.0 );
    
    auto group = Monitored::Group( monTool, eta, phi );
    eta = 0.2;
    phi = -0.1;
    //! [fill2D_correct]
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

  // 2D Hist fill should not affect 1D
  resetHists( histSvc );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 0 );
  {
    // This code will NOT cause a 2D histogram fill but instead
    // It will cause though 1D histogram fill if one is defined for this quantity.
    auto eta = Monitored::Scalar( "Eta", -99.0 );
    auto group = Monitored::Group( monTool, eta );
    eta = 0.2;
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 0 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 1 );

  {
    auto phi = Monitored::Scalar( "Phi", -99.0 );
    auto group = Monitored::Group( monTool, phi );
    phi = -0.1;
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 0 ); // still no entries as scope used above is not having both needed variables
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 1 ); // no increase of counts
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 1 ) ) EXPECTED( 1 );

  return true;
}


bool fillProfile( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {

  auto fill = [&]() {
    auto pt = Monitored::Scalar<double>( "pt", 3.0 );
    auto eta = Monitored::Scalar( "Eta", 0.2 );
    auto group = Monitored::Group( monTool, eta, pt );
    return 1;
  };

  auto check = [&](size_t N) {
    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/pt_vs_Eta", 1 ) ) EXPECTED( 0 );
    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/pt_vs_Eta", 2 ) ) EXPECTED( 3 );
    VALUE( getHist( histSvc, "/EXPERT/TestGroup/pt_vs_Eta" )->GetEntries() ) EXPECTED( N );
  };

  resetHists( histSvc ); check(fill());
  resetHists( histSvc ); check(fill_mt(fill));

  return true;
}


bool fillExplicitly( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {
  resetHists( histSvc );
  //! [fillExplicitly_noop]
  auto phi = Monitored::Scalar( "Phi", -99.0 );
  auto eta = Monitored::Scalar( "Eta", -99.0 );
  {
    auto group = Monitored::Group( monTool, eta, phi );
    group.setAutoFill( false ); // results in no histogram fills
    eta = 0.2;
    phi = -0.1;
  }
  //! [fillExplicitly_noop]
  
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 0 ); //  auto filling was disabled so no entries
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta" )->GetEntries() ) EXPECTED( 0 ); //  auto filling was disabled so no entries
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi" )->GetEntries() ) EXPECTED( 0 ); //  auto filling was disabled so no entries

  // Check explicit fill in loops
  {
    //! [fillExplicitly_fill]
    auto group = Monitored::Group( monTool, eta, phi );
    for ( size_t i = 0; i < 3; ++i ) {
      group.fill();   // this will fill and disable AutoFill
    }
    //! [fillExplicitly_fill]
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 3 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta" )->GetEntries() ) EXPECTED( 3 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi" )->GetEntries() ) EXPECTED( 3 );

  // Check explicit one-time fill via temporary Group instance
  {
    Monitored::Group( monTool, eta, phi ).fill();
  }
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 4 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta" )->GetEntries() ) EXPECTED( 4 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi" )->GetEntries() ) EXPECTED( 4 );

  return true;
}

bool fillWithCutMask( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {

  auto fill1 = [&]() {
    //! [fillWithCutMask]
    for (int ctr = 0; ctr < 10; ++ctr) {
      auto eta = Monitored::Scalar<double>( "Eta", -99 );
      auto cutMask = Monitored::Scalar<bool>( "CutMask", (ctr % 2) == 0); // filter every second entry
      auto group = Monitored::Group( monTool, eta, cutMask );
      eta = -0.2;
      //! [fillWithCutMask]
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
    //! [fillWithCutMask_collection]
    std::vector<float> etaVec{-0.2, 0.2, -0.4, 0.4, -0.6};
    std::vector<char> cutMaskVec{0, 1, 1, 1, 0};
    auto eta = Monitored::Collection( "Eta", etaVec );
    auto cutMask = Monitored::Collection( "CutMask", cutMaskVec );
    auto group = Monitored::Group( monTool, eta, cutMask );
    //! [fillWithCutMask_collection]
    return 3;
  };
  auto check2 = [&](size_t N) {
    VALUE( getHist( histSvc, "/EXPERT/TestGroup/Eta_CutMask" )->GetEntries() ) EXPECTED( N );
  };
  resetHists( histSvc ); check2(fill2());
  resetHists( histSvc ); check2(fill_mt(fill2));

  return true;
}

bool fillWithWeight( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {

  auto fill = [&]() {
    //! [fillWithWeight]
    auto pt = Monitored::Scalar<double>( "pt", 3.0 );
    auto weight = Monitored::Scalar<float>( "Weight", 0.5 );
    auto group = Monitored::Group( monTool, pt, weight );
    //! [fillWithWeight]
    return 1;
  };
  auto check = [&](size_t N) {
    VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/pt", 4 ) ) EXPECTED( 0.5*N );
  };
  resetHists( histSvc ); check(fill());
  resetHists( histSvc ); check(fill_mt(fill));

  auto fill2 = [&]() {
    //! [fillWithWeight_collection]
    std::vector<double> ptvec{1.0, 2.0, 3.0 ,4.0, 5.0};
    std::vector<float>   wvec{0.0, 0.5, 0.5, 1.0, 2.0};
    auto pt = Monitored::Collection( "pt", ptvec );
    auto weight = Monitored::Collection( "Weight", wvec );
    auto group = Monitored::Group( monTool, pt, weight );
    //! [fillWithWeight_collection]
    return 5;
  };
  auto check2 = [&](size_t N) {
    VALUE( getHist( histSvc, "/EXPERT/TestGroup/pt" )->GetEntries() ) EXPECTED( N );
    VALUE( getHist( histSvc, "/EXPERT/TestGroup/pt" )->GetMean() ) EXPECTED( 4.125 );
  };
  resetHists( histSvc ); check2(fill2());
  resetHists( histSvc ); check2(fill_mt(fill2));

  return true;
}

/// Example of custom scalar class
class Scalar {
public:
    Scalar() : m_value( 0 ) { }
    Scalar( double value ) : m_value( value ) { }

    void operator=( double value ) { m_value = value; }
    operator double() const { return m_value; }
private:
    double m_value;
};

/// Example track class
class Track {
public:
    Track() : m_eta( 0 ), m_phi( 0 ) {}
    Track( double e, double p ) : m_eta( e ), m_phi( p ) {}
    double eta() const { return m_eta; }
    double phi() const { return m_phi; }
private:
    double m_eta, m_phi;
};


bool fillFromNonTrivialSources( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {
  resetHists( histSvc );
  {
    auto eta = Monitored::Scalar( "Eta", Scalar( 0.2 ) ); //works when object to number conversion defined
    auto group = Monitored::Group( monTool, eta );
  }
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 1 );

  resetHists( histSvc );
  {
    //! [fillFromNonTrivialSources_lambda]
    std::vector<float> v{1,2,3};
    auto eta = Monitored::Scalar<int>( "Eta", [&](){ return v.size(); } );
    //! [fillFromNonTrivialSources_lambda]
    auto group = Monitored::Group( monTool, eta );
  }
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 1 );

  resetHists( histSvc );
  {
    //! [fillFromNonTrivialSources_collection]
    std::vector<float> eta( {0.2, 0.1} );
    std::vector<double> phi( {-1, 1} ) ;
    auto vectorT   = Monitored::Collection( "Eta", eta );
    auto setT      = Monitored::Collection( "Phi", phi );
    auto group = Monitored::Group( monTool, vectorT, setT );
    //! [fillFromNonTrivialSources_collection]
  }
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 2 );

  resetHists( histSvc );
  {
    //! [fillFromNonTrivialSources_array]
    std::array<double, 2> eta( {{0.1, 0.7}} );
    double phi[2] = {-2., -1.};
    auto arrayT = Monitored::Collection( "Eta", eta );
    auto rawArrayT = Monitored::Collection( "Phi", phi );
    auto group = Monitored::Group( monTool, arrayT, rawArrayT );
    //! [fillFromNonTrivialSources_array]
  }
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 2 );

  resetHists( histSvc );
  {
    //! [fillFromNonTrivialSources_obj_collection]
    Track tracks[2];
    auto eta = Monitored::Collection( "Eta", tracks, &Track::eta );
    auto phi = Monitored::Collection( "Phi", tracks, []( const Track& t ) { return t.phi(); } );

    auto group = Monitored::Group( monTool, eta, phi ); // this is binding to histograms

    tracks[0] = Track( 0.1, 0.9 );
    tracks[1] = Track( 1.3, 1. );
    //! [fillFromNonTrivialSources_obj_collection]
  }
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Eta", 2 ) ) EXPECTED( 1 );
  VALUE( contentInBin1DHist( histSvc, "/EXPERT/TestGroup/Phi", 2 ) ) EXPECTED( 1 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/Phi_vs_Eta" )->GetEntries() ) EXPECTED( 2 );

  return true;
}

bool assign() {
  //! [assign]
  auto eta = Monitored::Scalar( "Eta", -3. );
  eta = 0.6;
  //! [assign]
  VALUE ( double( eta ) ) EXPECTED ( 0.6 );
  auto etaBis = Monitored::Scalar( "EtaBis", 0. );
  etaBis = 0.4;
  VALUE( double( etaBis ) ) EXPECTED( 0.4 );
  etaBis = double( eta );
  VALUE( double( etaBis ) ) EXPECTED( 0.6 );
  return true;
}

bool operators() {
  //! [operators_comp]
  auto count = Monitored::Scalar<float>( "Count", 0 );
  bool comparisonResult = count == count;
  //! [operators_comp]
  
  VALUE( comparisonResult ) EXPECTED (true);
  count += 1;
  VALUE ( int(count) ) EXPECTED (1);
  count++;
  VALUE ( int(count) ) EXPECTED (2);
  --count;
  VALUE ( int(count) ) EXPECTED (1);
  count *= 3;
  VALUE ( int(count) ) EXPECTED (3);

  //! [operators_examples]
  count += 1;
  count++;
  --count;
  count *= 3;
  //! [operators_examples]

  return true;
}

bool timerFilling( ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc ) {
  //! [timerFilling]
  // The name of the monitored timer has to start with "TIME", else runtime error.
  auto t1 = Monitored::Timer( "TIME_t1" );  // default is microseconds
  auto t2 = Monitored::Timer<std::chrono::milliseconds>( "TIME_t2" );
  {
    auto group = Monitored::Group( monTool, t1, t2 );
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  //! [timerFilling]
  // There should be one entry in the histogram with roughly 10ms
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/TIME_t1" )->GetEntries() ) EXPECTED( 1 );
  VALUE( getHist( histSvc, "/EXPERT/TestGroup/TIME_t2" )->GetEntries() ) EXPECTED( 1 );
  double t1_value = getHist( histSvc, "/EXPERT/TestGroup/TIME_t1" )->GetMean();
  double t2_value = getHist( histSvc, "/EXPERT/TestGroup/TIME_t2" )->GetMean();
  assert( 8000 < t1_value && t1_value < 12000 );
  assert( 8 < t2_value && t2_value < 12 );

  // Test scoped timer
  auto t3 = Monitored::Timer<std::chrono::milliseconds>( "TIME_t3" );
  {
    auto group = Monitored::Group( monTool, t3 );
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


bool stringFilling(ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc) {

  auto fill = [&]() {
    //! [stringFilling]
    auto det = Monitored::Scalar<std::string>( "DetID", "SCT" );
    Monitored::Group(monTool, det);
    det = "PIX";
    Monitored::Group(monTool, det);
    //! [stringFilling]
    return 2;
  };
  auto check = [&](size_t N) {
    const TH1* h = getHist( histSvc, "/EXPERT/TestGroup/DetID" );
    VALUE( h->GetEntries() ) EXPECTED( N );
    VALUE( h->GetXaxis()->GetLabels()->GetEntries() ) EXPECTED( 2 );
    const int sctBin = h->GetXaxis()->FindFixBin("SCT");
    VALUE( sctBin ) EXPECTED( 1 );
    VALUE( h->GetBinContent( sctBin ) ) EXPECTED( N/2 );
  };

  resetHists( histSvc ); check(fill());
  resetHists( histSvc ); check(fill_mt(fill));

  return true;
}

bool stringFillingGen(ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc) {

  auto fill = [&]() {
    auto det = Monitored::Scalar<std::string>( "DetID", [&](){return "SCT";} );
    Monitored::Group(monTool, det);
    return 1;
  };
  auto check = [&](size_t N) {
    const TH1* h = getHist( histSvc, "/EXPERT/TestGroup/DetID" );
    VALUE( h->GetEntries() ) EXPECTED( N );
    VALUE( h->GetXaxis()->FindFixBin("SCT") ) EXPECTED( 1 );
  };

  resetHists( histSvc ); check(fill());
  resetHists( histSvc ); check(fill_mt(fill));

  return true;
}


bool stringFromCollection(ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc) {

  auto fill = [&]() {
    //! [stringFromCollection]
    struct StringInObject {
      int layer;
      std::string name;
      const std::string& getName() const { return name; }
    };
    std::vector<StringInObject> testData({{0, "PIX"}, {1, "PIX"}, {3, "SCT"}, {1, "PIX"}});
    auto name = Monitored::Collection("DetID", testData,  [](const StringInObject& s){ return s.getName(); }); // lambda as accessor

    auto ignored1 = Monitored::Collection("ignored", testData,  &StringInObject::getName  ); // access via member function
    auto ignored2 = Monitored::Collection("ignored", testData,  [](const StringInObject& s){ return s.getName().c_str(); }  ); // accessor returning const char* is supported
    Monitored::Group(monTool, name);
    //! [stringFromCollection]
    return 4; // number of entries
  };

  auto check = [&](size_t N) {
    const TH1* h = getHist( histSvc, "/EXPERT/TestGroup/DetID" );
    VALUE( h->GetEntries() ) EXPECTED( N );
    VALUE( h->GetXaxis()->GetLabels()->GetEntries() ) EXPECTED( 2 ); // two distinct strings used in input data
  };

  resetHists( histSvc ); check(fill());
  resetHists( histSvc ); check(fill_mt(fill));

  return true;
}

bool string2DFilling(ToolHandle<GenericMonitoringTool>& monTool, ITHistSvc* histSvc) {

  auto fill = [&]() {
    //! [string2DFilling]
    auto countID = Monitored::Scalar<std::string>( "DetID", "SCT" );
    std::vector<std::string> caloLabels( { "LAr", "LAr", "Tile" } );
    auto countCalo = Monitored::Collection<std::vector<std::string>>( "DetCalo", caloLabels );
    auto x = Monitored::Scalar("x", 1.2 );
    std::vector<double> yvalues({0.2, 2.1, 1.3});
    auto y = Monitored::Collection("y", yvalues );
    
    {
      // this will fill: (SCT,LAr) (SCT,LAr) (SCT,Tile)
      Monitored::Group(monTool, countID, countCalo);
    }
    {
      // this will fill: (LAr,0.2) (LAr,2.1) (Tile,1.3)
      Monitored::Group(monTool, countCalo, y);
    }
    {
      // this will fill: (LAr,1.2) (LAr,1.2) (Tile,1.2)
      Monitored::Group(monTool, countCalo, x);
    }
    //! [string2DFilling]
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
  log << MSG::DEBUG << "fillFromScalar" << endmsg;
  assert( fillFromScalar( validMon, histSvc ) );
  log << MSG::DEBUG << "fillFromScalarTrf" << endmsg;
  assert( fillFromScalarTrf( validMon, histSvc ) );
  log << MSG::DEBUG << "noToolBehaviourCorrect" << endmsg;
  assert( noToolBehaviourCorrect( emptyMon ) );
  log << MSG::DEBUG << "fillFromScalarIndependentScopes" << endmsg;
  assert( fillFromScalarIndependentScopes( validMon, histSvc ) );
  log << MSG::DEBUG << "fill2D" << endmsg;
  assert( fill2D( validMon, histSvc ) );
  log << MSG::DEBUG << "fillProfile" << endmsg;
  assert( fillProfile( validMon, histSvc ) );
  log << MSG::DEBUG << "fillExplicitly" << endmsg;
  assert( fillExplicitly( validMon, histSvc ) );
  log << MSG::DEBUG << "fillWithCutMask" << endmsg;
  assert( fillWithCutMask( validMon, histSvc ) );
  log << MSG::DEBUG << "fillWithWeight" << endmsg;
  assert( fillWithWeight( validMon, histSvc ) );
  log << MSG::DEBUG << "assign" << endmsg;
  assert( assign() );
  log << MSG::DEBUG << "operators" << endmsg;
  assert( operators() );
  log << MSG::DEBUG << "timerFilling" << endmsg;
  assert( timerFilling( validMon, histSvc ) );
  log << MSG::DEBUG << "stringFilling" << endmsg;
  assert( stringFilling( validMon, histSvc ) );
  log << MSG::DEBUG << "stringFillingGen" << endmsg;
  assert( stringFillingGen( validMon, histSvc ) );
  log << MSG::DEBUG << "string2DFilling" << endmsg;
  assert( string2DFilling( validMon, histSvc ) );
  log << MSG::DEBUG << "stringFromCollection" << endmsg;
  assert( stringFromCollection( validMon, histSvc ) );
  log << MSG::DEBUG << "All OK"  << endmsg;


  // Make sure that THistSvc gets finalized.
  // Otherwise, the output file will get closed while global dtors are running,
  // which can lead to crashes.
  svcmgr->stopServices().ignore();
  svcmgr->finalizeServices().ignore();

  return 0;
}
