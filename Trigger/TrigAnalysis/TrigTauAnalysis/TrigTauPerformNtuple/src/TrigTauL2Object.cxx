/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include "TrigTauPerformNtuple/TrigTauL2Object.h"

ClassImp(TrigTauL2Object)  

TrigTauL2Object::TrigTauL2Object()
{
  reset();
}

TrigTauL2Object::TrigTauL2Object(const TrigTauL2Object & other) :
  TrigTauTauObject(other),
  m_HadEnergy           (other.m_HadEnergy     ),
  m_EMEnergy            (other.m_EMEnergy      ),
  m_eCalib              (other.m_eCalib        ),
  m_eEMCalib            (other.m_eEMCalib      ),
  m_nCoreTracks         (other.m_nCoreTracks      ),
  m_nSlowTracks         (other.m_nSlowTracks      ),
  m_nIsoTracks          (other.m_nIsoTracks       ),
  m_charge              (other.m_charge           ), 
  m_leadingTrackPt      (other.m_leadingTrackPt   ),
  m_scalarPtSumCore     (other.m_scalarPtSumCore  ),
  m_scalarPtSumIso      (other.m_scalarPtSumIso   ),
  m_ptBalance           (other.m_ptBalance        ),

  m_EtCalib             (other.m_EtCalib       ),
  m_nMatchedTracks      (other.m_nMatchedTracks),
  m_roiID               (other.m_roiID),
  m_roiWord             (other.m_roiWord),
  m_tags                (other.m_tags),
  m_quality             (other.m_quality),
  m_caloRad             (other.m_caloRad),
  m_EMRadius3S          (other.m_EMRadius3S)
{

  m_3fastest.SetPxPyPzE(other.m_3fastest.Px(), other.m_3fastest.Py(), 
			other.m_3fastest.Pz(), other.m_3fastest.E() );

  for(unsigned int i = 0; i < NEMSAMPLES; ++i )
    {
      m_sEMRadius      [i] = other.m_sEMRadius       [i];
      m_EMEnergyWidth  [i] = other.m_EMEnergyWidth   [i];
      m_EMEnergyWide   [i] = other.m_EMEnergyWide    [i];
      m_EMEnergyMedium [i] = other.m_EMEnergyMedium  [i];
      m_EMEnergyNarrow [i] = other.m_EMEnergyNarrow  [i];
    }

  for(unsigned int i = 0; i < NHADSAMPLES; ++i )
    {
      m_sHadRadius     [i] = other.m_sHadRadius      [i];
      m_HadEnergyWidth [i] = other.m_HadEnergyWidth  [i];
      m_HadEnergyWide  [i] = other.m_HadEnergyWide   [i];
      m_HadEnergyMedium[i] = other.m_HadEnergyMedium [i];
      m_HadEnergyNarrow[i] = other.m_HadEnergyNarrow [i];
    }


  for(std::vector<TrigTauTrack*>::const_iterator itr = other.tracks.begin(); itr != other.tracks.end(); ++itr)
    tracks.push_back(new TrigTauTrack(**itr));
}


TrigTauL2Object::~TrigTauL2Object()
{
  reset();
}

void TrigTauL2Object::reset()
{
  TrigTauTauObject::reset();
  m_HadEnergy  = 0;     
  m_EMEnergy        = 0;
  m_eCalib          = 0;
  m_eEMCalib        = 0;
  m_EtCalib         = 0;
  m_nMatchedTracks  = 0;
  m_roiID           = 0;
  m_roiWord         = 0;
  m_tags.reset();
  m_quality         = 0;
  m_caloRad         = 0;
  m_EMRadius3S      = 0;

  for(unsigned int i = 0; i < NEMSAMPLES; ++i )
    {
      m_sEMRadius      [i] = 0;
      m_EMEnergyWidth  [i] = 0;
      m_EMEnergyWide   [i] = 0;
      m_EMEnergyMedium [i] = 0;
      m_EMEnergyNarrow [i] = 0;
    }

  for(unsigned int i = 0; i < NHADSAMPLES; ++i )
    {
      m_sHadRadius     [i] = 0;
      m_HadEnergyWidth [i] = 0;
      m_HadEnergyWide  [i] = 0;
      m_HadEnergyMedium[i] = 0;
      m_HadEnergyNarrow[i] = 0;
    }

  for(std::vector<TrigTauTrack*>::iterator itr = tracks.begin(); itr != tracks.end(); ++itr)
    delete *itr;
  tracks.erase(tracks.begin(), tracks.end() );

  m_nCoreTracks   = 0;  
  m_nSlowTracks   = 0;  
  m_nIsoTracks      = 0;
  m_charge          = 0;
  m_leadingTrackPt  = 0;
  m_scalarPtSumCore = 0;
  m_scalarPtSumIso  = 0;
  m_ptBalance       = 0;

  m_3fastest.SetPxPyPzE(0.,0.,0.,0.);

  return ;
}

int TrigTauL2Object::charge() const
{
  int charge = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      charge += (*itr)->charge();
    }
  return charge;
}



