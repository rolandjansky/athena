/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_Pt.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_Pt.h"




InDetPerfPlot_Pt::InDetPerfPlot_Pt(InDetPlotBase* pParent, const std::string & sDir):InDetPlotBase(pParent, sDir){
  
}

void 
InDetPerfPlot_Pt::initializePlots() {	
  const bool prependDirectory(false);
  SingleHistogramDefinition hd= retrieveDefinition("recpT", "default");
  m_recPt  = Book1D("recpT",hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second, prependDirectory);
  hd= retrieveDefinition("recpTlow", "default");
  m_recPtLow  = Book1D("recpTlow",hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second, prependDirectory);
}

void 
InDetPerfPlot_Pt::fill(const xAOD::IParticle& particle){
    const float pt(particle.pt()/1000.);
    m_recPt->Fill(pt);
    m_recPtLow->Fill(pt);
}

