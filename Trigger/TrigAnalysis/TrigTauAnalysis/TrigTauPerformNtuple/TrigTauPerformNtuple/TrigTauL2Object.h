/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUL2OBJECT_H
#define TRIGTAUL2OBJECT_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauL2Object
/// Package : TrigTauPerformNtuple
/// Author  : Olya Igonkina (University of Oregon)
/// Created : April 2006
///
/// DESCRIPTION:
///           trigger tau candidate
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TVector.h"
#include "TLorentzVector.h"
#include "TrigTauPerformNtuple/TrigTauTauObject.h"
#include "TrigTauPerformNtuple/TrigTauFlags.h"


class TrigTauL2Object : public TrigTauTauObject
{
 public :
  TrigTauL2Object();
  TrigTauL2Object(const TrigTauL2Object & other);

  virtual ~TrigTauL2Object();
  virtual void reset();
  
  enum L2CALOTAUERRORS {LARPROB=0,
			TILEPROB=1,
			ROBPROB=2,
			RODPROB=3,
			FAILSEED=4,
			FAILPRESEED=5,
			EMS0E0=6,
			EMS1E0=7,
			EMS2E0=8,
			EMS3E0=9,
			HADS1E0=10,
			HADS2E0=11,
			HADS3E0=12,
			OTHERERRORS=13,
			NOERRORS=14};
  bool QDecode(unsigned int iError) const;


  virtual int charge() const;
  float energyWidth2() const {return m_stripWidth2;}; // the name of TrigTauCluster::stripWidth() is misleading, 
  //                                                    as value is calculated as stripWidth2 for sampling 2
  
  unsigned int   nTracksInDPhi(float dPhiCut, float minPt = 2000) const ; // pt is in MeV 
  unsigned int   nTrackInDEta(float dEtaCut, float minPt = 2000) const; // 
  unsigned int   nMatchedTracks(float dRCut, float minPt = 2000 ) const; //
  int            chargeMatchedTracks(float dRCut, float minPt = 2000 ) const; //

  float          dRWithCluster(const TLorentzVector & vec) const;
  float          dRWithVector(const TLorentzVector & vec, const TLorentzVector & reference ) const;

  // dR is taken with respect to cluster position
  unsigned int   nTracksInDR(float dRCut, float minPt = 1500) const;
  int            chargeInDR(float dRCut, float minPt = 1500) const;
  unsigned int   nSlowTracksInDR(float dRCut, float maxPt , float minPt = 1500) const;
  unsigned int   nTracksInIsolation(float dRMin, float dRMax, float minPt = 1500) const;
  
  // dR is taken with respect to vector position
  unsigned int   nTracksInDR(float dRCut, const TLorentzVector & reference, float minPt = 1500) const;
  int            chargeInDR(float dRCut, const TLorentzVector & reference, float minPt = 1500) const;
  unsigned int   nSlowTracksInDR(float dRCut, float maxPt , const TLorentzVector & reference, float minPt = 1500) const;
  unsigned int   nTracksInIsolation(float dRMin, float dRMax, const TLorentzVector & reference, float minPt = 1500) const;
  
  //----------- public data members

  float           etClus() const;
  float           etClusCalib() const;
  float           etNarrow() const;
  float           etWide() const;

  // additional cluster information
  float            m_HadEnergy;
  float            m_EMEnergy;
  float            m_eCalib;
  float            m_eEMCalib;

  enum CONSTANT {NEMSAMPLES = 4, NHADSAMPLES = 3};
  float            m_sEMRadius      [NEMSAMPLES];
  float            m_EMEnergyWidth  [NEMSAMPLES];
  float            m_HadEnergyWidth [NHADSAMPLES];
  float            m_EMEnergyWide   [NEMSAMPLES];
  float            m_EMEnergyMedium [NEMSAMPLES];
  float            m_EMEnergyNarrow [NEMSAMPLES];


  float            m_sHadRadius     [NHADSAMPLES];
  float            m_HadEnergyWide  [NHADSAMPLES];
  float            m_HadEnergyMedium[NHADSAMPLES];
  float            m_HadEnergyNarrow[NHADSAMPLES];

  // track information 
  unsigned int             nTracks() const {return tracks.size();} ;
  const  TrigTauTrack&     track(unsigned int i ) const { return *tracks[i]; };
  inline void              resetTracks();
  std::vector<TrigTauTrack*> tracks;

  int              m_nCoreTracks;
  int              m_nSlowTracks;
  int              m_nIsoTracks;
  float            m_charge; // use float in case we ever will shift to weighted pt charge
  float            m_leadingTrackPt;
  float            m_scalarPtSumCore;
  float            m_scalarPtSumIso;
  float            m_ptBalance;

  TLorentzVector   m_3fastest; 

  // combined information
  float         ptTot() const ;
  float            m_EtCalib;
  int              m_nMatchedTracks;
  int              m_roiID;
  long int         m_roiWord;
  TrigTauFlags     m_tags;

  unsigned int     m_quality; // float or uint?

  float m_caloRad;
  float m_EMRadius3S;


 private:
  ClassDef(TrigTauL2Object, 10)  ;
};


void              TrigTauL2Object::resetTracks()
{
  for(std::vector<TrigTauTrack*>::iterator itr = tracks.begin(); itr != tracks.end(); ++itr)
    delete *itr;
  tracks.erase( tracks.begin(), tracks.end());
}


#endif //TRIGTAUTRIGOBJECT_H
