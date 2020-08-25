/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_TTHTOVVDECAYFILTER_H
#define GENERATORFILTERS_TTHTOVVDECAYFILTER_H

#include "GeneratorModules/GenFilter.h"


/// @brief Generator filter to select ttH->VV events.
///
/// This filter is based on XtoVVDecayFilter, and extends the X->VV decay
/// selection to include
///
///	1. Multiple grandparents
///	2. Multiple parents
///	3. The total numbers of such decay chains
///	4. Same-sign parents requirement
///
/// @author Huaqiao ZHANG
class TtHtoVVDecayFilter : public GenFilter {
public:

  TtHtoVVDecayFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

private:

  std::vector<int> m_PDGGrandParent;
  std::vector<int> m_PDGParent;
  std::vector<int> m_StatusParent;
  int m_TotalParent;
  int m_SameChargeParent;
  std::vector<int> m_PDGChild;

  int m_nHtoVV;
  int m_nGoodHtoVV;
  int m_nGoodHtoVVSameCharge;

  bool findAncestor(HepMC::ConstGenVertexPtr searchvertex,
                    int targetPDGID);

};

#endif
