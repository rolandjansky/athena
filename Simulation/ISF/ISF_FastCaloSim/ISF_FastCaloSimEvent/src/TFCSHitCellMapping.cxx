/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSHitCellMapping.h"
#include "ISF_FastCaloSimEvent/ICaloGeometry.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

//=============================================
//======= TFCSHitCellMapping =========
//=============================================

TFCSHitCellMapping::TFCSHitCellMapping(const char* name, const char* title, ICaloGeometry* geo) :
  TFCSLateralShapeParametrizationHitBase(name,title),
  m_geo(geo)
{
}

void TFCSHitCellMapping::simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/)
{
  int cs=calosample();
  const CaloDetDescrElement* cellele=m_geo->getDDE(cs,hit.eta(),hit.phi());
  if(cellele) {
    simulstate.deposit(cellele,hit.E());
  } else {
    ATH_MSG_ERROR("TFCSLateralShapeParametrizationHitCellMapping::simulate_hit: cellele="<<cellele<<" E="<<hit.E()<<" cs="<<cs<<" eta="<<hit.eta()<<" phi="<<hit.phi());
  }
}

void TFCSHitCellMapping::Print(Option_t *option) const
{
  TString opt(option);
  if(!opt.IsWhitespace()) opt="";
  TFCSLateralShapeParametrizationHitBase::Print(option);

  ATH_MSG_INFO(opt <<"  geo="<<m_geo);
}
