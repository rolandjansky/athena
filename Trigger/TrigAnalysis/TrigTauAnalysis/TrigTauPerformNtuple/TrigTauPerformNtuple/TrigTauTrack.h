/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUTRACK_H
#define TRIGTAUTRACK_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauTrack
/// Package : TauTrigPerformance
/// Author  : Olya Igonkina (University of Oregon)
/// Created : April 2006
///
/// DESCRIPTION:
///           contains tracks associated with tau object
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TLorentzVector.h"

class TrigTauTrack : public TLorentzVector
{
 public :
  TrigTauTrack();
  TrigTauTrack( const TrigTauTrack & other);
  virtual ~TrigTauTrack();

  virtual void reset();
  int charge() const {return m_charge;}

  int nSiHits() const { return m_nPixelHits + m_nSCTHits; } ;
  int nPixBlay() const { return m_nPixelHits + m_nBLHits; } ;
  int nBlayer() const { return m_nBLHits;} ;
  float nTRHItsRatio() const { return float(m_nTRTHits)/float(m_nStrawHits); };

  //==================
  float   m_chi2; // from TTP7 it is normalized to ndof
  int     m_charge;

  float   m_etaAtClus;
  float   m_phiAtClus;

  float   m_a0;
  float   m_a0error;

  int     m_nPixelHits;
  int     m_nSCTHits;
  int     m_nStrawHits; //( Trk :: numberOfTRTHits ) + ( Trk :: numberOfTRTOutliers );
  int     m_nTRTHits;   //( Trk :: numberOfTRTHighThresholdHits ) +( Trk :: numberOfTRTHighThresholdOutliers );
  int     m_nBLHits; // undefined for L2ID.

  float   m_z0;

 private:
  ClassDef(TrigTauTrack, 5)  ;
};

#endif
