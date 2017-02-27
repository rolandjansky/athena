/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_PT
#define INDETPHYSVALMONITORING_INDETPERFPLOT_PT
/**
 * @file InDetPerfPlot_Pt.h
 * @author shaun roe
 **/


// std includes
#include <string>

// local includes
#include "InDetPlotBase.h"
#include "xAODBase/IParticle.h"
#include "xAODTruth/TruthParticle.h"


///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_Pt: public InDetPlotBase {
public:
  InDetPerfPlot_Pt(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::IParticle& particle);
  void fill(const xAOD::TruthParticle& particle);
private:
  ///Pt Histogram with full pt range
  TH1* m_recPt;
  ///Pt histogram with restricted range
  TH1* m_recPtLow;
  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
};




#endif
