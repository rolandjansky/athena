/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "TrkValHistUtils/HitResidualPlots.h"

namespace Trk{

void HitResidualPlots::init() {

    residuals=NULL;
    pulls=NULL;
}
  
void HitResidualPlots::initializePlots(){

  if (m_sType!="") m_sType=m_sType+"_";
  residuals = Book1D(m_sType+"residuals",m_sType+" Hit residuals;residual;Entries",100,-20.,20.);
  pulls = Book1D(m_sType+"pulls",m_sType+" Hit pulls;pull;Entries",100,-20.,20.);
    
}

void HitResidualPlots::fill( const Trk::ResidualPull& resPull ) {

  const float residual = resPull.residual().front();
  const float pull = resPull.pull().front();

  residuals->Fill(residual);
  pulls->Fill(pull);

}

}//close namespace
#endif // not XAOD_ANALYSIS
