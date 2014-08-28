/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AraToolExample/AnExampleAraTool.h"
 
AnExampleAraTool::AnExampleAraTool(PropertyMgr *pmgr) : AraToolBase(pmgr) {
  declareProperty("property1",m_property1=0.5,"doc to property1");
  declareProperty("property2",m_property2=0.5,"doc to property2");
}


 
StatusCode AnExampleAraTool::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode AnExampleAraTool::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode AnExampleAraTool::performTask(float &inp, float &out)
{
  out=inp*m_property1*m_property2;

  std::cout<<"\nAnExampleAraTool::performTask(in,out):\n"
	   <<"\t property1="<<m_property1<<"\n"    
	   <<"\t property2="<<m_property2<<"\n"    
	   <<"\t in="<<inp<<"\n" 
	   <<"\t out=in*property1*property2="<<out<<"\n";

  return StatusCode::SUCCESS;
}
