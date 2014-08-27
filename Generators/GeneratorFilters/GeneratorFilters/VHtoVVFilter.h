/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_VHTOVVFILTER_H
#define GENERATORFILTERS_VHTOVVFILTER_H

#include "GeneratorModules/GenFilter.h"

/// A filter to select the decay of V1/V2 in H->V1V2, V1->decay, V2->decay with
/// VH production, e.g. V1/V2->leptons but V->all.
///
/// @author Christopher Lester
/// @author Tatsuya Masubuchi
class VHtoVVFilter : public GenFilter {
public:

  VHtoVVFilter(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

private:

  int m_PDGGrandParent;
  int m_PDGParent;
  std::vector<int> m_PDGHVChild1;
  std::vector<int> m_PDGHVChild2;
  std::vector<int> m_PDGAssocVChild;

  int m_nVHtoVV;
  int m_nGoodVHtoVV;
};

#endif
