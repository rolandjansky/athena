/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetBasicPlot
#define INDETPHYSVALMONITORING_InDetBasicPlot
/**
 * @file InDetBasicPlot.h
 * @author shaun roe
 **/


// std includes
#include <string>
#include <array>

// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetBasicPlot: public InDetPlotBase {
public:
  InDetBasicPlot(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::TrackParticle& particle);
  void fill(const xAOD::TruthParticle& particle);
private:
  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  // print out final stats on exact zeroes
  void finalizePlots();
};




#endif
