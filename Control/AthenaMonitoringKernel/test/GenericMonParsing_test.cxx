/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#undef NDEBUG
#include <cassert>
#include "TestTools/expect.h"
#include "TestTools/expect_exception.h"
#include "AthenaMonitoringKernel/HistogramDef.h"
using namespace Monitored;





bool parsing1DWorks() {
  auto def = HistogramDef::parse("EXPERT, TH1F, , , Eta, #eta of Clusters; #eta; number of RoIs, 50, -2.500000, 2.500000");
  VALUE ( def.ok )          EXPECTED ( true );  
  VALUE ( def.path )        EXPECTED ( "EXPERT" );
  VALUE ( def.type )        EXPECTED ( "TH1F" );
  VALUE ( def.name.size() ) EXPECTED ( 1 );
  VALUE ( std::string(def.name[0]) )     EXPECTED ( "Eta" );
  VALUE ( def.title )       EXPECTED ( "#eta of Clusters; #eta; number of RoIs" );
  VALUE ( def.xbins )       EXPECTED ( 50 );
  VALUE ( def.xmin )        EXPECTED ( -2.5 );
  VALUE ( def.xmax )        EXPECTED ( 2.5 );
  return true;
}

bool parsing2DWorks() {
  auto def = HistogramDef::parse("SHIFT, TH2F, , , Eta,Phi, #eta vs #phi of Clusters; #eta; #phi, 50, -2.500000, 2.500000, 64, -3.200000, 3.200000");
  VALUE ( def.ok )           EXPECTED ( true ) ;
  VALUE ( def.path )         EXPECTED ( "SHIFT" );
  VALUE ( def.type )         EXPECTED ( "TH2F" );
  VALUE ( def.name.size() )  EXPECTED ( 2 );
  VALUE ( std::string( def.name[0] ) ) EXPECTED ( "Eta");
  VALUE ( std::string( def.name[1] ) ) EXPECTED ( "Phi");
  VALUE ( def.alias ) EXPECTED( "Eta_vs_Phi" );
  VALUE ( def.title )        EXPECTED ( "#eta vs #phi of Clusters; #eta; #phi" );
  VALUE ( def.xbins )        EXPECTED ( 50 );
  VALUE ( def.xmin )         EXPECTED ( -2.5 );
  VALUE ( def.xmax )         EXPECTED (  2.5 );
  VALUE ( def.ybins )        EXPECTED ( 64 );
  VALUE ( def.ymin )         EXPECTED ( -3.2 );
  VALUE ( def.ymax )         EXPECTED (  3.2 );
  
  return true;
}

bool parsing3DWorks() {
  auto def = HistogramDef::parse("SHIFT, TProfile2D, , , Eta,Phi,pt, title, 50, -2.500000, 2.500000, 64, -3.200000, 3.200000, -1.000000, 1.000000");
  VALUE ( def.ok )           EXPECTED ( true ) ;
  VALUE ( def.path )         EXPECTED ( "SHIFT" );
  VALUE ( def.type )         EXPECTED ( "TProfile2D" );
  VALUE ( def.name.size() )  EXPECTED ( 3 );
  VALUE ( std::string( def.name[0] ) ) EXPECTED ( "Eta");
  VALUE ( std::string( def.name[1] ) ) EXPECTED ( "Phi");
  VALUE ( def.alias ) EXPECTED( "Eta_vs_Phi_vs_pt" );
  VALUE ( def.title )        EXPECTED ( "title" );
  VALUE ( def.xbins )        EXPECTED ( 50 );
  VALUE ( def.xmin )         EXPECTED ( -2.5 );
  VALUE ( def.xmax )         EXPECTED (  2.5 );
  VALUE ( def.ybins )        EXPECTED ( 64 );
  VALUE ( def.ymin )         EXPECTED ( -3.2 );
  VALUE ( def.ymax )         EXPECTED (  3.2 );
  VALUE ( def.zmin )         EXPECTED ( -1.0 );
  VALUE ( def.zmax )         EXPECTED (  1.0 );
  
  return true;
}

bool parsingLabeledWorks() {
  auto def = HistogramDef::parse("SHIFT, TH1D, , , Cut, Cut counter, 5, 0, 5, Cut1:Cut2:Eta:Pt:R");
  VALUE ( def.ok )           EXPECTED ( true ) ;
  VALUE ( def.path )         EXPECTED ( "SHIFT" );
  VALUE ( def.type )         EXPECTED ( "TH1D" );
  VALUE ( def.name.size() )  EXPECTED ( 1 );
  VALUE ( std::string( def.name[0]) ) EXPECTED ( "Cut");
  VALUE ( def.labels.size() )EXPECTED ( 5 );
  VALUE ( std::string( def.labels[0] ) ) EXPECTED ( "Cut1");
  VALUE ( std::string( def.labels[1] ) ) EXPECTED ( "Cut2");
  VALUE ( std::string( def.labels[2] ) ) EXPECTED ( "Eta");
  VALUE ( std::string( def.labels[3] ) ) EXPECTED ( "Pt");
  VALUE ( std::string( def.labels[4] ) ) EXPECTED ( "R");

  return true;
}

bool parsingWeightedWorks() {
  auto def = HistogramDef::parse("EXPERT, TH1F, Weight, , var, title, 5, 0, 5");
  VALUE ( def.ok )                   EXPECTED ( true );
  VALUE ( def.path )                 EXPECTED ( "EXPERT" );
  VALUE ( def.type )                 EXPECTED ( "TH1F" );
  VALUE ( def.weight )               EXPECTED ( "Weight" );
  VALUE ( def.name.size() )          EXPECTED ( 1 );
  VALUE ( std::string(def.name[0]) ) EXPECTED ( "var" );

  return true;
}

bool parsing1DArrayWorks() {
  auto def = HistogramDef::parse("EXPERT, TH1F, , , var, title, 0:1:2:4:8");
  VALUE ( def.ok )    EXPECTED ( true );
  VALUE ( def.xbins ) EXPECTED ( 4 );
  VALUE ( std::equal(def.xArray.begin(),def.xArray.end(),std::vector<double>({0,1,2,4,8}).begin()) ) EXPECTED ( true );
  return true;
}

bool parsing2DArrayWorks() {
  auto def = HistogramDef::parse("EXPERT, TH2F, , , var1,var2, title, 0:1:2:4:8, 0:4:6:7");
  VALUE ( def.ok )    EXPECTED ( true );
  VALUE ( def.xbins ) EXPECTED ( 4 );
  VALUE ( std::equal(def.xArray.begin(),def.xArray.end(),std::vector<double>({0,1,2,4,8}).begin()) ) EXPECTED ( true );
  VALUE ( def.ybins ) EXPECTED ( 3 );
  VALUE ( std::equal(def.yArray.begin(),def.yArray.end(),std::vector<double>({0,4,6,7}).begin()) ) EXPECTED ( true );
  return true;
}

bool badDefGeneratesExecption() {
  EXPECT_EXCEPTION( HistogramDefParseException,
		    HistogramDef::parse("S, TH1D, , x, 2.5, 0, 45") ); //2.5 bins can not be valid
  return true;
}

int main() {
  assert( parsing1DWorks() );
  assert( parsing2DWorks() );
  assert( parsing2DWorks() );
  assert( parsing3DWorks() );
  assert( parsingLabeledWorks() );
  assert( parsingWeightedWorks() );
  assert( parsing1DArrayWorks() );
  assert( parsing2DArrayWorks() );
  assert( badDefGeneratesExecption() );

  std::cout << "all ok" << std::endl;
  return 0;
}
