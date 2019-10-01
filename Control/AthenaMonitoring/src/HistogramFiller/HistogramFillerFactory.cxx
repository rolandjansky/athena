/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/algorithm/string.hpp>

#include "StaticHistogramProvider.h"
#include "LumiblockHistogramProvider.h"
#include "OfflineHistogramProvider.h"

#include "HistogramFiller1D.h"
#include "HistogramFillerEfficiency.h"
#include "CumulativeHistogramFiller1D.h"
#include "HistogramFillerRebinable1D.h"
#include "VecHistogramFiller1D.h"
#include "VecHistogramFiller1DWithOverflows.h"
#include "HistogramFillerProfile.h"
#include "HistogramFiller2D.h"
#include "HistogramFiller2DProfile.h"

#include "HistogramFillerFactory.h"

using namespace Monitored;

HistogramFiller* HistogramFillerFactory::create(const HistogramDef& def) {
  std::shared_ptr<IHistogramProvider> histogramProvider = createHistogramProvider(def);
  
  if (boost::starts_with(def.type, "TH1")) {
    if (def.opt.find("kCumulative") != std::string::npos) {
      return new CumulativeHistogramFiller1D(def, histogramProvider);
    } else if (def.opt.find("kAddBinsDynamically") != std::string::npos
	       || def.opt.find("kRebinAxes") != std::string::npos) {
      return new HistogramFillerRebinable1D(def, histogramProvider);
    } else if (def.opt.find("kVecUO") != std::string::npos) {
      return new VecHistogramFiller1DWithOverflows(def, histogramProvider);
    } else if (def.opt.find("kVec") != std::string::npos) {
      return new VecHistogramFiller1D(def, histogramProvider);
    } else {
      return new HistogramFiller1D(def, histogramProvider);
    }
  } else if (boost::starts_with(def.type, "TH2")) {
    return new HistogramFiller2D(def, histogramProvider);
  } else if (def.type == "TProfile") {
    return new HistogramFillerProfile(def, histogramProvider);
  } else if (def.type == "TProfile2D") {
    return new HistogramFiller2DProfile(def, histogramProvider);
  } else if (def.type == "TEfficiency") {
    return new HistogramFillerEfficiency(def, histogramProvider);
  }
  
  return nullptr;
}

std::shared_ptr<IHistogramProvider> HistogramFillerFactory::createHistogramProvider(const HistogramDef& def) {
  std::shared_ptr<IHistogramProvider> result;

  if ( def.convention.find("OFFLINE") != std::string::npos ) {
    result.reset(new OfflineHistogramProvider(m_gmTool, m_factory, def));
  } else if (def.opt.find("kLBNHistoryDepth") != std::string::npos) {
    result.reset(new LumiblockHistogramProvider(m_gmTool, m_factory, def));
  } else {
    result.reset(new StaticHistogramProvider(m_factory, def));
  }

  return result;
}
