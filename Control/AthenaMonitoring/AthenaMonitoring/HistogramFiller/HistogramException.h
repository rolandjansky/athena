/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramException_h
#define AthenaMonitoring_HistogramFiller_HistogramException_h

#include <stdexcept>

namespace Monitored {
  struct HistogramException: public std::runtime_error {
    HistogramException(std::string const& message)
      : std::runtime_error(message) {}
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramException_h */