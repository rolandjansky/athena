/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSHitCellMapping.h"
#include "ISF_FastCaloSimParametrization/CaloGeometry.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include <iostream>

//=============================================
//======= TFCSHitCellMapping =========
//=============================================

TFCSHitCellMapping::TFCSHitCellMapping(const char* name, const char* title, CaloGeometry* geo) :
  TFCSLateralShapeParametrizationHitBase(name,title),
  m_geo(geo)
{
}

void TFCSHitCellMapping::simulate_hit(t_hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
  int cs=calosample();
  const CaloDetDescrElement* cellele=m_geo->getDDE(cs,hit.eta(),hit.phi());
  simulstate.deposit(cellele,hit.E());
  //std::cout<<"TFCSLateralShapeParametrizationHitCellMapping::simulate_hit: cellele="<<cellele<<" E="<<hit.E()<<" cs="<<cs<<" eta="<<hit.eta()<<" phi="<<hit.phi()<<std::endl;
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSHitCellMapping)
