/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetTestPlot
#define INDETPHYSVALMONITORING_InDetTestPlot
/**
 * @file InDetTestPlot.h
 * @author shaun roe
 **/


// std includes
#include <string>

// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
class TEfficiency;


/**
 * @class InDetTestPlot
 * @brief Class holding plots to test the histogram definition service and subsequent filling
 */
class InDetTestPlot: public InDetPlotBase {
public:
  InDetTestPlot(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::TrackParticle& particle);
private:
  TH1* m_test;
  TH1* m_test1;
  TH1* m_test2;
  TProfile* m_testProfile;
  TH2* m_test2D;
  TEfficiency* m_testEff;
  TH1* m_nonsense;
  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
};




#endif
