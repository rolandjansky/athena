/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/Angularity.h"
#include "TLorentzVector.h"

using namespace std;
using namespace JetSubStructureUtils;

double Angularity::result(const fastjet::PseudoJet &jet) const
{
  if(jet.constituents().size() == 0) return -1;
  if(jet.m()==0.0) return -1;

  vector<fastjet::PseudoJet> constit_pseudojets = jet.constituents();
  TLorentzVector jet_p4(jet.px(), jet.py(), jet.pz(), jet.e());

  double Angularity2=-1.;
  const double m_a2=-2.;
  double sum_a2=0.;

  for(unsigned int iConstit=0; iConstit < constit_pseudojets.size(); iConstit++) {
    TLorentzVector tclus = TLorentzVector(constit_pseudojets[iConstit].px(),constit_pseudojets[iConstit].py(),constit_pseudojets[iConstit].pz(),constit_pseudojets[iConstit].e());
    double theta_i = jet_p4.Angle(tclus.Vect());
    double sintheta_i = sin(theta_i);
    if( sintheta_i == 0 ) continue; // avoid FPE
    double e_theta_i_a2 = constit_pseudojets[iConstit].E()*pow(sintheta_i,m_a2)*pow(1-cos(theta_i),1-m_a2);
    sum_a2 += e_theta_i_a2;
  }

  if ( jet.m() < 1.e-20 ) return -1.0;
  Angularity2 = sum_a2/jet.m(); 
  return Angularity2;
}
