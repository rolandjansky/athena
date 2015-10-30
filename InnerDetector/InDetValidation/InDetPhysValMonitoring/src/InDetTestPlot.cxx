/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetTestPlot.cxx
 * @author shaun roe
**/

#include "InDetTestPlot.h"




InDetTestPlot::InDetTestPlot(InDetPlotBase* pParent, const std::string & sDir):InDetPlotBase(pParent, sDir),
  m_test(nullptr),m_testProfile(nullptr){
  
}

void 
InDetTestPlot::initializePlots() {
	ATH_MSG_INFO("Hello, I am a test plot");
  //const bool prependDirectory(false);
  SingleHistogramDefinition hd= retrieveDefinition("test");
  ATH_MSG_INFO(hd.str());
  IDPVM_BOOK(m_test);
  //m_test  = Book1D("test",hd.title,hd.nBinsX,hd.xAxis.first,hd.xAxis.second, prependDirectory);
  hd= retrieveDefinition("testProfile");
  ATH_MSG_INFO(hd.str());
  book(m_testProfile, "testProfile");
}

void 
InDetTestPlot::fill(const xAOD::TrackParticle& /*particle*/){
    const float pt(rand() % 200);
    m_test->Fill(pt);
    const float p(rand() % 50);
    m_testProfile->Fill(p,p,1);
}

