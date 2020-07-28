/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_VHTOVVFILTERDILEP_H
#define GENERATORFILTERS_VHTOVVFILTERDILEP_H

#include "GeneratorModules/GenFilter.h"

/// A filter to select the decay of V1/V2 in H->V1V2, V1->decay, V2->decay with
/// VH production, e.g. V1/V2->leptons but V->all.
///
/// Known to work with PYTHIA6 and Pythia8 -- test (and update
/// this message!) before use in production with other generators.
///
/// @author Christopher Lester
/// @author Tatsuya Masubuchi
class VHtoVVDiLepFilter : public GenFilter {
public:

  VHtoVVDiLepFilter(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

private:

  int m_PDGGrandParent;
  int m_PDGParent;
  int m_PDGAssoc;
  int m_StatusCode;

  std::vector<int> m_PDGChildren;

  int m_nVHtoVV;
  int m_nGoodVHtoVV;

  void findAncestor(HepMC::ConstGenVertexPtr searchvertex,
                    int targetPDGID, int& n_okPDGChild);

};

#endif