float TrigTauL2Object::ptTot() const
{
  float px = 0;
  float py = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      px += (*itr)->Px();
      py += (*itr)->Py();
    }

  float etEM = (m_eEMCalib)/sqrt(1.+0.5*exp(2.* fabs(Eta())));
  px += etEM*sin(Phi());
  py += etEM*cos(Phi() );
  //px += etClus()*sin(Phi());
  //py += etClus()*cos(Phi() );

  return sqrt(px*px + py*py) ; 
}

float   TrigTauL2Object::etClus() const
{
  float et = (m_HadEnergy + m_EMEnergy)/cosh(m_caloEta);
  return et;
}

float   TrigTauL2Object::etClusCalib() const
{
  float et = 0.;
  if (m_eCalib > 0.) et = (m_eCalib)/cosh(m_caloEta);
  return et;
}

unsigned int TrigTauL2Object::nTracksInDPhi(float dPhiCut, float minPt ) const 
{
  unsigned int ntracks = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      if( (*itr)->Pt() < minPt ) continue;
      float dPhi =  m_caloPhi - (*itr)->m_phiAtClus;
      if( fabs(dPhi) > fabs(2*M_PI-dPhi) ) dPhi = 2*M_PI-dPhi;
      //if( fabs(dPhi)> M_PI )dPhi = dPhi - 2*M_PI;
      if( fabs(dPhi) < dPhiCut ) ntracks++;
    }

  return ntracks;
}
unsigned int TrigTauL2Object::nTrackInDEta(float dEtaCut, float minPt ) const
{
  unsigned int ntracks = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      if( (*itr)->Pt() < minPt ) continue;
      float dEta =  m_caloEta - (*itr)->m_etaAtClus;
      if( fabs(dEta) < dEtaCut ) ntracks++;
    }
  return ntracks;
}
unsigned int TrigTauL2Object::nMatchedTracks(float dRCut, float minPt ) const
{
  
  unsigned int ntracks = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      if( (*itr)->Pt() < minPt ) continue;
      float dPhi =  m_caloPhi - (*itr)->m_phiAtClus;
      if( fabs(dPhi) > fabs(2*M_PI-dPhi) ) dPhi = 2*M_PI-dPhi;
      //if( fabs(dPhi)> M_PI )dPhi = dPhi - 2*M_PI;
      float dEta =  m_caloEta - (*itr)->m_etaAtClus;
      float dR = sqrt( dPhi*dPhi + dEta*dEta );

      if( fabs(dR) < dRCut ) ntracks++;
    }
  return ntracks;
}
int   TrigTauL2Object::chargeMatchedTracks(float dRCut, float minPt) const
{
  int charge = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      if( (*itr)->Pt() < minPt ) continue;
      float dPhi =  m_caloPhi - (*itr)->m_phiAtClus;      
      if( fabs(dPhi) > fabs(2*M_PI-dPhi) ) dPhi = 2*M_PI-dPhi;
      //if( fabs(dPhi)> M_PI )dPhi = dPhi - 2*M_PI;
      float dEta =  m_caloEta - (*itr)->m_etaAtClus;
      float dR =  sqrt( dPhi*dPhi + dEta*dEta );  

      if( fabs(dR) < dRCut )
	charge += (*itr)->charge();
    }
  return charge;
}

float TrigTauL2Object::dRWithCluster(const TLorentzVector & vec) const
{
  float dPhi =  m_caloPhi - vec.Phi();
  if( fabs(dPhi) > fabs(2*M_PI-dPhi) ) dPhi = 2*M_PI-dPhi;

  //if( dPhi > M_PI )dPhi = dPhi - 2*M_PI;
  //if( dPhi < M_PI )dPhi = dPhi + 2*M_PI;
  float dEta =  m_caloEta - vec.Eta();
  return sqrt( dPhi*dPhi + dEta*dEta );  
}


unsigned int TrigTauL2Object::nTracksInDR(float dRCut, float minPt ) const
{
  int nTrks = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      if( (*itr)->Pt() < minPt ) continue;
      if( dRWithCluster(**itr) < dRCut ) nTrks++;
    }
  return nTrks;
}

int   TrigTauL2Object::chargeInDR(float dRCut, float minPt ) const
{
  int charge = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      if( (*itr)->Pt() < minPt ) continue;
      if( dRWithCluster(**itr) < dRCut ) charge += (*itr)->charge();
    }
  return charge;

}
unsigned int TrigTauL2Object::nSlowTracksInDR(float dRCut, float maxPt,float minPt ) const
{
  int nTrks = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      float pt = (*itr)->Pt() ;
      if( pt < minPt ) continue;
      if( pt > maxPt ) continue;
      if( dRWithCluster(**itr) < dRCut ) nTrks++;
    }
  return nTrks;  
}
unsigned int TrigTauL2Object::nTracksInIsolation(float dRMin, float dRMax, 
						 float minPt ) const
{
  int nTrks = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      float pt = (*itr)->Pt() ;
      if( pt < minPt ) continue;
      float dR =  dRWithCluster(**itr);
      if( dR >= dRMin && dR < dRMax ) nTrks++;
    }
  return nTrks;  
}

  

