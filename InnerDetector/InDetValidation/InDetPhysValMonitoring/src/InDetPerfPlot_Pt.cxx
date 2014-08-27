/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_Pt.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_Pt.h"

InDetPerfPlot_Pt::InDetPerfPlot_Pt(PlotBase* pParent, const std::string & sDir):PlotBase(pParent, sDir){
  //nop
}


void 
InDetPerfPlot_Pt::initializePlots() {
    m_recPt  = Book1D("recpT","p_{T} of selected rec tracks (in GeV);p_{T}(GeV/c)",200,0.,200, false);
    m_recPtLow  = Book1D("recpTlow","p_{T} of selected rec tracks (in GeV);p_{T}(GeV/c)",200,0,20, false);
}

void 
InDetPerfPlot_Pt::fill(const xAOD::IParticle& particle){
    const float pt(particle.pt()/1000.);
    m_recPt->Fill(pt);
    m_recPtLow->Fill(pt);
}

