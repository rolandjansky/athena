/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSPCAEnergyParametrization.h"
#include "ISF_FastCaloSimParametrization/FastCaloSim_CaloCell_ID.h"

//=============================================
//======= TFCSPCAEnergyParametrization =========
//=============================================

TFCSPCAEnergyParametrization::TFCSPCAEnergyParametrization(const char* name, const char* title):TFCSEnergyParametrization(name,title)
{
}

void TFCSPCAEnergyParametrization::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
  simulstate.set_Ebin(0);
  for(unsigned int i=0;i<CaloCell_ID_FCS::MaxSample;++i) simulstate.add_E(i,truth->Ekin()/CaloCell_ID_FCS::MaxSample);
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSPCAEnergyParametrization)

