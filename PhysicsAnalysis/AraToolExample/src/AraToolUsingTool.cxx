/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AraToolExample/AraToolUsingTool.h"
#include "assert.h"

AraToolUsingTool::AraToolUsingTool(PropertyMgr *pmgr)
  : AraToolBase(pmgr),
    m_alsoToolAra(nullptr)
{
  declareProperty("alsoProperty1",m_alsoProperty1=0.5,"doc to alsoProperty1");
  declareProperty("alsoTool",m_alsoTool,"doc to alsoTool");
}
 
StatusCode AraToolUsingTool::initialize()
{
  if(RunningARA) {
    std::cout<<"AraToolUsingTool::initialize()"<<std::endl;
    m_alsoToolAra=new AnExampleAraTool();
    assert(m_alsoToolAra&&"AnExampleAraTool Initialization Failed");
    m_alsoToolAra->setProperty1(10.);
    m_alsoToolAra->setProperty2(10.);
    //ARA Part
  } else {
    if ( m_alsoTool.retrieve().isFailure() ) {
      return StatusCode::FAILURE;
    } else {
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode AraToolUsingTool::finalize()
{
  if(RunningARA) {
    std::cout<<"AraToolUsingTool::finalize()"<<std::endl;
    delete m_alsoToolAra;
  } else {
    m_alsoTool.release();
  }
  
  return StatusCode::SUCCESS;
}

StatusCode AraToolUsingTool::alsoPerformTask(float &inp, float &out)
{
  std::cout<<"\nAraToolUsingTool::alsoPerformTask(in,out):\n"
	   <<"\t alsoproperty1="<<m_alsoProperty1<<"\n"    
	   <<"\t in="<<inp<<"\n"    
	   <<"\t Start Calling AnExampleAraTool::performTask()\n"
	   <<"\t  - - - - - - - - - - - - - - - - - - - - \n";
  
  if(RunningARA) {
    m_alsoToolAra->performTask(inp,out);
  } else {
    m_alsoTool->getTool()->performTask(inp,out);
  }   

  std::cout<<"\t  - - - - - - - - - - - - - - - - - - - - \n"
	   <<"\t End Calling AnExampleAraTool::performTask()\n"
	   <<"\t\t Input to AnExampleAraTool::performTask() ="<<inp<<"\n"    
	   <<"\t\t Output from AnExampleAraTool::performTask()="<<out<<"\n";
  out+=m_alsoProperty1;
  std::cout<<"\t Output+=alsoProperty1 -> Output="<<out<<"\n";

  return StatusCode::SUCCESS;
}
