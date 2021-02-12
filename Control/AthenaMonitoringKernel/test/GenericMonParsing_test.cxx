/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#undef NDEBUG
#include <cassert>
#include <nlohmann/json.hpp>
#include "TestTools/expect.h"
#include "TestTools/expect_exception.h"
#include "AthenaMonitoringKernel/HistogramDef.h"
using namespace Monitored;
using json = nlohmann::json;

json defaultJson() {
  json j;
  j["alias"] = "var";
  j["allvars"] = json::array({"var"});
  j["convention"] = "";
  j["opt"] = "";
  j["path"] = "";
  j["title"] = "var";
  j["type"] = "TH1F";
  j["weight"] = "";
  j["cutMask"] = "";
  j["xarray"] = json::array();
  j["xbins"] = 100;
  j["xlabels"] = json::array();
  j["xmax"] = 1.0;
  j["xmin"] = 0.0;
  j["xvar"] = "var";
  j["yarray"] = json::array();
  j["ybins"] = 0;
  j["ylabels"] = json::array();
  j["ymax"] = 0.0;
  j["ymin"] = 0.0;
  j["yvar"] = "";
  j["zbins"] = 0;
  j["zlabels"] = json::array();
  j["zmax"] = 0.0;
  j["zmin"] = 0.0;
  j["zvar"] = "";
  j["Sumw2"] = false;
  j["kLBNHistoryDepth"] = 0;
  j["kAddBinsDynamically"] = false;
  j["kRebinAxes"] = false;
  j["kCanRebin"] = false;
  j["kVec"] = false;
  j["kVecUO"] = false;
  j["kCumulative"] = false;
  j["kLive"] = false;
  j["merge"] = "";
  j["treeDef"] = "";
  return j;
}

bool parse1D() {
  json check = defaultJson();
  auto def = HistogramDef::parse(check.dump());

  VALUE ( def.ok ) EXPECTED ( true );
  VALUE ( def.alias ) EXPECTED ( "var" );
  VALUE ( def.type ) EXPECTED ( "TH1F" );
  VALUE ( def.name.size() ) EXPECTED ( 1 );
  VALUE ( def.xvar ) EXPECTED ( "var" );
  VALUE ( def.xbins ) EXPECTED ( 100 );
  VALUE ( def.xmin ) EXPECTED ( 0.0 );
  VALUE ( def.xmax ) EXPECTED ( 1.0 );

  return true;
}

bool parse1D_options() {
  json check = defaultJson();
  check["xbins"] = 10;
  check["xmin"] = -1.0;
  check["xmax"] = 1.0;
  check["title"] = "toptitle; xtitle; ytitle";
  check["Sumw2"] = true;
  check["kLBNHistoryDepth"] = 99;
  check["path"] = "mypath/tohistograms";
  check["type"] = "TH1D";
  check["weight"] = "myweight";
  check["cutMask"] = "mycutmask";
  auto def = HistogramDef::parse(check.dump());

  VALUE ( def.ok ) EXPECTED ( true );
  VALUE ( def.xbins ) EXPECTED ( 10 );
  VALUE ( def.xmin ) EXPECTED ( -1.0 );
  VALUE ( def.xmax ) EXPECTED ( 1.0 );
  VALUE ( def.title ) EXPECTED ( "toptitle; xtitle; ytitle" );
  VALUE ( def.Sumw2 ) EXPECTED ( true );
  VALUE ( def.kLBNHistoryDepth ) EXPECTED ( 99 );
  VALUE ( def.path ) EXPECTED ( "mypath/tohistograms" );
  VALUE ( def.type ) EXPECTED ( "TH1D" );
  VALUE ( def.weight ) EXPECTED ( "myweight" );
  VALUE ( def.cutMask ) EXPECTED ( "mycutmask" );

  return true;
}

bool parse2D() {
  json check = defaultJson();
  check["type"] = "TH2F";
  check["allvars"] = {"varX", "varY"};
  check["alias"] = "varX_vs_varY";
  check["xvar"] = "varX";
  check["yvar"] = "varY";
  check["ybins"] = 10;
  check["ymin"] = 0.0;
  check["ymax"] = 20.0;
  check["title"] = "X vs. Y; X [x unit]; Y [y unit]";
  auto def = HistogramDef::parse(check.dump());

  VALUE ( def.ok ) EXPECTED ( true );
  VALUE ( def.type ) EXPECTED ( "TH2F" );
  VALUE ( def.name.size() ) EXPECTED ( 2 );
  VALUE ( std::string(def.name[0]) ) EXPECTED ( "varX" );
  VALUE ( std::string(def.name[1]) ) EXPECTED ( "varY" );
  VALUE ( def.alias ) EXPECTED( "varX_vs_varY" );
  VALUE ( def.title ) EXPECTED ( "X vs. Y; X [x unit]; Y [y unit]" );
  VALUE ( def.xbins ) EXPECTED ( 100 );
  VALUE ( def.xmin ) EXPECTED ( 0.0 );
  VALUE ( def.xmax ) EXPECTED ( 1.0 );
  VALUE ( def.ybins ) EXPECTED ( 10 );
  VALUE ( def.ymin ) EXPECTED ( 0.0 );
  VALUE ( def.ymax ) EXPECTED ( 20.0 );

  return true;
}

bool parse3D() {
  json check = defaultJson();
  check["type"] = "TProfile2D";
  check["allvars"] = {"varX", "varY", "varZ"};
  check["alias"] = "varX_vs_varY_vs_varZ";
  check["xvar"] = "varX";
  check["yvar"] = "varY";
  check["zvar"] = "varZ";
  check["ybins"] = 10;
  check["ymin"] = 0.0;
  check["ymax"] = 20.0;
  check["zmin"] = -1.0;
  check["zmax"] = 1.0;
  check["title"] = "X vs. Y vs. Z; X [x unit]; Y [y unit]; Z [z unit]";
  auto def = HistogramDef::parse(check.dump());

  VALUE ( def.ok ) EXPECTED ( true );
  VALUE ( def.type ) EXPECTED ( "TProfile2D" );
  VALUE ( def.name.size() ) EXPECTED ( 3 );
  VALUE ( std::string(def.name[0]) ) EXPECTED ( "varX" );
  VALUE ( std::string(def.name[1]) ) EXPECTED ( "varY" );
  VALUE ( std::string(def.name[2]) ) EXPECTED ( "varZ" );
  VALUE ( def.alias ) EXPECTED( "varX_vs_varY_vs_varZ" );
  VALUE ( def.title ) EXPECTED ( "X vs. Y vs. Z; X [x unit]; Y [y unit]; Z [z unit]" );
  VALUE ( def.xbins ) EXPECTED ( 100 );
  VALUE ( def.xmin ) EXPECTED ( 0.0 );
  VALUE ( def.xmax ) EXPECTED ( 1.0 );
  VALUE ( def.ybins ) EXPECTED ( 10 );
  VALUE ( def.ymin ) EXPECTED ( 0.0 );
  VALUE ( def.ymax ) EXPECTED ( 20.0 );
  VALUE ( def.zmin ) EXPECTED ( -1.0 );
  VALUE ( def.zmax ) EXPECTED ( 1.0 );
  
  return true;
}

int main() {
  assert( parse1D() );
  assert( parse1D_options() );
  assert( parse2D() );
  assert( parse3D() );

  std::cout << "all ok" << std::endl;
  return 0;
}
