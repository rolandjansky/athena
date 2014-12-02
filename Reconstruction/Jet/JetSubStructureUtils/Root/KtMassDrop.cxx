/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/KtMassDrop.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"
#include <math.h>
#include <float.h>

using namespace std;
using namespace JetSubStructureUtils;

double KtMassDrop::result(const fastjet::PseudoJet &jet) const
{
  if(jet.m() == 0 ) return -1;
  if(jet.constituents().size() == 0) return -1;
	
  fastjet::JetDefinition jet_def = fastjet::JetDefinition(fastjet::kt_algorithm, 1.5,
                                                          fastjet::E_scheme, fastjet::Best);
  fastjet::ClusterSequence kt_clust_seq(jet.constituents(), jet_def);
  fastjet::PseudoJet kt_jet = fastjet::sorted_by_pt(kt_clust_seq.inclusive_jets()).front();

  fastjet::PseudoJet p1, p2;
  if(kt_jet.has_parents(p1, p2)) {
    return fmax(p1.m(), p2.m()) / jet.m();
  }

  // No parents
  return -1;
}
