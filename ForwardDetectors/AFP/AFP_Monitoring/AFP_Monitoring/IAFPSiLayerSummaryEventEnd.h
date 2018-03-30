/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
 

#ifndef IAFPSILAYERSUMMARYEVENTEND_H
#define IAFPSILAYERSUMMARYEVENTEND_H

#include <string>

class IAFPSiLayerMonitor;

class IAFPSiLayerSummaryEventEnd
{
public:
  virtual void fillEventEnd (const std::string& binName, const IAFPSiLayerMonitor* layer) = 0;
};

#endif
