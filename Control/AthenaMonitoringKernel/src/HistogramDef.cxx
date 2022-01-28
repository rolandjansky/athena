/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoringKernel/HistogramDef.h"
#include <nlohmann/json.hpp>

using namespace Monitored;
using json = nlohmann::json;

const HistogramDef HistogramDef::parse(const std::string& histogramDefinition) {
  const json& setting = json::parse(histogramDefinition);
  HistogramDef result;

  result.name = setting["allvars"].get<std::vector<std::string>>();
  result.alias = static_cast<std::string>(setting["alias"]);
  result.type = static_cast<std::string>(setting["type"]);
  result.path = static_cast<std::string>(setting["path"]);
  result.title = static_cast<std::string>(setting["title"]);
  result.weight = static_cast<std::string>(setting["weight"]);
  result.cutMask = static_cast<std::string>(setting["cutMask"]);

  result.convention = static_cast<std::string>(setting["convention"]);
  size_t offline = result.convention.find("OFFLINE");
  if (offline != std::string::npos) {
    result.runmode = RunMode::Offline;
    std::string durationString = result.convention.substr(offline);
    if (durationString.find("run") != std::string::npos) {
      result.runperiod = RunPeriod::Run;
    } else if (durationString.find("lowStat") != std::string::npos) {
      result.runperiod = RunPeriod::LowStat;
    } else {
      result.runperiod = RunPeriod::Lumiblock;
    }
  } else {
    result.runmode = RunMode::Online;
  }

  // all histogram options arising from 'opt'
  result.Sumw2 = setting["Sumw2"];
  result.kLBNHistoryDepth = setting["kLBNHistoryDepth"];
  result.kAddBinsDynamically = setting["kAddBinsDynamically"];
  result.kRebinAxes = setting["kRebinAxes"];
  result.kCanRebin = setting["kCanRebin"];
  result.kVec = setting["kVec"];
  result.kVecUO = setting["kVecUO"];
  result.kCumulative = setting["kCumulative"];
  result.kLive = setting["kLive"];
  result.kAlwaysCreate = setting["kAlwaysCreate"];

  result.xvar = static_cast<std::string>(setting["xvar"]);
  result.xbins = setting["xbins"];
  result.xmin = setting["xmin"];
  result.xmax = setting["xmax"];
  result.xlabels = setting["xlabels"].get<std::vector<std::string>>();
  result.xarray = setting["xarray"].get<std::vector<double>>();

  result.yvar = static_cast<std::string>(setting["yvar"]);
  result.ybins = setting["ybins"];
  result.ymin = setting["ymin"];
  result.ymax = setting["ymax"];
  result.ylabels = setting["ylabels"].get<std::vector<std::string>>();
  result.yarray = setting["yarray"].get<std::vector<double>>();

  result.zvar = static_cast<std::string>(setting["zvar"]);
  result.zbins = setting["zbins"];
  result.zmin = setting["zmin"];
  result.zmax = setting["zmax"];
  result.zlabels = setting["zlabels"].get<std::vector<std::string>>();

  result.merge = static_cast<std::string>(setting["merge"]);

  result.treeDef = static_cast<std::string>(setting["treeDef"]);

  result.ok = true;
  return result;
}
