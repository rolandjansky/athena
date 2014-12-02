/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/Dipolarity.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"
#include "TVector2.h"
#include <math.h>
#include <iostream>

using namespace std;
using namespace JetSubStructureUtils;

Dipolarity::Dipolarity(unsigned int m_n1, unsigned int m_n2, bool exclusive, double subjetR) :
  m_exclusive(exclusive),
  m_n1(m_n1),
  m_n2(m_n2),
  m_subjetR(subjetR) {

    if(m_n2 <= m_n1 || m_n1 == 0) {
    cout << "dipolarity was called with invalid arguments" << endl;
    }
}

double Dipolarity::result(const fastjet::PseudoJet &jet) const
{
  vector<fastjet::PseudoJet> constit_pseudojets = jet.constituents();
  if(constit_pseudojets.size() == 0) return -1;

  vector<fastjet::PseudoJet> kt_subjets;
  fastjet::PseudoJet jet1, jet2;

  if(m_exclusive == false) {
    fastjet::JetDefinition jet_def = fastjet::JetDefinition(fastjet::kt_algorithm, m_subjetR,
                                                            fastjet::E_scheme, fastjet::Best);
    fastjet::ClusterSequence kt_clust_seq(constit_pseudojets, jet_def);
    kt_subjets = fastjet::sorted_by_pt(kt_clust_seq.inclusive_jets(5000.0));

    if(kt_subjets.size() < m_n2) {
      //Not enough subjets
      return 0.0;
    }
  }
  else {
    if(m_n2 > constit_pseudojets.size()) {
      //We were asked to calculate exclusiveDipolarity, but there are not enough constituents
      return 0.0;
    }

    fastjet::JetDefinition jet_def = fastjet::JetDefinition(fastjet::kt_algorithm, 1.5,
                                                            fastjet::E_scheme, fastjet::Best);
    fastjet::ClusterSequence kt_clust_seq(constit_pseudojets, jet_def);
    kt_subjets = fastjet::sorted_by_pt(kt_clust_seq.exclusive_jets((int)m_n2));
  }

  jet1 = kt_subjets[m_n1-1];
  jet2 = kt_subjets[m_n2-1];
  return dipolarity(constit_pseudojets, jet1, jet2);
}

double Dipolarity::dipolarity(vector<fastjet::PseudoJet> &constit_pseudojets,
                                  fastjet::PseudoJet jet1, fastjet::PseudoJet jet2) const
{
  float dipolarity = 0;
  float sumpt = 0;

  TVector2 v12(jet2.eta() - jet1.eta(), jet1.delta_phi_to(jet2));
  if(v12.Mod2() < 0.001) return -1;

  for(unsigned int iConstit = 0; iConstit < constit_pseudojets.size(); iConstit++) {
    fastjet::PseudoJet constituent = constit_pseudojets[iConstit];

    sumpt += constituent.perp();

    TVector2 v;

    v.Set(constituent.eta() - jet1.eta(), jet1.delta_phi_to(constituent));
    float test = v * v12.Unit();

    if(test < 0)
      dipolarity += constituent.perp()*v.Mod2(); 
    else if(test < v12.Mod())
      dipolarity += constituent.perp()*(v.Mod2() - pow(v * v12.Unit(), 2));                                              
    else {
      v.Set(constituent.eta() - jet2.eta(), jet2.delta_phi_to(constituent));
      dipolarity += constituent.perp()*v.Mod2();                                                                           
    }
  }

  if(sumpt < 0.001) return -1;
  return dipolarity/(sumpt*v12.Mod2());
}
