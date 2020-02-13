/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoringKernel/HistogramDef.h"
#include <nlohmann/json.hpp>

using namespace Monitored;
using json = nlohmann::json;

const HistogramDef HistogramDef::parse(const std::string& histogramDefinition) {
  const json& setting = json::parse(histogramDefinition);
  HistogramDef result;

  result.name = setting["allvars"].get<std::vector<std::string>>();
  result.alias = setting["alias"];
  result.type = setting["type"];
  result.path = setting["path"];
  result.title = setting["title"];
  result.opt = setting["opt"];
  result.convention = setting["convention"];
  result.weight = setting["weight"];

  result.xvar = setting["xvar"];
  result.xbins = setting["xbins"];
  result.xmin = setting["xmin"];
  result.xmax = setting["xmax"];
  result.xlabels = setting["xlabels"].get<std::vector<std::string>>();
  result.xarray = setting["xarray"].get<std::vector<double>>();

  result.yvar = setting["yvar"];
  result.ybins = setting["ybins"];
  result.ymin = setting["ymin"];
  result.ymax = setting["ymax"];
  result.ylabels = setting["ylabels"].get<std::vector<std::string>>();
  result.yarray = setting["yarray"].get<std::vector<double>>();

  result.zvar = setting["zvar"];
  result.zbins = setting["zbins"];
  result.zmin = setting["zmin"];
  result.zmax = setting["zmax"];
  result.zlabels = setting["zlabels"].get<std::vector<std::string>>();

  result.ok = true;
  return result;
}
