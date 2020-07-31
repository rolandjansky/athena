/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XTOVVDECAYFILTEREXTENDED_H
#define GENERATORFILTERS_XTOVVDECAYFILTEREXTENDED_H

#include "GeneratorModules/GenFilter.h"

///  The main purpose of this EF is to specify the decay of V1/V2
///  in H->V1V2, V1->decay, V2->decay with VH production.
///  (for example, V1/V2->leptonic decay but V->all)
///
/// @author Junichi TANAKA
/// @author Tatsuya MASUBUCHI
/// Modified such that it looks in the history of the particle mothers
/// and finds whether there is a mother with m_PDGGrandParent
/// Nikolaos Rompotis 15 January 2015
class XtoVVDecayFilterExtended : public GenFilter {
public:

  XtoVVDecayFilterExtended(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

private:

  int m_PDGGrandParent;
  int m_PDGParent;
  int m_StatusParent;
  std::vector<int> m_PDGChild1;
  std::vector<int> m_PDGChild2;

  int m_nHtoVV;
  int m_nGoodHtoVV;

  void FindAncestor(HepMC::ConstGenVertexPtr searchvertex,
                    int targetPDGID, bool& okPDGChild1, bool& okPDGChild2);
  HepMC::ConstGenParticlePtr CheckGrandparent(HepMC::ConstGenParticlePtr pitr, int &);
  bool RunHistory(HepMC::ConstGenParticlePtr pitr);
};

#endif
