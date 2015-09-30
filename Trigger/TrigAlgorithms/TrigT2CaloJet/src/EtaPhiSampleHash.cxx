/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2CaloJet/EtaPhiSampleHash.h"

#include "TrigT2CaloJet/HashedTrig3Momentum.h"

#include "TVector2.h" // Phi_mpi_pi

#include <iostream>
#include <cmath>    // M_PI, fabs



const double EtaPhiSampleHash::kMinEta = -5.0;
const double EtaPhiSampleHash::kMaxEta = +5.0;
const double EtaPhiSampleHash::kMinPhi = -M_PI;
const double EtaPhiSampleHash::kMaxPhi = +M_PI;

//------------------------------------------------
EtaPhiSampleHash::EtaPhiSampleHash(const u_short &nEtaBins, const u_short &nPhiBins,
				   double minEta, double maxEta,
				   double minPhi, double maxPhi):

 minEta(minEta),
 maxEta(maxEta),
 minPhi(minPhi),
 maxPhi(maxPhi),
 nEtaBins(nEtaBins),
 nPhiBins(nPhiBins)
{
  checkValues();
  dEta = (maxEta-minEta)/ static_cast<double>(nEtaBins);
  dPhi = (maxPhi-minPhi)/ static_cast<double>(nPhiBins);
  invDeta = 1.0/dEta;
  invDphi = 1.0/dPhi;
}

//------------------------------------------------
void EtaPhiSampleHash::checkValues()
{
  using namespace std;
  if(nEtaBins >= kMaxNetaBins)
    cerr<<"EtaPhiSampleHash: number of eta bins too large"
	<<" ("<<nEtaBins<<" >= "<<kMaxNetaBins<<")"<<endl;
  if(nPhiBins >= kMaxNphiBins)
    cerr<<"EtaPhiSampleHash: number of phi bins too large"
	<<" ("<<nPhiBins<<" >= "<<kMaxNphiBins<<")"<<endl;
}
//------------------------------------------------
u_int EtaPhiSampleHash::hash(const double &eta, const double &phi,
			     const  CaloSampling::CaloSample &sample) const
{
  double rPhi = TVector2::Phi_mpi_pi(phi);
  if(eta<minEta || eta>maxEta || rPhi<minPhi || rPhi>maxPhi){
    std::cerr<<"EtaPhiSampleHash::hash("<<eta<<", phi, "<<sample<<")"<<std::endl
	     <<"out of bounds: returning 0"<<std::endl;
    return 0;
  }
  return (sample & kSixBits) + 
    ((int((eta-minEta)*invDeta) & kTenBits) <<  6) +
    ((int((phi-minPhi)*invDphi) & kTenBits) << 16);
}
//------------------------------------------------
u_int EtaPhiSampleHash::hash(const Trig3Momentum &t3m) const
{
  return hash(t3m.eta(), t3m.phi(), t3m.caloSample());
}
//------------------------------------------------
u_int EtaPhiSampleHash::hash(const HashedTrig3Momentum &htm) const
{
  return hash(htm.eta(), htm.phi(), htm.caloSample());
}

//------------------------------------------------
bool EtaPhiSampleHash::etaPhiSample(const u_int &hash, double &eta, double &phi,
				    CaloSampling::CaloSample &sample) const
{
  sample = static_cast< CaloSampling::CaloSample >(hash & kSixBits);
  eta = minEta + dEta * ((hash >>  6) & kTenBits);
  phi = minPhi + dPhi * ((hash >> 16) & kTenBits);
  return true;
}
