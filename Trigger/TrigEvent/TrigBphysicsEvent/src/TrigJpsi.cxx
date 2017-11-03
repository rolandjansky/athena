/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
  NAME:     TrigJpsi.cxx
  PACKAGE:  TrigBphysEvent
********************************************************************/
#include "TrigBphysicsEvent/TrigJpsi.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include <math.h>

TrigJpsi::TrigJpsi()
{
    m_jpsimuon[0] = nullptr; 
    m_jpsimuon[1] = nullptr;
}

TrigJpsi::TrigJpsi(const CombinedMuonFeature* jpsimuon1, const CombinedMuonFeature* jpsimuon2)
{
    setDaughters(jpsimuon1,jpsimuon2);
}

TrigJpsi::~TrigJpsi() 
{}

const CombinedMuonFeature* TrigJpsi::getDaughter(const int i) const 
{
    if (i==0 || i==1) 
        return m_jpsimuon[i];
    else
        return 0;
}

void TrigJpsi::setDaughters( const CombinedMuonFeature* jpsimuon1, const CombinedMuonFeature* jpsimuon2)
{ 
    m_jpsimuon[0]=jpsimuon1;
    m_jpsimuon[1]=jpsimuon2;
    CLHEP::HepLorentzVector hlv[2];
    
    for ( int i=0 ; i<2 ; ++i )
    {		
	double cot_teta=1/tan(2*atan(exp(-m_jpsimuon[i]->IDTrack()->param()->eta())));		
	double mass_muon = 105.658357;
        double absPt = fabs(m_jpsimuon[i]->pt());
	const double px = cos(m_jpsimuon[i]->IDTrack()->param()->phi0())*absPt;
        const double py = sin(m_jpsimuon[i]->IDTrack()->param()->phi0())*absPt;
	const double pz = cot_teta*absPt;
	const double en = sqrt((px*px+py*py+pz*pz)+ mass_muon*mass_muon);
		
	hlv[i].set(px,py,pz,en);
    }
    m_hlvJ=hlv[0]+hlv[1];

}
