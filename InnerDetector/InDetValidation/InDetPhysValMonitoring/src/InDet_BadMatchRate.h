/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDET_BADMATCHRATE
#define INDETPHYSVALMONITORING_INDET_BADMATCHRATE

/**
 *@file InDet_BadMatchRate.h
 *@author: Max Baugh
 *
**/

#include <string>
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"

class TProfile;

class InDet_BadMatchRate:public InDetPlotBase{
 public:
  InDet_BadMatchRate(InDetPlotBase * pParent,  std::string sDir);
  //~virtual InDet_BadMatchRate(){/**nop**/};


  void fillDenominator(const xAOD::TrackParticle& particle);
  void fillNumBMR(const xAOD::TrackParticle& particle);
  void fillNumRF(const xAOD::TrackParticle& particle);
  
 private:
  int m_trackEtaBins;
  float m_etaMin;
  float m_etaMax;
  
  TProfile* m_base;
  TProfile* m_badMatchRateNumerator;
  TProfile* m_reallyFakeNumerator;

  TProfile* m_badMatchRate;
  TProfile* m_reallyFakeRate;

  void initializePlots();
  void finalizePlots();
};


#endif
