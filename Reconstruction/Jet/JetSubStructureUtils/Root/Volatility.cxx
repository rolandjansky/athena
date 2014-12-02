/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/Volatility.h"
#include "JetSubStructureUtils/QjetsPlugin.h"

using namespace std;
using namespace JetSubStructureUtils;

double Volatility::result(const fastjet::PseudoJet &jet) const
{
  vector<fastjet::PseudoJet> constit_pseudojets = jet.constituents();
  if(constit_pseudojets.size() == 0) return -999;

  QjetsPlugin qjets_plugin(_zcut, _dcut_fctr, _exp_min, _exp_max, _rigidity, _truncation_fctr);
  fastjet::JetDefinition qjets_def(&qjets_plugin);

  unsigned int n = 0;
  double sum_mass = 0, sum_mass2 = 0;
  for(unsigned int i=0; i < _num_iterations; i++) {
    if(_seed != -1) { // Seed needs to be different for each run of Qjets, but this has consistent running
      qjets_plugin.SetRandSeed(_seed + i);
    }
    fastjet::ClusterSequence clust_seq(constit_pseudojets, qjets_def);
    vector<fastjet::PseudoJet> qjets = fastjet::sorted_by_pt(clust_seq.inclusive_jets());

    if(qjets.size() == 0) {
      //cout << "0 jets" << endl;
      continue;
    }

    n++;
    sum_mass += qjets[0].m();
    sum_mass2 += qjets[0].m2();
  }

  if(n == 0) {
    // Prevent div-0
    return -999;
  }

  double rms_dev2 = sum_mass2/n - pow(sum_mass/n, 2.0);
  if(rms_dev2 < 0) {
    // To prevent NaN. Small negative numbers can happen due to floating point inaccuracy
    // when all qjets are same -> i.e. 0 rms
    rms_dev2 = 0;
  }
  
  double rms_dev = sqrt(rms_dev2);
  double volatility = rms_dev / (sum_mass/n);

  return volatility;
}

void Volatility::setSeed(unsigned int seed)
{
  _seed = seed;
}
