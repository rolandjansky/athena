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

 m_minEta(minEta),
 m_maxEta(maxEta),
 m_minPhi(minPhi),
 m_maxPhi(maxPhi),
 m_nEtaBins(nEtaBins),
 m_nPhiBins(nPhiBins)
{
  checkValues();
  m_dEta = (m_maxEta-m_minEta)/ static_cast<double>(m_nEtaBins);
  m_dPhi = (m_maxPhi-m_minPhi)/ static_cast<double>(m_nPhiBins);
  m_invDeta = 1.0/m_dEta;
  m_invDphi = 1.0/m_dPhi;
}

//------------------------------------------------
void EtaPhiSampleHash::checkValues()
{
  using namespace std;
  if(m_nEtaBins >= kMaxNetaBins)
    cerr<<"EtaPhiSampleHash: number of eta bins too large"
	<<" ("<<m_nEtaBins<<" >= "<<kMaxNetaBins<<")"<<endl;
  if(m_nPhiBins >= kMaxNphiBins)
    cerr<<"EtaPhiSampleHash: number of phi bins too large"
	<<" ("<<m_nPhiBins<<" >= "<<kMaxNphiBins<<")"<<endl;
}
//------------------------------------------------
u_int EtaPhiSampleHash::hash(const double &eta, const double &phi,
			     const  CaloSampling::CaloSample &sample) const
{
  double rPhi = TVector2::Phi_mpi_pi(phi);
  if(eta<m_minEta || eta>m_maxEta || rPhi<m_minPhi || rPhi>m_maxPhi){
    std::cerr<<"EtaPhiSampleHash::hash("<<eta<<", phi, "<<sample<<")"<<std::endl
	     <<"out of bounds: returning 0"<<std::endl;
    return 0;
  }
  return (sample & kSixBits) + 
    ((int((eta-m_minEta)*m_invDeta) & kTenBits) <<  6) +
    ((int((phi-m_minPhi)*m_invDphi) & kTenBits) << 16);
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
  eta = m_minEta + m_dEta * ((hash >>  6) & kTenBits);
  phi = m_minPhi + m_dPhi * ((hash >> 16) & kTenBits);
  return true;
}
