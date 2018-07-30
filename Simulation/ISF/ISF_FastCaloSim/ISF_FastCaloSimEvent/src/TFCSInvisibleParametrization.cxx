/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSInvisibleParametrization.h"

//=============================================
//======= TFCSInvisibleParametrization =========
//=============================================

void TFCSInvisibleParametrization::simulate(TFCSSimulationState& /*simulstate*/,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/)
{
  ATH_MSG_VERBOSE("now in TFCSInvisibleParametrization::simulate(). Don't do anything for invisible");
}

