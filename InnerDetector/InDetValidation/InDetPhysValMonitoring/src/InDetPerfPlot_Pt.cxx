/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_Pt.cxx
 * @author shaun roe
 * This was the first performance plot class written in this package, more or less
 * simply as a demonstrator. 
**/

#include "InDetPerfPlot_Pt.h"

InDetPerfPlot_Pt::InDetPerfPlot_Pt(InDetPlotBase* pParent, const std::string & sDir):InDetPlotBase(pParent, sDir),
m_recPt{},
m_recPtLow{}{
  //nop
}

void 
InDetPerfPlot_Pt::initializePlots() {	
  book(m_recPt, "recpT");
  book(m_recPtLow, "recPtLow");
}

void 
InDetPerfPlot_Pt::fill(const xAOD::IParticle& particle){
    const float pt(particle.pt()/1000.);
    m_recPt->Fill(pt);
    m_recPtLow->Fill(pt);
}

