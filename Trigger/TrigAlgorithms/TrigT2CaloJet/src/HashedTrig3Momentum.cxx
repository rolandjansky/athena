/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2CaloJet/HashedTrig3Momentum.h"

#include <cmath> // fabs

HashedTrig3Momentum::HashedTrig3Momentum() : Trig3Momentum(), m_hash(0) {}
//------------------------------------------------
HashedTrig3Momentum::HashedTrig3Momentum(const Trig3Momentum &t3m, const u_int &hash) :
  Trig3Momentum(t3m), m_hash(hash)
{
}
//------------------------------------------------
void HashedTrig3Momentum::addE(const double &eta, const double &phi,
			       const double &energy)
{
  //-dbg-using namespace std;
  //-dbg-cout<<"adding ("<<eta<<", "<<phi<<", "<<energy<<") to "
  //-dbg-    <<" ("<<eta_<<", "<<phi_<<", "<<ene_<<")"<<endl;
  updateWeightedCoord(eta, phi, energy);
  setE(e() + energy);
  //-dbg-cout<<"now ("<<eta_<<", "<<phi_<<", "<<ene_<<")"<<endl;
}
//------------------------------------------------
void HashedTrig3Momentum::reset()
{
  m_hash = 0;
  setE(0.);
  setEta(0.);
  setPhi(0.);
}
//------------------------------------------------
void HashedTrig3Momentum::addE(const Trig3Momentum &t3m)
{
  HashedTrig3Momentum::addE(t3m.eta(), t3m.phi(), t3m.e());
}
//------------------------------------------------
void HashedTrig3Momentum::updateWeightedCoord(const double &etaVal,
					      const double &phiVal,
					      const double &energyVal)
{
  double weightOld(fabs(e())), weightNew(fabs(energyVal));
  double weightTot(weightOld+weightNew);
  if(weightTot==0.0){
    setEta(0.5*(eta() + etaVal));
    setPhi(0.5*(phi() + phiVal));
  } else {
    double invWeightTot = 1.0 / weightTot;
    setEta((eta()*weightOld + etaVal*weightNew) * invWeightTot);
    setPhi((phi()*weightOld + phiVal*weightNew) * invWeightTot);
  }
}
//------------------------------------------------
