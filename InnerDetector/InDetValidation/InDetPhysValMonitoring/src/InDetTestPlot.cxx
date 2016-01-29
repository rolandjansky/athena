/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetTestPlot.cxx
 * @author shaun roe
**/

#include "InDetTestPlot.h"

InDetTestPlot::InDetTestPlot(InDetPlotBase* pParent, const std::string & sDir):InDetPlotBase(pParent, sDir),
  m_test(nullptr),m_test1(nullptr),m_test2(nullptr),m_testProfile(nullptr){
  
}

void 
InDetTestPlot::initializePlots() {
	ATH_MSG_INFO("Hello, I am a test plot");
	/**
	 * This was the pre-existing way of booking a histogram without using the HistogramDefinitionSvc
   * const bool prependDirectory(false);
   * m_test  = Book1D("testHistoId","my title",nBinsX,xAxisStart,xAxisEnd, prependDirectory);
   **/

  /** 
   *  This macro (in the base class) uses the variable name to infer the identifier in the  
   *	hdef file by stripping off the 'm_' prefix; this is convenient *IF* you know what 
   *  you're doing.
   **/
  IDPVM_BOOK(m_test); //books a histogram with hdef identifier of 'test'
  //
  /**
   * This is a two step method which retrieves the histogram definition explicitly and then
   * uses it. This allows the same definition to be used for multiple histograms
   **/
  SingleHistogramDefinition hd= retrieveDefinition("test");
	ATH_MSG_INFO(hd.str()); //print out the definition, formatted as a string
	hd.name="test1";
	book(m_test1, hd);
	hd.name="test2";
	book(m_test2, hd); //book two histograms with the same axes, title etc
	//
  /**
   * This is likely to be the most common use case for the HistogramDefinitionSvc;
   * a histogram is booked using the hdef identifier 'testProfile' and is handed
   * the appropriate pointer. In this case a TProfile example is used, but also works for TH1
   **/
  book(m_testProfile, "testProfile");
}

void 
InDetTestPlot::fill(const xAOD::TrackParticle& /*particle*/){
    const float pt(rand() % 200);
    m_test->Fill(pt);
    const float p(rand() % 50);
    m_testProfile->Fill(p,p,1);
}

