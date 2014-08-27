/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_PARENTCHILDFILTER_H
#define GENERATORFILTERS_PARENTCHILDFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Allows the user to search for any given decay  Parent -> Child + X
/// @author B K Gjelsten, March 2006
class ParentChildFilter : public GenFilter {
public:

  ParentChildFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  std::vector<int> m_PDGParent;
  double m_PtMinParent;
  double m_PtMaxParent;
  double m_MassMinParent;
  double m_MassMaxParent;
  double m_EtaRangeParent;
  std::vector<int> m_PDGChild;
  double m_PtMinChild;
  double m_EtaRangeChild;

};

#endif
