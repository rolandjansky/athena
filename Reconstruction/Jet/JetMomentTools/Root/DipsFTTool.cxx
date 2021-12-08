/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// DipsFTTool.cxx

#include "JetMomentTools/DipsFTTool.h"
#include "AsgDataHandles/WriteDecorHandle.h"
// #include "xAODJet/JetConstituentVector.h"

//**********************************************************************

DipsFTTool::DipsFTTool(std::string myname)
  : asg::AsgTool(myname)
{
}

//**********************************************************************

StatusCode DipsFTTool::initialize(){

  if(m_jetContainerName.empty()){
    ATH_MSG_ERROR("DipsFTTool needs to have its input jet container name configured!");
    return StatusCode::FAILURE;
  }

  // Prepend jet container name
  m_scoreKey = m_jetContainerName + "." + m_scoreKey.key();

  ATH_CHECK(m_scoreKey.initialize());
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode DipsFTTool::decorate(const xAOD::JetContainer& jets) const {

  SG::WriteDecorHandle<xAOD::JetContainer, float> scoreHandle(m_scoreKey);

  for(const xAOD::Jet* jet : jets){
    scoreHandle(*jet) = score(*jet);
  }

  return StatusCode::SUCCESS;
}

//**********************************************************************

float DipsFTTool::score(const xAOD::Jet& jet) const {
  //Get DIPZ score here

  return -1;
}

