/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <string>
#include <sstream>

// Framework
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"

// TDAQ
#include "eformat/SourceIdentifier.h"

// Local
#include "TrigCostMonitor/TrigNtPostTool.h"

using namespace std;

//---------------------------------------------------------------------------------------
Trig::TrigNtPostTool::TrigNtPostTool(const std::string &type,
				     const std::string &name,
				     const IInterface  *parent)
  :AlgTool(type, name, parent),
   m_log(0),
   m_storeGate("StoreGateSvc", name),
   m_config(0)
{
  declareInterface<Trig::ITrigNtTool>(this);
  declareInterface<Trig::TrigNtPostTool>(this);
}

//---------------------------------------------------------------------------------------
Trig::TrigNtPostTool::~TrigNtPostTool()
{
  delete m_log; m_log = 0;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtPostTool::initialize()
{    
  m_log = new MsgStream(msgSvc(), name());
  
  log() << MSG::DEBUG << "initialize()" << endreq;

  if(m_storeGate.retrieve().isFailure()) {
    log() << MSG::ERROR << "Could not retrieve: " << m_storeGate << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log() << MSG::DEBUG << "Retrieved: " << m_storeGate << endreq;    
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtPostTool::finalize()
{
  //
  // Clean up
  //  
  log() << MSG::DEBUG << "finalize()" << endreq; 

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtPostTool::Fill(TrigMonConfig *config)
{
  if(!m_config) {
    m_config = config;
  }

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtPostTool::Fill(TrigMonEvent &event)
{
  //
  // Read ROB ids and match ids to Detector/subdetector
  //
  if(outputLevel() <= MSG::VERBOSE) {
    log() << MSG::VERBOSE << "Processing new event..." << endreq; 
  }
  
  if(!m_config) {
    log() << MSG::DEBUG << "Null TrigMonConfig pointer" << endreq; 
    return false;
  }

  std::vector<std::string> &vars = m_config->getVarName();
  std::vector<uint32_t>    &keys = m_config->getVarId();

  if(vars.size() != keys.size()) {
    log() << MSG::WARNING << "Mismatch in number of keys and vars: " << endreq
	  << " # vars: " << vars.size() << endreq
	  << " # keys: " << keys.size() << endreq;
    return false;
  }

  for(unsigned int i = 0; i < event.size<TrigMonROB>(); ++i) {
    const TrigMonROB &rob = event.at<TrigMonROB>(i);

    const std::vector<TrigMonROBData> &dvec = rob.getData();
    const std::vector<TrigMonROBSum>   svec = rob.getSum();
    
    if(outputLevel() <= MSG::VERBOSE) {
      log() << MSG::VERBOSE << "TrigMonROB at " << i << endreq; 
    }

    for(unsigned int d = 0; d < dvec.size(); ++d) {
      const TrigMonROBData &data = dvec.at(d);
      
      if(outputLevel() <= MSG::DEBUG) {
	log() << MSG::DEBUG << "TrigMonROBData id, size=" << data.getROBId() << ", " << data.getROBSize() << endl;
      }

      if(!m_robIds.insert(data.getROBId()).second) continue; 

      if(outputLevel() <= MSG::VERBOSE) {
	log() << MSG::VERBOSE << "TrigMonROBData at " << d << endreq; 
      }

      const eformat::helper::SourceIdentifier robS(data.getROBId());
            
      keys.push_back(data.getROBId());
      vars.push_back("ROB:SUBDET:"+robS.human_detector()+":GROUP:"+robS.human_group());
      
      if(outputLevel() <= MSG::DEBUG) {
	std::stringstream str;
	str <<"TrigMonROBData size/id/subdetector/group: " 
	    << std::setw(6) << std::setfill(' ') << std::dec << data.getROBSize() << "/" 
	    << std::hex << data.getROBId() << "/" 
	    << robS.human_detector() << "/" 
	    << robS.human_group();

	 log() << MSG::DEBUG << str.str() << endreq;
      }
    }

    for(unsigned int s = 0; s < svec.size(); ++s) {
      const TrigMonROBSum &data = svec.at(s);

      if(!m_detIds.insert(data.getSubDet()).second) continue; 

      if(outputLevel() <= MSG::VERBOSE) {
	log() << MSG::VERBOSE << "TrigMonROBSum at " << s << endreq; 
      }

      const eformat::SubDetector edet = static_cast<eformat::SubDetector>(data.getSubDet());
      const eformat::helper::SourceIdentifier robS(edet, 0, 0);
            
      keys.push_back(data.getSubDet());
      vars.push_back("DET:SUBDET:"+robS.human_detector()+":GROUP:"+robS.human_group());
      
      if(outputLevel() <= MSG::DEBUG) {
	std::stringstream str;
	str << "TrigMonROBSum size/id/subdetector/group: " 
	    << std::setw(6) << std::setfill(' ') << std::dec << data.getSize() << "/" 
	    << std::hex << data.getSubDet() << "/" 
	    << robS.human_detector() << "/" 
	    << robS.human_group();

	log() << MSG::DEBUG << str.str() << endreq;
      }
    }    
  }
   
  return true;
}
