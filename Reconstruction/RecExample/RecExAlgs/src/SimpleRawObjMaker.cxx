/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimpleRawObjMaker.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "RecExAlgs/SimpleRawObjMaker.h"
#include "RecExAlgs/SimpleRawObj.h"

//================ Constructor =================================================

SimpleRawObjMaker::SimpleRawObjMaker(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_rawObjOutputName("unspecified"),
  m_rawMessage("unspecified")
{
  //  template for property declaration
  declareProperty("RawObjOutputName", m_rawObjOutputName, "key of output object");
  declareProperty("RawMessage", m_rawMessage,"message to be printed");
}

//================ Destructor =================================================

SimpleRawObjMaker::~SimpleRawObjMaker()
{}


//================ Initialisation =================================================

StatusCode SimpleRawObjMaker::initialize()
{
  // Code entered here will be executed once at program start.
  
  ATH_MSG_INFO(name() << " initialize()"
               << endmsg
               << " Raw Message is: " << m_rawMessage << endmsg
               << " will write out RawObj with key " << m_rawObjOutputName);

  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode SimpleRawObjMaker::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode SimpleRawObjMaker::execute()
{

  ATH_MSG_INFO(" Raw Message is: " << m_rawMessage);

  SimpleRawObj * theObj = new SimpleRawObj();

  ATH_MSG_INFO(" recording SimpleRawObj " << m_rawObjOutputName);

  StatusCode sc = evtStore()->record(theObj,m_rawObjOutputName,false);

  if (sc.isFailure()) {
    ATH_MSG_ERROR(" could not record theObj " << m_rawObjOutputName);
  }
  
  // Code entered here will be executed once per event
  return StatusCode::SUCCESS;
}

//============================================================================================

