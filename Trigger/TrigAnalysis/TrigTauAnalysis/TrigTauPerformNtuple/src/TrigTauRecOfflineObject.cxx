/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauRecOfflineObject.h"

ClassImp(TrigTauRecOfflineObject)  

TrigTauRecOfflineObject::TrigTauRecOfflineObject()
{
  reset();
}

TrigTauRecOfflineObject::TrigTauRecOfflineObject(const TrigTauRecOfflineObject & other) :
  TrigTauRecObject(other),  

  m_Likelihood    (other.m_Likelihood   ),  
  m_ElTauLikelihood (other.m_ElTauLikelihood),  
  m_LikelihoodOld    (other.m_LikelihoodOld),

  m_tauJetLL_lowpt        (other.m_tauJetLL_lowpt   ),
  m_tauELL_lowpt        (other.m_tauELL_lowpt   ),
  m_tauJetNN       (other.m_tauJetNN   ),
  m_tauENN        (other.m_tauENN   ),

  m_discriCut   (other.m_discriCut),
  m_discriLL    (other.m_discriLL),
  m_discriNN    (other.m_discriNN),
  m_discriPDRS   (other.m_discriPDRS),
  m_discriCutTMVA   (other.m_discriCutTMVA),
  m_efficNN   (other.m_efficNN),
  
  m_BDTJetScore( other.m_BDTJetScore),
  m_BDTEleScore( other.m_BDTEleScore),
  m_SafeLikelihood( other.m_SafeLikelihood),
  
  m_accept (other.m_accept),
  tracks (other.tracks),
  looseTracks (other.looseTracks),
  conversionTracks (other.conversionTracks)
{
  for(std::vector<TrigTauTrack*>::const_iterator itr = other.tracks.begin(); itr != other.tracks.end(); ++itr)
    tracks.push_back(new TrigTauTrack(**itr));
  for(std::vector<TrigTauTrack*>::const_iterator itr = other.looseTracks.begin(); itr != other.looseTracks.end(); ++itr)
    looseTracks.push_back(new TrigTauTrack(**itr));
  for(std::vector<TrigTauTrack*>::const_iterator itr = other.conversionTracks.begin(); itr != other.conversionTracks.end(); ++itr)
    conversionTracks.push_back(new TrigTauTrack(**itr));

  // TauEDM v16
  for(std::vector<TrigTauTrack*>::const_iterator itr = other.seedCaloTracks.begin(); itr != other.seedCaloTracks.end(); ++itr)
      seedCaloTracks.push_back(new TrigTauTrack(**itr));
  for(std::vector<TrigTauTrack*>::const_iterator itr = other.seedTrkTracks.begin(); itr != other.seedTrkTracks.end(); ++itr)
      seedTrkTracks.push_back(new TrigTauTrack(**itr));
  for(std::vector<TrigTauTrack*>::const_iterator itr = other.seedCaloWideTracks.begin(); itr != other.seedCaloWideTracks.end(); ++itr)
      seedCaloWideTracks.push_back(new TrigTauTrack(**itr));
  for(std::vector<TrigTauTrack*>::const_iterator itr = other.seedTrkWideTracks.begin(); itr != other.seedTrkWideTracks.end(); ++itr)
      seedTrkWideTracks.push_back(new TrigTauTrack(**itr));
  for(std::vector<TrigTauTrack*>::const_iterator itr = other.otherTracks.begin(); itr != other.otherTracks.end(); ++itr)
      otherTracks.push_back(new TrigTauTrack(**itr));
}

TrigTauRecOfflineObject::~TrigTauRecOfflineObject()
{
  reset();
}

void TrigTauRecOfflineObject::reset()
{
  TrigTauRecObject::reset();
  m_Likelihood     =0;
  m_ElTauLikelihood  =0;
  m_tauJetLL_lowpt=0;
  m_tauELL_lowpt=0;
  m_tauJetNN=0;
  m_tauENN=0;

  m_LikelihoodOld =0;
  m_discriCut        =0;
  m_discriNN         =0;
  m_discriPDRS       =0;
  m_discriCutTMVA    =0;
  m_efficNN          =0;
  m_BDTJetScore      =0;
  m_BDTEleScore      =0;
  m_SafeLikelihood   =0;
  
  m_accept           =0;

  m_discriLL         =0;


  resetTracks();
  resetLooseTracks();
  resetConversionTracks();
  resetSeedCaloTracks();
  resetSeedTrkTracks();
  resetSeedCaloWideTracks();
  resetSeedTrkWideTracks();
  resetOtherTracks();
  
  return ;
}

bool TrigTauRecOfflineObject::goodTauRec() const
{
  if(  m_seedType != TrigTauTauObject::CALOOBJECT &&
       m_seedType != TrigTauTauObject::BOTH ) return false;

  // Perhaps change this to nProngs()?
  if( nTracks() == 1 ){
    if( m_Likelihood > 2.2 )  return true;
  }else {
    if( m_Likelihood > 4.8 )  return true;
  }
  return false;
}

bool TrigTauRecOfflineObject::goodTau1p3p() const
{
  if(  m_seedType != TrigTauTauObject::TRACK && 
       m_seedType != TrigTauTauObject::BOTH ) return false;
  
  if( m_discriCut>0.5 ) return true;
  return false;
}

void  TrigTauRecOfflineObject::resetTracks() 
{
  for(std::vector<TrigTauTrack*>::iterator itr = tracks.begin(); itr != tracks.end(); ++itr)
    delete *itr;
  tracks.erase(tracks.begin(), tracks.end());
} 

void  TrigTauRecOfflineObject::resetLooseTracks() 
{
  for(std::vector<TrigTauTrack*>::iterator itr = looseTracks.begin(); 
      itr != looseTracks.end(); ++itr)
    delete *itr;
  looseTracks.erase(looseTracks.begin(), looseTracks.end());
} 
void  TrigTauRecOfflineObject::resetConversionTracks() 
{
  for(std::vector<TrigTauTrack*>::iterator itr = conversionTracks.begin(); 
      itr != conversionTracks.end(); ++itr)
    delete *itr;
  conversionTracks.erase(conversionTracks.begin(), conversionTracks.end());
} 

void  TrigTauRecOfflineObject::resetSeedCaloTracks() 
{
  for(std::vector<TrigTauTrack*>::iterator itr = seedCaloTracks.begin(); 
      itr != seedCaloTracks.end(); ++itr)
      delete *itr;
  seedCaloTracks.clear();
} 

void  TrigTauRecOfflineObject::resetSeedTrkTracks() 
{
  for(std::vector<TrigTauTrack*>::iterator itr = seedTrkTracks.begin(); 
      itr != seedTrkTracks.end(); ++itr)
      delete *itr;
  seedTrkTracks.clear();
} 

void  TrigTauRecOfflineObject::resetSeedCaloWideTracks() 
{
  for(std::vector<TrigTauTrack*>::iterator itr = seedCaloWideTracks.begin(); 
      itr != seedCaloWideTracks.end(); ++itr)
      delete *itr;
  seedCaloWideTracks.clear();
} 

void  TrigTauRecOfflineObject::resetSeedTrkWideTracks() 
{
  for(std::vector<TrigTauTrack*>::iterator itr = seedTrkWideTracks.begin(); 
      itr != seedTrkWideTracks.end(); ++itr)
      delete *itr;
  seedTrkWideTracks.clear();
} 

void  TrigTauRecOfflineObject::resetOtherTracks() 
{
  for(std::vector<TrigTauTrack*>::iterator itr = otherTracks.begin(); 
      itr != otherTracks.end(); ++itr)
      delete *itr;
  otherTracks.clear();
} 
