/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include <iostream>

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
  m_hits.resize(CaloCell_ID_FCS::MaxSample);
  for(int i=0;i<CaloCell_ID_FCS::MaxSample;++i)
  {
    m_E[i]=0;
    m_Efrac[i]=0;
    m_hits[i].clear();
  }
}

void TFCSSimulationState::Print(Option_t *) const
{
  std::cout<<"Ebin="<<m_Ebin<<" E="<<E()<<std::endl;
  for(int i=0;i<CaloCell_ID_FCS::MaxSample;++i) if(E(i)!=0)
  {
    std::cout<<"  E"<<i<<"("<<CaloSampling::getSamplingName(i)<<")="<<E(i)<<" E"<<i<<"/E="<<Efrac(i)<<" #hits="<<m_hits[i].size()<<std::endl;
  }
}


