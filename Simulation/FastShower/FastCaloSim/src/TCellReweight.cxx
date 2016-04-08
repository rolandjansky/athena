/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSim/TCellReweight.h"
#include "TMath.h"
#include "TCanvas.h"
#include <iostream>
#include <sstream>

using namespace std;

//=============================================
//========= TSplineReweight ===========
//=============================================

THitCellReweight::THitCellReweight():TLateralShapeCorrection(),m_factor(1.0)
{
  SetBit(kHasCellFactor);
}

std::string THitCellReweight::str() const
{
  stringstream out;
  out<<TLateralShapeCorrection::str();
  out<<"; *= "<<factor();
  
  return out.str();
}

float THitCellReweight::cellfactor(int deta,int dphi) const
{
  if(deta==0 && dphi==0) return factor();
  return 1.0;
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(THitCellReweight)
