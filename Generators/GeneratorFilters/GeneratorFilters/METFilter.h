/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_METFILTER_H
#define GENERATORFILTERS_METFILTER_H

#include "GeneratorModules/GenFilter.h"


/// Selects events with true missing transverse energy above threshold
/// @author Robindra Prabhu, April 2006
class METFilter : public GenFilter {
public:
  METFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_MissingEtCut;
  double m_MissingEtUpperCut;
  int m_MissingEtCalcOption;
  std::string m_MissingEtObjName;

};

#endif
