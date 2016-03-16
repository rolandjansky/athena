/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2CaloJet/Trig3MomentumMerger.h"

#include <iostream>
#include <algorithm>
#include <cmath>    // M_PI, fabs

//------------------------------------------------
Trig3MomentumMerger::Trig3MomentumMerger() :
m_hashMap(), m_gridsHandler()
{
}
//------------------------------------------------
void resetCell(HashedTrig3Momentum &c)
{
  c.reset();
}
//------------------------------------------------
// match criterion to be used with find_if
struct HashMatchCriterion
{
  HashMatchCriterion(const u_int value, const EtaPhiSampleHash &hm):
    hash_(value), m_hashMap(hm) {};
  bool operator() (const HashedTrig3Momentum &cell) {
    return hash_ == cell.hash();
  };
  u_int hash_;
  EtaPhiSampleHash m_hashMap;
};

//------------------------------------------------
// functor to be used with for_each
struct CellMerger
{
  CellMerger(const EtaPhiSampleHash &hm, GridsHandler &gridsHandler) :
    hm_(hm), m_gridsHandler(gridsHandler) {}
  void operator() (const Trig3Momentum &t3m) {
    u_int hash(hm_.hash(t3m));
    HashMatchCriterion hmc(hash, hm_);
    Vmc &grid = m_gridsHandler.grid(t3m.caloSample());
    Vmc::iterator mergeCell = find_if(grid.begin(), grid.end(), hmc);
    if(mergeCell != grid.end()) mergeCell->addE(t3m);
    else grid.push_back(HashedTrig3Momentum(t3m, hash));
  }
  EtaPhiSampleHash hm_;
  GridsHandler &m_gridsHandler;
};
//------------------------------------------------
bool Trig3MomentumMerger::mergeCells(const Vt3m &input, Vt3m &output)
{
  m_gridsHandler.clearAllGrids();
  CellMerger merger(m_hashMap, m_gridsHandler);
  std::for_each(input.begin(), input.end(), merger);
  m_gridsHandler.appendAllGrids(output);
  return true;
}
//------------------------------------------------
unsigned int Trig3MomentumMerger::expectedLength(const Vt3m &input)
{
  // 4*4 b/c we should reduce the (eta,phi) granularity from 0.025x0.025 -> 0.1x0.1
  unsigned int expectedRedFactor=4*4;
  return input.size()/expectedRedFactor;
}
//------------------------------------------------
void testEtaPhiSampleHash(){
  using std::cout;
  using std::endl;
  double epsilon = 1.0e-9;
  double eta = EtaPhiSampleHash::kMinEta + epsilon;
  double phi = EtaPhiSampleHash::kMinPhi + epsilon;
  double dEta(2.0*5.0/100.), dPhi(2.0*M_PI/64.);
  const int nSamples = 25;
  // drop this and use GridsHandler::allSamples_
  CaloSampling::CaloSample samples[nSamples] = 
    {
       CaloSampling::PreSamplerB,
       CaloSampling::EMB1,
       CaloSampling::EMB2,
       CaloSampling::EMB3,   
       CaloSampling::PreSamplerE,
       CaloSampling::EME1,
       CaloSampling::EME2,
       CaloSampling::EME3,     
       CaloSampling::HEC0,
       CaloSampling::HEC1,
       CaloSampling::HEC2,
       CaloSampling::HEC3,            
       CaloSampling::TileBar0,
       CaloSampling::TileBar1,
       CaloSampling::TileBar2,      
       CaloSampling::TileGap1,
       CaloSampling::TileGap2,
       CaloSampling::TileGap3,      
       CaloSampling::TileExt0,
       CaloSampling::TileExt1,
       CaloSampling::TileExt2,      
       CaloSampling::FCAL0,
       CaloSampling::FCAL1,
       CaloSampling::FCAL2,               
       CaloSampling::Unknown
    };

  CaloSampling::CaloSample sample = CaloSampling::PreSamplerB;
  EtaPhiSampleHash eph;
  int iSample(0);
  eta = EtaPhiSampleHash::kMinEta + epsilon;
  while(eta<EtaPhiSampleHash::kMaxEta){
    phi = EtaPhiSampleHash::kMinPhi + epsilon;
    while(phi<EtaPhiSampleHash::kMaxPhi){
      iSample = 0;
      while(iSample<nSamples){
	sample = samples[iSample];
	u_int hash(eph.hash(eta,phi,sample));
	cout<<"(eta, phi, sample) "
	    <<"= ("<<eta<<", "<<phi<<", "<<sample<<")"
	    <<" -> "<<hash<<endl;
	double oEta(0.), oPhi(0.);
	CaloSampling::CaloSample oSample(CaloSampling::Unknown);
	eph.etaPhiSample(hash, oEta, oPhi, oSample);
	if(fabs(eta-oEta)>dEta)
	  cout<<"deltaEta("<<hash<<", "<<fabs(eta-oEta)<<")"
	      <<" : in "<<eta<<" out "<<oEta<<endl;
	if(fabs(phi-oPhi)>dPhi)
	  cout<<"deltaPhi("<<hash<<", "<<fabs(phi-oPhi)<<")"
	      <<" : in "<<phi<<" out "<<oPhi<<endl;
	if(sample!=oSample)
	  cout<<"sample("<<hash<<")"
	      <<" : in "<<sample<<" out "<<oSample<<endl;
	iSample++;
      } // end while(iSample)
      phi += dPhi;
    } // end while(phi)
    eta += dEta;
  } // end while(eta)
}
//------------------------------------------------

