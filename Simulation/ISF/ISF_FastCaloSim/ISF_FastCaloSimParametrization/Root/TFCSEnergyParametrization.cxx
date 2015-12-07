/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSEnergyParametrization.h"
#include "ISF_FastCaloSimParametrization/FastCaloSim_CaloCell_ID.h"

//=============================================
//======= TFCSEnergyParametrization =========
//=============================================

TFCSEnergyParametrization::TFCSEnergyParametrization(const char* name, const char* title):TFCSParametrization(name,title)
{
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSEnergyParametrization)

