/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/KtSplittingScale.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"

using namespace std;
using namespace JetSubStructureUtils;

double KtSplittingScale::result(const fastjet::PseudoJet &jet) const
{
  if(jet.constituents().size() == 0) return -1;

  fastjet::JetDefinition jet_def = fastjet::JetDefinition(fastjet::kt_algorithm, 1.5,
                                                          fastjet::E_scheme, fastjet::Best);
  fastjet::ClusterSequence kt_clust_seq(jet.constituents(), jet_def);
  fastjet::PseudoJet kt_jet = fastjet::sorted_by_pt(kt_clust_seq.inclusive_jets()).front();
  double split = 1.5*sqrt(kt_clust_seq.exclusive_subdmerge(kt_jet, m_nSubJets));
  return split;
}
