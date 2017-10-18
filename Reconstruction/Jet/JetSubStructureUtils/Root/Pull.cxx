/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/Pull.h"
#include "TLorentzVector.h"
#include "JetSubStructureUtils/BoostToCenterOfMass.h"

using namespace std;
using namespace JetSubStructureUtils;

map<string, double> Pull::result(const fastjet::PseudoJet &jet) const
{
  vector<fastjet::PseudoJet> constit_pseudojets = jet.constituents();
  map<string, double> Variables;
  Variables["PullMag"] =  -999.*1000.;
  Variables["PullPhi"] =  -999.*1000.;
  Variables["Pull_C00"] =  -999.*1000.;
  Variables["Pull_C01"] =  -999.*1000.;
  Variables["Pull_C10"] =  -999.*1000.;
  Variables["Pull_C11"] =  -999.*1000.;

  if (constit_pseudojets.size() < 2) {
    return Variables;
  }

  double T[6];
  for (int i=0; i<6; i++){
    T[i] = 0;
  }

  TVector2 r_pull;//pull (times jet pt)
  double jeta = m_useEtaInsteadOfY ? jet.eta() : jet.rapidity();

  for(vector<fastjet::PseudoJet>::iterator cit = constit_pseudojets.begin(); cit != constit_pseudojets.end(); cit++) {
    const fastjet::PseudoJet &constit = *cit;

    double dphi = jet.delta_phi_to(constit); 
    double ceta = m_useEtaInsteadOfY ? constit.eta() : constit.rapidity();

    TVector2 r_i(ceta-jeta, dphi);  //position on eta-phi plane relative to weighted centroid
    T[2] += (ceta*ceta) - (jeta*jeta);
    T[3] += r_i.X() * r_i.Y();
    T[4] += r_i.Y() * r_i.X();
    T[5] += (constit.phi_std() * constit.phi_std()) - (jet.phi_std() * jet.phi_std());
    r_i *= (constit.pt() * r_i.Mod());
    r_pull += r_i;
  }

  if(jet.pt() != 0) {
    T[0] = r_pull.Mod()/jet.pt();
  }

  T[1] = TVector2::Phi_mpi_pi(r_pull.Phi()); //this means something quite different than statshape package at ANL!!

  Variables["PullMag"]  = T[0];
  Variables["PullPhi"]  = T[1];
  Variables["Pull_C00"] = T[2];
  Variables["Pull_C01"] = T[3];
  Variables["Pull_C10"] = T[4];
  Variables["Pull_C11"] = T[5];

  return Variables; 
}
