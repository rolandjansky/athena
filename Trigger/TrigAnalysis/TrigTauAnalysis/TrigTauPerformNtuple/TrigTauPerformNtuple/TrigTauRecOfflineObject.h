/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAURECOFFLINEOBJECT_H
#define TRIGTAURECOFFLINEOBJECT_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauRecOfflineObject.h
/// Package : TrigTauPerformNtuple
/// Author  : Olya Igonkina (Nikhef)
/// Created : June 2008
///
/// DESCRIPTION:
///           ofline TauJet to be dumped in root ntuple
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TVector.h"
#include "TLorentzVector.h"
#include "TrigTauPerformNtuple/TrigTauRecObject.h"

class TrigTauRecOfflineObject : public TrigTauRecObject
{
 public :

  TrigTauRecOfflineObject();
  TrigTauRecOfflineObject(const TrigTauRecOfflineObject & other);


  virtual ~TrigTauRecOfflineObject();
  void reset();

  bool goodTauRec() const;
  bool goodTau1p3p() const;
  bool goodRec() const {return goodTauRec() || goodTau1p3p(); }


 
  float            m_Likelihood;    
  float            m_ElTauLikelihood;  
  float            m_LikelihoodOld; 

  float            m_tauJetLL_lowpt;
  float            m_tauELL_lowpt;
  float            m_tauJetNN;
  float            m_tauENN;

  float           m_discriCut;
  float           m_discriLL;
  float           m_discriNN;       
  float           m_discriPDRS;   
  float           m_discriCutTMVA;
  float           m_efficNN;   
  
  float           m_BDTJetScore;
  float           m_BDTEleScore;
  float           m_SafeLikelihood;
  
  unsigned int             m_accept;

  // Access functions for specific bits (from isTauFlag)
  bool isTauCutLoose() const          {return (0x1 << 0)  & m_accept;}
  bool isTauCutMedium() const         {return (0x1 << 1)  & m_accept;}
  bool isTauCutTight() const          {return (0x1 << 2)  & m_accept;}
  bool isMuonVeto() const             {return (0x1 << 4)  & m_accept;}
  bool isTauLlhTight() const          {return (0x1 << 5)  & m_accept;}
  bool isTauLlhMedium() const         {return (0x1 << 6)  & m_accept;}
  bool isTauLlhLoose() const          {return (0x1 << 7)  & m_accept;}
  bool isTauCutSafeLoose() const      {return (0x1 << 10) & m_accept;}
  bool isTauCutSafeMedium() const     {return (0x1 << 11) & m_accept;}
  bool isTauCutSafeTight() const      {return (0x1 << 12) & m_accept;}
  bool isTauCutSafeCaloLoose() const  {return (0x1 << 13) & m_accept;}
  bool isTauCutSafeCaloMedium() const {return (0x1 << 14) & m_accept;}
  bool isTauCutSafeCaloTight() const  {return (0x1 << 15) & m_accept;}
  bool isElectronVetoLoose() const    {return (0x1 << 16) & m_accept;}
  bool isElectronVetoMedium() const   {return (0x1 << 17) & m_accept;}
  bool isElectronVetoTight() const    {return (0x1 << 18) & m_accept;}
  bool isJetBDTLoose() const          {return (0x1 << 19) & m_accept;}
  bool isJetBDTMedium() const         {return (0x1 << 20) & m_accept;}
  bool isJetBDTTight() const          {return (0x1 << 21) & m_accept;}
  bool isEleBDTLoose() const          {return (0x1 << 22) & m_accept;}
  bool isEleBDTMedium() const         {return (0x1 << 23) & m_accept;}
  bool isEleBDTTight() const          {return (0x1 << 24) & m_accept;}
  bool isSafeLlhLoose() const         {return (0x1 << 25) & m_accept;}
  bool isSafeLlhMedium() const        {return (0x1 << 26) & m_accept;}
  bool isSafeLlhTight() const         {return (0x1 << 27) & m_accept;}
  
  // track information 
  unsigned int             nTracks() const {return tracks.size();} ;
  const  TrigTauTrack&     track(unsigned int i ) const { return *tracks[i]; };
  void                     resetTracks();
  std::vector<TrigTauTrack* > tracks;

  // track information 
  unsigned int             nLooseTracks() const {return looseTracks.size();} ;
  const  TrigTauTrack&     looseTrack(unsigned int i ) const { return *looseTracks[i]; };
  void                     resetLooseTracks();
  std::vector<TrigTauTrack* > looseTracks;

  // conversion information
  unsigned int             nConversionTracks() const {return conversionTracks.size();} ;
  const  TrigTauTrack&     conversionTrack(unsigned int i ) const { return *conversionTracks[i]; };
  void                     resetConversionTracks() ;
  std::vector<TrigTauTrack * > conversionTracks;

   // seedCalo_tracks
  unsigned int             nSeedCaloTracks() const {return seedCaloTracks.size();} ;
  const  TrigTauTrack&     seedCaloTrack(unsigned int i ) const { return *seedCaloTracks[i]; };
  void                     resetSeedCaloTracks() ;
  std::vector<TrigTauTrack * > seedCaloTracks;

   // seedTrk_tracks
  unsigned int             nSeedTrkTracks() const {return seedTrkTracks.size();} ;
  const  TrigTauTrack&     seedTrkTrack(unsigned int i ) const { return *seedTrkTracks[i]; };
  void                     resetSeedTrkTracks() ;
  std::vector<TrigTauTrack * > seedTrkTracks;

   // seedCalo_wideTrks
  unsigned int             nSeedCaloWideTracks() const {return seedCaloWideTracks.size();} ;
  const  TrigTauTrack&     seedCaloWideTrack(unsigned int i ) const { return *seedCaloWideTracks[i]; };
  void                     resetSeedCaloWideTracks() ;
  std::vector<TrigTauTrack * > seedCaloWideTracks;

   // seedTrk_wideTrks
  unsigned int             nSeedTrkWideTracks() const {return seedTrkWideTracks.size();} ;
  const  TrigTauTrack&     seedTrkWideTrack(unsigned int i ) const { return *seedTrkWideTracks[i]; };
  void                     resetSeedTrkWideTracks() ;
  std::vector<TrigTauTrack * > seedTrkWideTracks;

   // otherTrks
  unsigned int             nOtherTracks() const {return otherTracks.size();} ;
  const  TrigTauTrack&     otherTrack(unsigned int i ) const { return *otherTracks[i]; };
  void                     resetOtherTracks() ;
  std::vector<TrigTauTrack * > otherTracks;

  // Best estimate for multiplicity
  int nProngs() const {return nTracks() - nConversionTracks();}

 private:

  ClassDef(TrigTauRecOfflineObject, 5)  ;
};

#endif //TRIGTAURECOBJECT_H
