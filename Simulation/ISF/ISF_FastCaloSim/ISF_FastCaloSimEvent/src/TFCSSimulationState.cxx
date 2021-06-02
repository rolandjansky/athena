/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/Random/RandomEngine.h"

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"
#include <iostream>
#include <cstring>

//=============================================
//======= TFCSSimulationState =========
//=============================================

TFCSSimulationState::TFCSSimulationState(CLHEP::HepRandomEngine* randomEngine)
  : m_randomEngine(randomEngine)
{
  clear();
}

void TFCSSimulationState::clear()
{
  set_SF(1);
  m_Ebin=-1;
  m_Etot=0;
  for(int i=0;i<CaloCell_ID_FCS::MaxSample;++i)
  {
    m_E[i]=0;
    m_Efrac[i]=0;
  }
}

void TFCSSimulationState::deposit(const CaloDetDescrElement* cellele, float E)
{
  auto mele=m_cells.find(cellele);
  if(mele==m_cells.end()) {
    m_cells.emplace(cellele,0);
    mele=m_cells.find(cellele);
  }  
  m_cells[cellele]+=E;
}

void TFCSSimulationState::Print(Option_t *) const
{
  std::cout<<"Ebin="<<m_Ebin<<" E="<<E()<<" #cells="<<m_cells.size()<<std::endl;
  for(int i=0;i<CaloCell_ID_FCS::MaxSample;++i) if(E(i)!=0)
  {
    std::cout<<"  E"<<i<<"("<<CaloSampling::getSamplingName(i)<<")="<<E(i)<<" E"<<i<<"/E="<<Efrac(i)<<std::endl;
  }
  if(m_AuxInfo.size()>0) {
    std::cout<<"  AuxInfo has "<<m_AuxInfo.size()<<" elements"<<std::endl;
    for(auto& a : m_AuxInfo) {
      std::cout<<"    "<<a.first<<" : bool="<<a.second.b<<" char="<<a.second.c<<" int="<<a.second.i<<" float="<<a.second.f<<" double="<<a.second.d<<" void*="<<a.second.p<<std::endl;
    }
  }  
}

std::uint32_t TFCSSimulationState::getAuxIndex(std::string s) 
{
  return TFCSSimulationState::fnv1a_32(s.c_str(),s.size());
}

std::uint32_t TFCSSimulationState::getAuxIndex(const char* s) 
{
  return TFCSSimulationState::fnv1a_32(s,std::strlen(s));
}

void TFCSSimulationState::AddAuxInfoCleanup(const TFCSParametrizationBase* para)
{
  m_AuxInfoCleanup.insert(para);
}

void TFCSSimulationState::DoAuxInfoCleanup()
{
  for(auto para : m_AuxInfoCleanup) {
    para->CleanAuxInfo(*this);
  }
}

