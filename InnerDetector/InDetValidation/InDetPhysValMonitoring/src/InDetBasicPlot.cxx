/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetBasicPlot.cxx
 * @author shaun roe
 **/


#include "InDetPhysValMonitoringUtilities.h"

#include "InDetBasicPlot.h"
#include <cmath>
using namespace IDPVM;

InDetBasicPlot::InDetBasicPlot(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir)
 {
  
  //nop

}

void
InDetBasicPlot::initializePlots() {

 //  book(m_my_histo, "my_histo");
 //  book(m_my_second_histo, "my_second_histo");

}

void
InDetBasicPlot::fill(const xAOD::TruthParticle& /*particle*/) {
  
 //  fillHisto(m_my_histo, particle.eta());

}

void
InDetBasicPlot::fill(const xAOD::TrackParticle& /*particle*/) {

 //  fillHisto(m_my_second_histo, particle.phi());

}

void
InDetBasicPlot::finalizePlots() {

  // nop

}
