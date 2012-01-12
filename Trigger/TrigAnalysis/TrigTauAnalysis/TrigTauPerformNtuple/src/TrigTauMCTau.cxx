/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
using namespace std;
#include "TrigTauPerformNtuple/TrigTauMCTau.h"

ClassImp(TrigTauMCTau)  


TrigTauMCTau::TrigTauMCTau() :
  TrigTauTrueParticle(),
  _visible(0,0,0,0)
{}

TrigTauMCTau::TrigTauMCTau(const TrigTauTrueParticle & other ) :
  TrigTauTrueParticle(other)
{
  makeVisible();
}

TrigTauMCTau::~TrigTauMCTau() {}

TrigTauMCTau & TrigTauMCTau::operator = (const TrigTauTrueParticle & other )
{
  TrigTauTrueParticle::operator = (other);
  makeVisible();
  return *this;
}

TrigTauMCTau & TrigTauMCTau::operator = (const TrigTauMCTau & other )
{
  TrigTauTrueParticle::operator = (other);
  _visible = other._visible;  
  return *this;
}


void  TrigTauMCTau::print(int shift, bool completeTree) const
{
  TrigTauTrueParticle::print(shift, completeTree);
} 


  // TAU DECAY functions
TrigTauMCTau::DECAY     TrigTauMCTau::decayType() const
{
  int nNu = 0;
  int nGamma = 0;
  int nE = 0;
  int nPi = 0;
  int nK = 0;
  int nMu = 0;
  int nK0 = 0;
  unsigned int nDau =  nSavedDaughters();
  for(unsigned int idau = 0; idau < nDau ; ++ idau)
    {
      if (daughter(idau)==0) continue;
      switch (abs( daughter(idau)->pdgId() ) )
        {
        case 12 :
        case 14 :
        case 16 : nNu++; break;
        case 11 : nE++; break;
        case 13 : nMu++; break;
        case 211 : nPi++; break;
        case 22: nGamma++; break;
        case 321: nK++; break;
	case 130:
	case 310:
	case 311: nK0++; break;
        }
    }
  if( nNu == 2 && nE == 1  ) return ENUNU;
  if( nNu == 2 && nMu == 1 ) return MUNUNU;
  if( nNu == 2 ) return OTHER;

  // One prong
  if ( nPi+nK == 1) {
    if (nK0 > 0 ) return OTHERPR1;
    if ( nGamma == 0) {
      if (nPi == 1) return PINU;
      if (nK == 1) return KNU;
    }
    if ( nGamma == 2 ) {
      if (nPi == 1) return RHONU;
    }

    return OTHERPR1;
  }

  // 3-prong
  if ( nPi+nK == 3) {
    return PR3;
  }
  return OTHER;

}

bool TrigTauMCTau::hadronicDecay() const
{
  TrigTauMCTau::DECAY theDecay = decayType();

  if (theDecay == ENUNU) return false;
  if (theDecay == MUNUNU) return false;
  return true;
}

bool TrigTauMCTau::isOneProng() const
{
  TrigTauMCTau::DECAY theDecay = decayType();

  if (theDecay == PR3) return false;
  if (theDecay == OTHER) return false;
  return true;
}

const TLorentzVector&  TrigTauMCTau::visible() const
{
  return _visible;
}

/*
bool      TrigTauMCTau::trueTau() const
{
  if( abs(pdgId() ) != 15 ) return false;
  if( status() == 3 ) return false; // documentary
  unsigned int nDau =  nSavedDaughters();
  for(unsigned int idau = 0; idau < nDau ; ++ idau)
    if( abs( daughter(idau)->pdgId() ) == 15 ) return false; // documentary which is not marked as such

  return true;
}

bool      TrigTauMCTau::goodTrueTau() const
{
  if( !trueTau() ) return false;
  // if( Pt() < 20 ) return false;
  float eta = fabs(Eta());
  if( eta > 2.5  ) return false; //|| (eta>1.37 && eta < 1.52)

  DECAY  decay = decayType();
  if(decay  == ENUNU || decay == MUNUNU || decay == OTHER )
    return false;

  return true;
  
}
*/
    
void TrigTauMCTau::makeVisible() 
{
  // Visible 4-mom starts with tau itself
  _visible = *this;

  unsigned int nDau =  nSavedDaughters();
  for(unsigned int idau = 0; idau < nDau ; ++ idau)
    {
      TrigTauTrueParticle* theDaughter = daughter(idau);
      if( theDaughter == 0 ) continue;
      int pdgId = abs(theDaughter->pdgId());


      // Subtract off neutrinos
      if (pdgId == 12 || pdgId == 14 || pdgId == 16) _visible -= *theDaughter;
    }
}

const TrigTauTrueParticle* 
TrigTauMCTau::leadingTrack() const {

  const TrigTauTrueParticle* lead = 0;

  for (unsigned int ipart = 0; ipart < nSavedDaughters(); ipart++) {
    const TrigTauTrueParticle* part = daughter(ipart);
    if (part->charge() == 0) continue;

    if (lead == 0) {
      lead = part;
    } else if (lead->Pt() < part->Pt()) {
      lead = part;
    }
  }

  return lead;
}
