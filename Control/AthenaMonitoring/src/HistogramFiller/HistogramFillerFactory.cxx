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

#include "AthenaMonitoring/HistogramFiller/HistogramFillerFactory.h"

using namespace Monitored;

HistogramFiller* HistogramFillerFactory::create(const HistogramDef& def) {
  TNamed* const histogram = m_factory->create(def);

  if (boost::starts_with(def.type, "TH1")) {
    if (def.opt.find("kCumulative") != std::string::npos) {
      return new CumulativeHistogramFiller1D(dynamic_cast<TH1*>(histogram), def);
    } else if (def.opt.find("kVecUO") != std::string::npos) {
      return new VecHistogramFiller1DWithOverflows(dynamic_cast<TH1*>(histogram), def);
    } else if (def.opt.find("kVec") != std::string::npos) {
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