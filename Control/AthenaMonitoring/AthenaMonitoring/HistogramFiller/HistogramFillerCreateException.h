/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFillerCreateException_h
#define AthenaMonitoring_HistogramFiller_HistogramFillerCreateException_h

#include <stdexcept>

namespace Monitored {
  struct HistogramFillerCreateException: public std::runtime_error {
    HistogramFillerCreateException(std::string const& message)
      : std::runtime_error(message) {}
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFillerCreateException_h */