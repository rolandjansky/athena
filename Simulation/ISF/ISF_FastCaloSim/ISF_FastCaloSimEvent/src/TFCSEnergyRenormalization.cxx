/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSEnergyRenormalization.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

//=============================================
//======= TFCSEnergyRenormalization =========
//=============================================

TFCSEnergyRenormalization::TFCSEnergyRenormalization(const char* name, const char* title):TFCSParametrization(name,title)
{
}

TFCSEnergyRenormalization::~TFCSEnergyRenormalization()
{
}

FCSReturnCode TFCSEnergyRenormalization::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/) const
{
  std::vector< double > energies(CaloCell_ID_FCS::MaxSample,0);

  //Loop over all cells and sum up energies
  for(const auto& iter : simulstate.cells()) {
    const CaloDetDescrElement* theDDE=iter.first;
    int layer=theDDE->getSampling();
    energies[layer]+=iter.second;
  }

  std::vector< float > scalefactor(CaloCell_ID_FCS::MaxSample,1);
  
  for(int layer=0;layer<CaloCell_ID_FCS::MaxSample;++layer) {
    if(energies[layer]!=0) scalefactor[layer]=simulstate.E(layer)/energies[layer];
  }

  //Loop over all cells and apply the scalefactor
  for(auto& iter : simulstate.cells()) {
    const CaloDetDescrElement* theDDE=iter.first;
    int layer=theDDE->getSampling();
    iter.second*=scalefactor[layer];
  }

  if(msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("Apply scale factors : ");
    for (int layer=0;layer<CaloCell_ID_FCS::MaxSample;++layer) {
      ATH_MSG_DEBUG("  "<<layer<<" *= "<<scalefactor[layer]<<" : "<<energies[layer]<<" -> "<<simulstate.E(layer));
    }  
  }

  return FCSSuccess;
}

