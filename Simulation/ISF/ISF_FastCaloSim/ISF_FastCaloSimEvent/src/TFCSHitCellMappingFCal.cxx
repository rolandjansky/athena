/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSHitCellMappingFCal.h"
#include "ISF_FastCaloSimEvent/ICaloGeometry.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

//=============================================
//======= TFCSHitCellMappingFCal =========
//=============================================


FCSReturnCode TFCSHitCellMappingFCal::simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/)
{
  int cs=calosample();
  const CaloDetDescrElement* cellele=m_geo->getFCalDDE(cs,hit.x(),hit.y(),hit.z());
  ATH_MSG_DEBUG("HIT: cellele="<<cellele<<" E="<<hit.E()<<" cs="<<cs<<" x="<<hit.x()<<" y="<<hit.y() << " z="<<hit.z());

  /// protection against cases where hits cannot be matched to a FCal cell 
  if((hit.x() == 0 &&  hit.y() == 0) || cellele == 0) {
    ATH_MSG_WARNING("TFCSLateralShapeParametrizationHitCellMapping::simulate_hit: cellele="<<cellele<<" E="<<hit.E()<<" cs="<<cs<<" eta="<<hit.eta()<<" phi="<<hit.phi());
   return (FCSReturnCode)(FCSRetry+5); // retry simulation up to 5 times
  }
    
  simulstate.deposit(cellele,hit.E());
   
  return FCSSuccess;
}
