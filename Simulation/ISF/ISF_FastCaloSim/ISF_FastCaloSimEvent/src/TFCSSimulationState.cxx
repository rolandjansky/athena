/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

//=============================================
//======= TFCSSimulationState =========
//=============================================

TFCSSimulationState::TFCSSimulationState()
{
  clear();
}

void TFCSSimulationState::clear()
{
  m_Ebin=-1;
  m_Etot=0;
  for(int i=0;i<CaloCell_ID_FCS::MaxSample;++i)
  {
   m_E[i]=0;
  }
}

void TFCSSimulationState::deposit_HIT(int /*sample*/,double /*hit_eta*/,double /*hit_phi*/,double /*hit_weight*/)
{
}

