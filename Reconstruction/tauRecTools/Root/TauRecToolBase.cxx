/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauRecToolBase.h"
#include "PathResolver/PathResolver.h"



TauEventData defaultTauEventData;

//________________________________________
std::string TauRecToolBase::find_file(const std::string& fname) const {
  static const std::string m_tauRecToolsTag="tauRecTools/00-00-00/";
  std::string full_path = PathResolverFindCalibFile(m_tauRecToolsTag+fname);
  if(full_path=="") full_path = PathResolverFindCalibFile(fname);
  return full_path;
}

//________________________________________
void TauRecToolBase::setTauEventData(TauEventData* data){
  m_data=data;
  if(m_data==0) {
    m_data=&defaultTauEventData;
    m_data->clear();
  }
}

//________________________________________
TauRecToolBase::TauRecToolBase(const std::string& name) :
  asg::AsgTool(name) {

}

//________________________________________
StatusCode TauRecToolBase::initialize(){
  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode TauRecToolBase::eventInitialize(){
  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode TauRecToolBase::execute(xAOD::TauJet&){
  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode TauRecToolBase::eventFinalize(){
  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode TauRecToolBase::finalize(){
  return StatusCode::SUCCESS;
}
