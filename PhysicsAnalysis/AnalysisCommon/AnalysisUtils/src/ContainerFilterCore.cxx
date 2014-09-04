/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisUtils/ContainerFilterCore.h"

ContainerFilterCore::ContainerFilterCore(PropertyMgr *pmgr) : AraToolBase(pmgr) {
  declareProperty("Selectors",m_selectorHandles,"The Selectors");
}

StatusCode ContainerFilterCore::initialize()
{
  if(!RunningARA) {
    if ( m_selectorHandles.retrieve().isFailure() ) {
      return StatusCode::FAILURE;
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode ContainerFilterCore::finalize()
{
  if(RunningARA) {
    std::vector<ISelectorCore*>::iterator toolItr=m_selectors.begin();
    std::vector<ISelectorCore*>::iterator toolEnd=m_selectors.end();

    for (;toolItr!=toolEnd;toolItr++) 
      delete *toolItr;
  } 

  return StatusCode::SUCCESS;
}

void ContainerFilterCore::addSelector(ISelectorCore *selector){
  if (selector)
    m_selectors.push_back(selector);

}


DataVector<INavigable4Momentum> *ContainerFilterCore::filter(DataVector<INavigable4Momentum> * in){

  DataVector<INavigable4Momentum>::iterator itr=in->begin();
  DataVector<INavigable4Momentum>::iterator iend=in->end();
  
  DataVector<INavigable4Momentum> *out=new DataVector<INavigable4Momentum>();

  for (;itr!=iend;itr++) {
    bool pass=true;

    if (RunningARA)
      pass=filterParticleARA(*itr);
    else
      pass=filterParticleAthena(*itr);

    if(pass)
      out->push_back(*itr);
  }

  return out;

}

bool ContainerFilterCore::filterParticleARA(INavigable4Momentum *part){

  std::vector<ISelectorCore*>::iterator toolItr=m_selectors.begin();
  std::vector<ISelectorCore*>::iterator toolEnd=m_selectors.end();
  
  bool pass=true;

  for (;pass && toolItr!=toolEnd;toolItr++) {
    pass=(*toolItr)->accept(part);
  }

  return pass;
}

bool ContainerFilterCore::filterParticleAthena(INavigable4Momentum *part){
  ToolHandleArray< ISelector >::iterator toolItr = m_selectorHandles.begin();
  ToolHandleArray< ISelector >::iterator toolEnd = m_selectorHandles.end();
  
  bool pass=true;
  
  for (;pass && toolItr!=toolEnd;toolItr++) {
    pass=(*toolItr)->getTool()->accept(part);
  }

  return pass;
}

