/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/SubjetFinder.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"
#include <iostream>

#include "fastjet/EECambridgePlugin.hh"

using namespace std;
using namespace JetSubStructureUtils;

SubjetFinder::SubjetFinder(fastjet::JetAlgorithm fj_jetalg, float jet_radius, float pt_min, int exclusive_njets) :
  m_fj_jetalg(fj_jetalg), m_jetrad(jet_radius), m_ptmin(pt_min), m_exclusivenjets(exclusive_njets)
{
}

vector<fastjet::PseudoJet> SubjetFinder::result(const fastjet::PseudoJet &jet) const
{
  vector<fastjet::PseudoJet> constit_pseudojets = jet.constituents();
  vector<fastjet::PseudoJet> subjets;
  if(constit_pseudojets.size() == 0) { 
    cout << "Warning in SubjetFinder: jet has no constituents" << endl;
    return subjets;
  }

  fastjet::ClusterSequence *clust_seq = NULL;

  if (m_fj_jetalg==fastjet::ee_kt_algorithm) {
    fastjet::JetDefinition jet_def = fastjet::JetDefinition(fastjet::ee_kt_algorithm);
    clust_seq =  new fastjet::ClusterSequence(constit_pseudojets, jet_def);
  }else if (m_fj_jetalg==fastjet::plugin_algorithm) {
    fastjet::JetDefinition jet_def = fastjet::JetDefinition(new fastjet::EECambridgePlugin(m_jetrad));
    clust_seq =  new fastjet::ClusterSequence(constit_pseudojets, jet_def);
  }else{
    fastjet::JetDefinition jet_def = fastjet::JetDefinition(m_fj_jetalg, m_jetrad, fastjet::E_scheme, fastjet::Best);
    clust_seq =  new fastjet::ClusterSequence(constit_pseudojets, jet_def);
  }

  if(m_exclusivenjets < 0) { // Inclusive
    subjets = fastjet::sorted_by_pt(clust_seq->inclusive_jets(m_ptmin));
  }
  else {
    subjets = fastjet::sorted_by_pt(clust_seq->exclusive_jets_up_to(m_exclusivenjets));
  }

  if(subjets.size() == 0) {
    delete clust_seq;
  }
  else {
    clust_seq->delete_self_when_unused();
  }

  return subjets;
}
