/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <iostream>
using namespace std;
#include "TrigTauPerformNtuple/TrigTauEvent.h"
#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformNtuple/TrigTauTrueParticle.h"
#include "TrigTauPerformNtuple/TrigTauRecObject.h"
#include "TrigTauPerformNtuple/TrigTauL1Object.h"
#include "TrigTauPerformNtuple/TrigTauL2Object.h"
#include "TTimeStamp.h"



ClassImp(TrigTauEvent)  

  TrigTauEvent::TrigTauEvent()  
{
  reset();
}
TrigTauEvent::~TrigTauEvent() 
{ reset();}

void TrigTauEvent::reset()
{
  runNumber = -1;
  eventNumber = -1;
  time_stamp = 0;
  time_stamp_ns_offset =0;
  lumi_block = -1;
  bunch_crossing = -1;
  mbts_A_time = -1000;
  mbts_C_time = -1000;
  lar_A_time = -1000;
  lar_C_time = -1000;

  bunch_isFilled             = false;
  bunch_isInTrain            = false;
  bunch_distFromFrontBC      = -1;
  bunch_distFromTailBC       = -1;
  bunch_distFromFrontFilled  = -1;
  bunch_distFromTailFilled   = -1;
  
  mcEvent.reset();
  offlineEvent.reset();
  decision.reset();
  detectorStatus.reset();

  for(std::vector<TrigTauJetL1Object*>::iterator itr = jetL1.begin(); itr != jetL1.end(); ++itr)
    delete *itr;
  jetL1.erase(jetL1.begin(),     jetL1.end() );


  for(std::vector<TrigTauL1Object*>::iterator itr = tauL1.begin(); itr != tauL1.end(); ++itr)
    delete *itr;
  tauL1.erase(tauL1.begin(),     tauL1.end() );

  for(std::vector<TrigTauL2Object*>::iterator itr = tauL2.begin(); itr != tauL2.end(); ++itr)
    delete *itr;
  tauL2.erase(tauL2.begin(), tauL2.end() );

  for(std::vector<TrigTauRecEFObject*>::iterator itr = tauEF.begin(); itr != tauEF.end(); ++itr)
    delete *itr;
  tauEF.erase(tauEF.begin(),     tauEF.end() );

  for(std::vector<TrigTauTrackCollection*>::iterator itr = tracksRoIEF.begin(); itr != tracksRoIEF.end(); ++itr)
    delete *itr;
  tracksRoIEF.erase(tracksRoIEF.begin(), tracksRoIEF.end() );

  for(std::vector<TrigTauRoI*>::iterator itr = otherTrigRoI.begin(); itr != otherTrigRoI.end(); ++itr)
    delete *itr;
  otherTrigRoI.erase(otherTrigRoI.begin(), otherTrigRoI.end() );

  for(std::vector<TrigTauTauChain*>::iterator itr = tauChains.begin(); itr != tauChains.end(); ++itr)
    delete *itr;
  tauChains.erase(tauChains.begin(), tauChains.end() );

  etMissL1.reset();
  etMissL2.reset();
  etMissEF.reset();
  etMissEF_FEB.reset();
  etMissEF_noiseSupp.reset();
  etMissEF_allCells.reset();

}



bool  TrigTauEvent::same(const TLorentzVector & one, 
			 const TLorentzVector & another,
			 float dPhiMax, float dEtaMax ) const 
{
  if( fabs(  one.Eta() - another.Eta() )> dEtaMax ) return false;
  float dPhi =  fabs(  one.Phi() - another.Phi());
  if ( dPhi > 2.*M_PI ) dPhi -=  2.*M_PI ;
  if(dPhi > dPhiMax ) return false;
  return true;  
}

int TrigTauEvent::bestMatch(const TLorentzVector & object, 
			    std::vector<TLorentzVector*>::const_iterator ifirst,
			    std::vector<TLorentzVector*>::const_iterator iend,
			    float dPhiMax, float dEtaMax) const
{
  float dRMin = 100000;  
  int returnIndex = -1;

  float oEta = object.Eta();
  float oPhi = object.Phi();
  unsigned int index = 0;
  for(  std::vector<TLorentzVector*>::const_iterator ivec = ifirst;
       ivec != iend; ++ivec, ++index )
    {

      float dEta = fabs(  (*ivec)->Eta() - oEta );
      if( dEta >dEtaMax) continue;

      float dPhi =  fabs(  (*ivec)->Phi() - oPhi );
      if( dPhi > 2.*M_PI ) dPhi -=  2.*M_PI ;
      if( dPhi > dPhiMax ) continue;

      float dR = dPhi*dPhi + dEta*dEta;
      if( dR < dRMin ) { dRMin = dR; returnIndex = index ; }
    }
  
  return returnIndex;
}

int TrigTauEvent::eventHour() const
{
  TTimeStamp tt;
  tt.SetSec(time_stamp);
  return tt.GetTime()/10000;
}
int TrigTauEvent::eventMinute() const
{
  TTimeStamp tt;
  tt.SetSec(time_stamp);
  return (tt.GetTime()-eventHour()*10000)/100;
}
int TrigTauEvent::eventSecond() const
{
  TTimeStamp tt;
  tt.SetSec(time_stamp);
  return tt.GetTime()%100;
}
