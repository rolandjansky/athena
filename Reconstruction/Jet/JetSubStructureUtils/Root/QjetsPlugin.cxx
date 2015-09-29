/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/QjetsPlugin.h"
#include "Qjets.h"

using namespace std;
using namespace JetSubStructureUtils;

QjetsPlugin::QjetsPlugin(double zcut, double dcut_fctr, double exp_min, double exp_max, double rigidity, double truncation_fctr)
: m_rand_seed_set(false),
  m_zcut(zcut), 
  m_dcut_fctr(dcut_fctr), 
  m_exp_min(exp_min), 
  m_exp_max(exp_max),
  m_rigidity(rigidity), 
  m_truncation_fctr(truncation_fctr)
{
} 

void QjetsPlugin::SetRandSeed(unsigned int seed){
  m_rand_seed_set = true;
  m_seed = seed;
}

double QjetsPlugin::R()const{
  return 0.;
}

string QjetsPlugin::description() const{
  string desc("Qjets pruning plugin");
  return desc;
}

void QjetsPlugin::run_clustering(fastjet::ClusterSequence & cs) const{
  Qjets qjets(m_zcut, m_dcut_fctr, m_exp_min, m_exp_max, m_rigidity, m_truncation_fctr);
  if(m_rand_seed_set)
    qjets.SetRandSeed(m_seed);
  qjets.Cluster(cs);
}
