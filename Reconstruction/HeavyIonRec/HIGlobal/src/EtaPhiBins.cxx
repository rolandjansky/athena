/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//

#include "HIGlobal/EtaPhiBins.h"
#include <iostream>

using namespace std;

/*
EtaPhiBins::EtaPhiBins(void){}
*/


EtaPhiBins::EtaPhiBins(const char* name, Float_t eta_min,Float_t eta_max,Int_t netabins,Int_t nphibins)
  : m_EtaMin(eta_min), m_EtaMax(eta_max), m_NEtaBins(netabins),
    m_NPhiBins(nphibins), m_H2(name,name,netabins,eta_min,eta_max,nphibins,-3.14159,3.14159)
{
    m_H2.SetDirectory(0);
}

/*
EtaPhiBins::~EtaPhiBins(void)
{
}
*/

Float_t EtaPhiBins::GetDeta()
{
  return (m_EtaMax-m_EtaMin)/m_NEtaBins;
}

Float_t EtaPhiBins::GetDphi()
{
  return (2*3.14159)/m_NPhiBins;
}

void EtaPhiBins::Fill(Rec::TrackParticleContainer* tpc)
{

  //cout << "EtaPhiBins::Fill:  tracks = " << tpc->size() << endl;

  Rec::TrackParticleContainer::const_iterator trackIter = tpc->begin();
  Rec::TrackParticleContainer::const_iterator trackEnd = tpc->end();
  for(  ; trackIter!=trackEnd ; ++trackIter)
    {
      //13.07.11 Savannah #84284 Scott fix
      //Rec::TrackParticle* track = *trackIter;
      const Rec::TrackParticle* track = *trackIter;
      //cout << "Track: Eta = " << track->eta() << "  Phi: " << track->phi() << endl;
      if (track)
	{
	  m_H2.Fill(track->eta(),track->phi());
	}
      else
	{
	  cout << "Why is track zero?" << endl;
	}
    }
}

void EtaPhiBins::Fill(DataVector<INavigable4Momentum>* tpc)
{

  //cout << "EtaPhiBins::Fill:  tracks = " << tpc->size() << endl;

  DataVector<INavigable4Momentum>::const_iterator trackIter = tpc->begin();
  DataVector<INavigable4Momentum>::const_iterator trackEnd = tpc->end();

  for(  ; trackIter!=trackEnd ; ++trackIter)
    {
      const INavigable4Momentum* track = *trackIter;

      if (track)
	{
	  m_H2.Fill(track->eta(),track->phi());
	}
      else
	{
	  cout << "Why is track zero?" << endl;
	}
    }
}

void  EtaPhiBins::Fill(float eta, float phi)
{
    m_H2.Fill(eta, phi);
}

float EtaPhiBins::GetBinContent(int ieta, int iphi) const
{
  // get the number of entries in (eta, phi) bin
  // WARNING: bin numbering as in the case of ROOT histograms: 1..max, not 0..max-1

   return  m_H2.GetBinContent(ieta, iphi);
}

