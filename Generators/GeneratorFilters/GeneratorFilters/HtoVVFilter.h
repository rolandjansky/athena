/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_HTOVVFILTER_H
#define GENERATORFILTERS_HTOVVFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Specify the decay of V1/V2 in H->V1V2, V1->decay, V2->decay with VH production.
///
/// Example: V1/V2->leptonic decay but V->all
/// @author Junichi TANAKA
class HtoVVFilter : public GenFilter {
public:

  HtoVVFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  int m_PDGGrandParent;
  int m_PDGParent;
  std::vector<int> m_PDGChild1;
  std::vector<int> m_PDGChild2;

  int m_nHtoVV;
  int m_nGoodHtoVV;

};

#endif
