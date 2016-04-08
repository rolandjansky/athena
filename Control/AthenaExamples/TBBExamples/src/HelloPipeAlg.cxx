/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iterator>
#include "TBBExamples/IHelloTool.h"
#include "HelloPipeAlg.h"

HelloPipeAlg::HelloPipeAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_nTokens(4), m_filterSerial(true),
  m_myPrivateHelloTool("HelloTool",this), m_myPublicHelloTool("HelloTool"), m_pipeline()
{
  
  declareProperty("NTokens", m_nTokens, 
		  "number of tokens running through the pipeline");
  declareProperty("FilterSerial", m_filterSerial,
		  "control processing filters running mode");

  declareProperty("MyPrivateHelloTool", m_myPrivateHelloTool, "private IHelloTool");
  declareProperty("MyPublicHelloTool", m_myPublicHelloTool, "public, shared IHelloTool");

}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


class PipeInput : public tbb::filter {
private:
  int m_nTokensToDo;
  int m_nTokens;
  std::string m_message;
public:
  PipeInput(int nTokens) : tbb::filter(tbb::filter::serial_in_order), 
			   m_nTokensToDo(nTokens-1), 
			   m_nTokens(nTokens-1), m_message("Counting") {}
  void resetNTokens() { m_nTokens = m_nTokensToDo; }
  void * operator()(void*) {
    return (void*) ((0 >= --m_nTokens) ? 0 : &m_message); 
  }
};


class IHTPipeWrapper : public tbb::filter {
private:
  IHelloTool& m_iht;
public:
  IHTPipeWrapper(IHelloTool& wrapped, tbb::filter::mode tbbmod=tbb::filter::serial_in_order) :
    tbb::filter(tbbmod), m_iht(wrapped) {
    //std::cout << "IHTPipeWrapper created for tool " << m_iht.name() << std::endl;
    
  }
  void * operator()(void* item) {
    std::string& message(*(std::string*)item);
    m_iht.saySomething(message).ignore();
    //this may be used to pass something to the next tool in the pipe. 
    //Not used in this simple example
    return (void*) &message; 
  }
};


StatusCode HelloPipeAlg::initialize() {

  // Part 1: print where you are
  ATH_MSG_INFO ("initialize()");

  ATH_MSG_INFO 
    ("  " << m_myPrivateHelloTool.propertyName() 
     << " = " << m_myPrivateHelloTool.type() 
     << endreq
     << "  " << m_myPublicHelloTool.propertyName() 
     << " = " << m_myPublicHelloTool.type());


  // Part 3: Retrieve the tools using the ToolHandles
  if ( m_myPrivateHelloTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL 
      (m_myPrivateHelloTool.propertyName() << ": Failed to retrieve tool "
       << m_myPrivateHelloTool.type());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO 
      (m_myPrivateHelloTool.propertyName() << ": Retrieved tool " 
       << m_myPrivateHelloTool.type());
  }
  if ( m_myPublicHelloTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL 
      (m_myPublicHelloTool.propertyName() << ": Failed to retrieve tool "
       << m_myPublicHelloTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO (m_myPublicHelloTool.propertyName() << ": Retrieved tool "
		  << m_myPublicHelloTool.type());
  }
  //FIXME, this should be wrapped
  tbb::filter::mode concMode(tbb::filter::parallel);
  if (m_filterSerial) concMode=tbb::filter::serial_in_order;
  m_pipeFilters.push_back(new PipeInput(m_nTokens));  
  m_pipeline.add_filter(*(m_pipeFilters.back()));
  m_pipeFilters.push_back(new IHTPipeWrapper(*m_myPublicHelloTool, concMode));
  m_pipeline.add_filter(*(m_pipeFilters.back()));
  m_pipeFilters.push_back(new IHTPipeWrapper(*m_myPrivateHelloTool, concMode));
  m_pipeline.add_filter(*(m_pipeFilters.back()));

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode HelloPipeAlg::execute() {

  // Part 1: print where you are
  ATH_MSG_INFO ("execute()");
  //FIXME no filter run!!!
  m_pipeline.run(m_nTokens*4);
  PipeInput* pinf(dynamic_cast<PipeInput*>(&*(m_pipeFilters.front())));
  if (pinf) {
    pinf->resetNTokens();
    return StatusCode::SUCCESS;
  } else {
    return StatusCode::FAILURE;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode HelloPipeAlg::finalize() {

  // Part 1: print where you are
  ATH_MSG_INFO ("finalize()");
  m_pipeline.clear();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode HelloPipeAlg::beginRun() {
  // Part 1: print where you are
  ATH_MSG_INFO ("beginRun()");

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode HelloPipeAlg::endRun() {
  // Part 1: print where you are
  ATH_MSG_INFO ("endRun()");

  return StatusCode::SUCCESS;
}
