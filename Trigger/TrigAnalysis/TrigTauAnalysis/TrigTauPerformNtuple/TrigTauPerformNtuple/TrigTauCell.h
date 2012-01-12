/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUCELL_H
#define TRIGTAUCELL_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauCell
/// Package : TauTrigPerformance
/// Author  : S.Xella, Olya Igonkina (University of Oregon)
/// Created : Jan 2008
///
/// DESCRIPTION:
///           contains cells associated with tau object
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class TrigTauCell :  public TObject
{
 public :
  TrigTauCell();
  TrigTauCell( const TrigTauCell & other);
  virtual ~TrigTauCell();

  virtual void reset();

  //==================
  float   m_e;
  float   m_eta;
  float   m_phi;
  int     m_samp;
  int     m_gain;
  float   m_noise;
  float   m_time;
  float   m_quality;
  bool    m_badcell;

 private:
  ClassDef(TrigTauCell, 9)  ;
};

#endif
