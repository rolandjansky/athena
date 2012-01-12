/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUBUGFIX_H
#define _TRIGTAUBUGFIX_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauBugFix.h
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence, Olya Igonkina
/// Created : February 2007
///
/// DESCRIPTION:
///           Hack to fix up the  objects to replace track+calo eta/phi
///           with calo-only eta/phi.  This should be a temporary fix.
///           - nothing is permanent as temporary fixes... Now it is a class to fix varios problems
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class TString;
class TrigTauEvent;
class TrigTauL2Object;

class TrigTauBugFix : public TObject {

 public:
  enum TTPVERSION{TTP4, TTP5, TTP6, TTP7, TTP9, TTP10, TTP11, TTP12, TTP13, TTP14, TTP15};
  enum POOLFILE{ RDO, ESD, AOD};

  TrigTauBugFix( TTPVERSION version=TTP12, POOLFILE fileType=RDO) :
    m_TTP(version), m_fileType(fileType), 
    m_nTotal(0),
    m_nL1GeVtoMeV_Error(0), m_nL2roiWordIs0_Error(0), m_nDuplicateTrack_Error(0), 
    m_nZeroPointerTracks_Error(0), m_nTRTOnlyTracks(0) {};

  virtual ~TrigTauBugFix() {};

  void setTTPVersion(TTPVERSION version){ m_TTP=version; };
  void setPOOLFile( POOLFILE fileType) {m_fileType = fileType; };
  int setTTPVersion(const TString & fileName); // sets both m_TTP and m_fileType

  void fix(TrigTauEvent* event);

  void printSummary() const;

  bool correctL1GeVtoMev(TrigTauEvent* event ) ;
  bool removeDuplicateTracks(TrigTauEvent* event);
  bool removeDuplicateTracks(TrigTauL2Object * obj);
  bool resetIfl2roiWordIs0(TrigTauEvent* event);
  bool resetIfZeroPointerTracks(TrigTauEvent* event);
  bool attachTRTOnlyTracks(TrigTauEvent*event);

private:

  float tauRec_et_corr_TTP4(double et_in, double eta_in, int ntrk) const;



  TTPVERSION m_TTP;
  POOLFILE   m_fileType;

  int m_nTotal;
  int m_nL1GeVtoMeV_Error;
  int m_nL2roiWordIs0_Error;
  int m_nDuplicateTrack_Error;
  int m_nZeroPointerTracks_Error;
  int m_nTRTOnlyTracks;


  ClassDef(TrigTauBugFix, 2) ;

};

#endif
