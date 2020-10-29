/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CheckForFinalPartons.h"

CheckForFinalPartons::CheckForFinalPartons(const std::string &type, const std::string &name, const IInterface *parent):
AthAlgTool(type, name, parent), m_nFailures(0){
  declareInterface<IPythia8Custom>(this);
  declareProperty("MaxFailures", m_maxFailures=5);
}

/*
 StatusCode initialize(){
 
 return StatusCode::SUCCESS;
 }
 
 StatusCode finalize(){
 
 return StatusCode::SUCCESS;
 }
 */
StatusCode CheckForFinalPartons::ModifyPythiaEvent(Pythia8::Pythia &pythia) const {
  
  bool status = true;
  while( !(acceptEvent(pythia.event) && status) && m_nFailures <= m_maxFailures ){
    status = pythia.next();
    ++ m_nFailures;
    ATH_MSG_WARNING("Event rejected due to unhadronised partons");
    ATH_MSG_WARNING("Events rejected so far: "<<m_nFailures<< " (limit "<<m_maxFailures<<")");
  }
  
  if(m_nFailures > m_maxFailures){
    ATH_MSG_ERROR("Too many failures due to unhadronised partons in a single job.");
    ATH_MSG_ERROR("Maximum allowed failures per job = " << m_maxFailures);
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}


bool CheckForFinalPartons::acceptEvent(Pythia8::Event &event) const {
  
  for(int ii=0; ii != event.size(); ++ii){
    
    if(!event[ii].isFinal()) continue;
    
    if(abs(event[ii].id()) < 7 || event[ii].id() == 21 ) return false;
    
  }
  
  return true;
}

