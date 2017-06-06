/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <string>
#include <sstream>

// Framework
#include "AthenaKernel/errorcheck.h"
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
  :AthAlgTool(type, name, parent),
   m_config(0)
{
  declareInterface<Trig::ITrigNtTool>(this);
  declareInterface<Trig::TrigNtPostTool>(this);
}

//---------------------------------------------------------------------------------------
Trig::TrigNtPostTool::~TrigNtPostTool()
{
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtPostTool::initialize()
{    
  
  ATH_MSG_DEBUG("initialize()" );

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtPostTool::finalize()
{
  //
  // Clean up
  //  
  ATH_MSG_DEBUG("finalize()" ); 

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
  ATH_MSG_VERBOSE("Processing new event..." ); 
  
  if(!m_config) {
    ATH_MSG_DEBUG("Null TrigMonConfig pointer" ); 
    return false;
  }

  std::vector<std::string> &vars = m_config->getVarName();
  std::vector<uint32_t>    &keys = m_config->getVarId();

  if(vars.size() != keys.size()) {
    ATH_MSG_WARNING("Mismatch in number of keys and vars: " );
    ATH_MSG_WARNING(" # vars: " << vars.size() );
    ATH_MSG_WARNING(" # keys: " << keys.size() );
    return false;
  }

  for(unsigned int i = 0; i < event.size<TrigMonROB>(); ++i) {
    const TrigMonROB &rob = event.at<TrigMonROB>(i);

    const std::vector<TrigMonROBData> &dvec = rob.getData();
    const std::vector<TrigMonROBSum>   svec = rob.getSum();
    
    ATH_MSG_VERBOSE("TrigMonROB at " << i ); 

    for(unsigned int d = 0; d < dvec.size(); ++d) {
      const TrigMonROBData &data = dvec.at(d);
      
      ATH_MSG_DEBUG("TrigMonROBData id, size=" << data.getROBId() << ", " << data.getROBSize());

      if(!m_robIds.insert(data.getROBId()).second) continue; 

      ATH_MSG_VERBOSE("TrigMonROBData at " << d ); 

      const eformat::helper::SourceIdentifier robS(data.getROBId());
            
      keys.push_back(data.getROBId());
      vars.push_back("ROB:SUBDET:"+robS.human_detector()+":GROUP:"+robS.human_group());
      
      if(msgLvl(MSG::DEBUG)) {
        std::stringstream str;
        str <<"TrigMonROBData size/id/subdetector/group: " 
          << std::setw(6) << std::setfill(' ') << std::dec << data.getROBSize() << "/" 
          << std::hex << data.getROBId() << "/" 
          << robS.human_detector() << "/" 
          << robS.human_group();

        ATH_MSG_DEBUG(str.str() );
      }
    }

    for(unsigned int s = 0; s < svec.size(); ++s) {
      const TrigMonROBSum &data = svec.at(s);

      if(!m_detIds.insert(data.getSubDet()).second) continue; 

      ATH_MSG_VERBOSE("TrigMonROBSum at " << s ); 

      const eformat::SubDetector edet = static_cast<eformat::SubDetector>(data.getSubDet());
      const eformat::helper::SourceIdentifier robS(edet, 0, 0);
            
      keys.push_back(data.getSubDet());
      vars.push_back("DET:SUBDET:"+robS.human_detector()+":GROUP:"+robS.human_group());
      
      if(msgLvl(MSG::DEBUG)) {
        std::stringstream str;
        str << "TrigMonROBSum size/id/subdetector/group: " 
          << std::setw(6) << std::setfill(' ') << std::dec << data.getSize() << "/" 
          << std::hex << data.getSubDet() << "/" 
          << robS.human_detector() << "/" 
          << robS.human_group();

        ATH_MSG_DEBUG(str.str() );
      }
    }    
  }
   
  return true;
}
