/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#undef NDEBUG
#include <cassert>
#include "TestTools/expect.h"
#include "AthenaMonitoring/HistogramDef.h"
using namespace Monitored;





bool parsing1DWorks() {
  auto def = HistogramDef::parse("EXPERT, TH1F, Eta, #eta of Clusters; #eta; number of RoIs, 50, -2.500000, 2.500000, ");
  VALUE ( def.ok )          EXPECTED ( true );  
  VALUE ( def.path )        EXPECTED ( "EXPERT" );
  VALUE ( def.type )        EXPECTED ( "TH1F" );
  VALUE ( def.name.size() ) EXPECTED ( 1 );
  VALUE ( std::string(def.name[0]) )     EXPECTED ( "Eta" );
  VALUE ( def.xbins )       EXPECTED ( 50 );
  VALUE ( def.xmin )        EXPECTED ( -2.5 );
  VALUE ( def.xmax )        EXPECTED ( 2.5 );
  return true;
}

bool parsing2DWorks() {
  auto def = HistogramDef::parse("SHIFT, TH2F, Eta,Phi, #eta vs #phi of Clusters; #eta; #phi, 50, -2.500000, 2.500000,64, -3.200000, 3.200000, ");
  VALUE ( def.ok )           EXPECTED ( true ) ;
  VALUE ( def.path )         EXPECTED ( "SHIFT" );
  VALUE ( def.type )         EXPECTED ( "TH2F" );
  VALUE ( def.name.size() )  EXPECTED ( 2 );
  VALUE ( std::string(def.name[0]) ) EXPECTED ( "Eta");
  VALUE ( std::string(def.name[1]) ) EXPECTED ( "Phi");
  VALUE ( def.xbins )        EXPECTED ( 50 );
  VALUE ( def.xmin )         EXPECTED ( -2.5 );
  VALUE ( def.xmax )         EXPECTED (  2.5 );
  VALUE ( def.ybins )        EXPECTED ( 64 );
  VALUE ( def.ymin )         EXPECTED ( -3.2 );
  VALUE ( def.ymax )         EXPECTED (  3.2 );
  
  return true;
}

int main() {
  assert( parsing1DWorks() );
  assert( parsing2DWorks() );
  // TODO extend to 3D and labeled histograms
  std::cout << "all ok" << std::endl;
  return 0;
}
