/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/ZCut.h"
#include "JetSubStructureUtils/KtSplittingScale.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"

using namespace std;
using namespace JetSubStructureUtils;

double ZCut::result(const fastjet::PseudoJet &jet) const
{
  vector<fastjet::PseudoJet> constit_pseudojets = jet.constituents();
  if(constit_pseudojets.size() == 0) return -1;

  fastjet::JetDefinition jet_def = fastjet::JetDefinition(fastjet::kt_algorithm, 1.5,
                                                          fastjet::E_scheme, fastjet::Best);
  fastjet::ClusterSequence kt_clust_seq(constit_pseudojets, jet_def);

  if(constit_pseudojets.size() < m_nSubJets) {
    //We were asked to calculate zCut, but there are not enough constituents
    return 0.0;
  }
  vector<fastjet::PseudoJet> subjets = kt_clust_seq.exclusive_jets((int)m_nSubJets);

  // Find last split jet (cluster_hist_index should be highest for the last created jet)
  fastjet::PseudoJet *lastSplitSubjet = NULL;
  int max_cluster_hist_index = -1;
  for(size_t iSubjet=0; iSubjet < subjets.size(); iSubjet++) {
    fastjet::PseudoJet parent1, parent2;
    if(kt_clust_seq.has_parents(subjets[iSubjet], parent1, parent2) &&
       subjets[iSubjet].cluster_hist_index() > max_cluster_hist_index) {
      max_cluster_hist_index = subjets[iSubjet].cluster_hist_index();
      lastSplitSubjet = &subjets[iSubjet];
    }
  }

  if(lastSplitSubjet == NULL) {
    //None of the subjets were split
    return 0.0;
  }

  KtSplittingScale split(m_nSubJets);
  double dmin = pow(split.result(jet), 2.0);

  double zcut = -1;
  if(dmin == 0)
    zcut = 0;
  else
    zcut = dmin / (dmin + lastSplitSubjet->m2());

  return zcut;
}