float TrigTauL2Object::dRWithVector(const TLorentzVector & vec,const TLorentzVector & reference ) const
{
  float dPhi = reference.Phi() - vec.Phi();
  if( fabs(dPhi) > fabs(2*M_PI-dPhi) ) dPhi = 2*M_PI-dPhi;

  //if( dPhi > M_PI )dPhi = dPhi - 2*M_PI;
  //if( dPhi < M_PI )dPhi = dPhi + 2*M_PI;
  float dEta =  reference.Eta() - vec.Eta();
  return sqrt( dPhi*dPhi + dEta*dEta );  
}

unsigned int TrigTauL2Object::nTracksInDR(float dRCut, const TLorentzVector & reference, float minPt ) const
{
  int nTrks = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      if( (*itr)->Pt() < minPt ) continue;
      if( dRWithVector(**itr, reference) < dRCut ) nTrks++;
    }
  return nTrks;
}

int   TrigTauL2Object::chargeInDR(float dRCut, const TLorentzVector & reference, float minPt ) const
{
  int charge = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      if( (*itr)->Pt() < minPt ) continue;
      if( dRWithVector(**itr, reference) < dRCut ) charge += (*itr)->charge();
    }
  return charge;

}
unsigned int TrigTauL2Object::nSlowTracksInDR(float dRCut, float maxPt,const TLorentzVector & reference, float minPt ) const
{
  int nTrks = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      float pt = (*itr)->Pt() ;
      if( pt < minPt ) continue;
      if( pt > maxPt ) continue;
      if( dRWithVector(**itr, reference) < dRCut ) nTrks++;
    }
  return nTrks;  
}
unsigned int TrigTauL2Object::nTracksInIsolation(float dRMin, float dRMax, 
						 const TLorentzVector & reference, float minPt ) const
{
  int nTrks = 0;
  for(std::vector<TrigTauTrack*>::const_iterator itr = tracks.begin();
      itr != tracks.end(); ++itr )
    {
      float pt = (*itr)->Pt() ;
      if( pt < minPt ) continue;
      float dR =  dRWithVector(**itr, reference);
      if( dR >= dRMin && dR < dRMax ) nTrks++;
    }
  return nTrks;  
}

  
float TrigTauL2Object::etNarrow() const
{
  float Et = 0;
  for(unsigned int i = 0; i < NEMSAMPLES; ++i )
    Et += m_EMEnergyNarrow[i];
  for(unsigned int i = 0; i < NHADSAMPLES; ++i )
    Et += m_HadEnergyNarrow[i];

  Et /= cosh(m_caloEta ); // here we are trying to reproduce Calo EtNar

  return Et;
}

float TrigTauL2Object::etWide() const
{
  float Et = 0;
  for(unsigned int i = 0; i < NEMSAMPLES; ++i )
      Et += m_EMEnergyWide[i];
  for(unsigned int i = 0; i < NHADSAMPLES; ++i )
      Et += m_HadEnergyWide[i];
  
  Et /= cosh(m_caloEta );
  
  return Et;
}

bool TrigTauL2Object::QDecode(unsigned int iError) const
{
  bool isError=false;
  bool returnValue=false;

  if (iError==LARPROB) {
    if (0x000000FF & m_quality)  {isError=true; returnValue=true;}
  } else if (iError==TILEPROB) {
    if (0x0FFF0000 & m_quality)  {isError=true; returnValue=true;}
  } else if (iError==ROBPROB) {
    if (0x10000000 & m_quality)  {isError=true; returnValue=true;}
  } else if (iError==RODPROB) {
    if (0x20000000 & m_quality)  {isError=true; returnValue=true;}
  } else if (iError==FAILSEED) {
    if ((m_quality  & (1<<30)) !=0)  {isError=true; returnValue=true;}
  } else if (iError==FAILPRESEED) {
    if ((m_quality  & (1<<31)) !=0)  {isError=true; returnValue=true;}
  } else if (iError==EMS0E0) {
    if ((m_quality  & (1<<15)) !=0)  {isError=true; returnValue=true;}
  } else if (iError==EMS1E0) {
    if ((m_quality  & (1<<14)) !=0)  {isError=true; returnValue=true;}
  } else if (iError==EMS2E0) {
    if ((m_quality  & (1<<13)) !=0)  {isError=true; returnValue=true;}
  } else if (iError==EMS3E0) {
    if ((m_quality  & (1<<12)) !=0)  {isError=true; returnValue=true;}
  } else if (iError==HADS1E0) {
    if ((m_quality  & (1<<11)) !=0)  {isError=true; returnValue=true;}
  } else if (iError==HADS2E0) {
    if ((m_quality  & (1<<10)) !=0)  {isError=true; returnValue=true;}
  } else if (iError==HADS3E0) {
    if ((m_quality  & (1<<9)) !=0)  {isError=true; returnValue=true;}
  } else if (iError==OTHERERRORS) {
    if (m_quality && !isError) returnValue=true;
  } else if (iError==NOERRORS) {
    if (!m_quality) returnValue=true;
  } else {
    returnValue=false;
  }

  return returnValue;
}
