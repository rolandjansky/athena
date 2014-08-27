/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XXTOVVFILTER_H
#define GENERATORFILTERS_XXTOVVFILTER_H

#include "GeneratorModules/GenFilter.h"

/// @author N. Panikashvili
class XXvvGGFilter : public GenFilter {
public:

  XXvvGGFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_EtaRange;
  bool m_gammaZ;
  bool m_ZZ;

};

#endif
