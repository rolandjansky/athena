/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/algorithm/string.hpp>

#include "HistogramFiller1D.h"
#include "HistogramFillerEfficiency.h"
#include "CumulativeHistogramFiller1D.h"
#include "VecHistogramFiller1D.h"
#include "VecHistogramFiller1DWithOverflows.h"
#include "HistogramFillerProfile.h"
#include "HistogramFiller2D.h"
#include "HistogramFiller2DProfile.h"

#include "DynamicHistogramFiller1D.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFillerFactory.h"

using namespace std;
using namespace Monitored;

HistogramFiller* HistogramFillerFactory::create(const HistogramDef& def) {
  const bool isDynamicHistogram = def.opt.find("kDynamic") != string::npos;

  if (!isDynamicHistogram) {
    return createStaticFiller(def);
  } else {
    return createDynamicFiller(def);
  }
}

HistogramFiller* HistogramFillerFactory::createStaticFiller(const HistogramDef& def) {
  TNamed* const histogram = m_factory->create(def);

  if (boost::starts_with(def.type, "TH1")) {
    if (def.opt.find("kCumulative") != string::npos) {
      return new CumulativeHistogramFiller1D(dynamic_cast<TH1*>(histogram), def);
    } else if (def.opt.find("kVecUO") != string::npos) {
      return new VecHistogramFiller1DWithOverflows(dynamic_cast<TH1*>(histogram), def);
    } else if (def.opt.find("kVec") != string::npos) {
      return new VecHistogramFiller1D(dynamic_cast<TH1*>(histogram), def);
    } else {
      return new HistogramFiller1D(dynamic_cast<TH1*>(histogram), def);
    }
  } else if (boost::starts_with(def.type, "TH2")) {
    return new HistogramFiller2D(dynamic_cast<TH2*>(histogram), def);
  } else if (def.type == "TProfile") {
    return new HistogramFillerProfile(dynamic_cast<TProfile*>(histogram), def);
  } else if (def.type == "TProfile2D") {
    return new HistogramFiller2DProfile(dynamic_cast<TProfile2D*>(histogram), def);
  } else if (def.type == "TEfficiency") {
    return new HistogramFillerEfficiency(dynamic_cast<TEfficiency*>(histogram), def);
  }
  
  return nullptr;
}

HistogramFiller* HistogramFillerFactory::createDynamicFiller(const HistogramDef& def) {
  if (boost::starts_with(def.type, "TH1")) {
    return new DynamicHistogramFiller1D(m_gmTool, m_factory, def);
  }

  return nullptr;
}