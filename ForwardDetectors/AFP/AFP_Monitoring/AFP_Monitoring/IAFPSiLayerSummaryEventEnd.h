/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
 

#ifndef IAFPSILAYERSUMMARYEVENTEND_H
#define IAFPSILAYERSUMMARYEVENTEND_H

#include <string>

class IAFPSiLayerMonitor;

/// Class providing interface to summary objects which can be filled once per event.
class IAFPSiLayerSummaryEventEnd
{
public:
  /// @brief Fills the distribution using data passed in arguments.
  /// 
  /// @param binName name of the bin (layer) in summary distribution for which entry is to be added
  /// @param layer pointer to the IAFPSiLayerMonitor object which provides value to fill the distribution
  virtual void fillEventEnd (const std::string& binName, const IAFPSiLayerMonitor* layer) = 0;
};

#endif
