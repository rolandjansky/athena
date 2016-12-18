/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_VHTOVVFILTER_H
#define GENERATORFILTERS_VHTOVVFILTER_H

#include "GeneratorModules/GenFilter.h"

/// A filter to select the decay of V, V1 and V2 in VH production
/// with H->V1V2, V1->decay, V2->decay
/// e.g. V1/V2->leptons but V->all.
///
/// @author Christopher Lester
/// @author Tatsuya Masubuchi
/// @author James Robinson

class VHtoVVFilter : public GenFilter {
public:

  VHtoVVFilter(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();
  bool allChildrenInList(HepMC::GenParticle* particle, std::vector<int> childPDGIDs);
  HepMC::GenParticle* lastDecayWithSamePDGID(HepMC::GenParticle* particle);

private:

  int m_PDGGrandParent;
  int m_PDGParent;
  int m_PDGAssocV;
  std::vector<int> m_PDGHVChild1;
  std::vector<int> m_PDGHVChild2;
  std::vector<int> m_PDGAssocVChild;
  int m_StatusIntermediateHardScatter;
  int m_StatusOutgoingHardScatter;

  int m_nVHtoVV;
  int m_nGoodVHtoVV;
};

#endif
