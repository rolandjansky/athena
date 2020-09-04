/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauRecToolBase.h"
#include "PathResolver/PathResolver.h"

// ROOT include(s)
#include "TEnv.h"
#include "THashList.h"

#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <cstdlib>


std::string TauRecToolBase::find_file(const std::string& fname) const {
  std::string full_path;
  //offline calib files are in GroupData
  //online calib files are in release
  full_path = PathResolverFindCalibFile(m_tauRecToolsTag+"/"+fname);
  if(full_path=="") full_path = PathResolverFindCalibFile(fname);
  return full_path;
}
StatusCode TauRecToolBase::readConfig() {
  // Sanity check to see if property ConfigPath is declared for a tool. Might be
  // removed once all tools are updated to have a config path declared.
  // in athena getProperties returns std::vector<Gaudi::Details::PropertyBase*>
  // in rc     getProperties returns std::map<std::string,Property*>
#ifndef XAOD_STANDALONE
  bool configPathDeclared = false;
  for (Gaudi::Details::PropertyBase* property : getProperties())
  {
    if (property->name() == "ConfigPath")
    {
      configPathDeclared = true;
      break;
    }
  }
  if (!configPathDeclared)
#elif defined(XAOD_STANDALONE)
  PropertyMgr::PropMap_t property_map = getPropertyMgr()->getProperties();
  if (property_map.find("ConfigPath") == property_map.end())
#else
#   error "What environment are we in?!?"
#endif // XAOD_STANDALONE
  {
    ATH_MSG_INFO("No config file path property declared yet, this is not recommended");
    return StatusCode::SUCCESS;
  }
  
  // get configured config path and load file via TEnv
  const std::string* config_file_path_property;
  // if (getProperty("ConfigPath", config_file_path).isFailure())
  //   return StatusCode::FAILURE;
  config_file_path_property = getProperty<std::string>("ConfigPath");
  std::string config_file_path = find_file(*config_file_path_property);
  TEnv env;
  env.ReadFile(PathResolverFindCalibFile(config_file_path).c_str(),kEnvAll);

  THashList* lList = env.GetTable();
  for( Int_t i = 0; lList && i < lList->GetEntries(); ++i )
  {
    StatusCode sc;
    // types of properties are handled differently as well 
#ifndef XAOD_STANDALONE
    // get type of variable with the entry name
    const std::type_info* type = getProperty(lList->At( i )->GetName()).type_info();

    // search for type is needed by env.GetValue function (needs a variable of the correct type as 2nd argument)
    if (*type == typeid(bool))
      sc = this->setProperty(lList->At( i )->GetName(),
        bool(env.GetValue(lList->At( i )->GetName(),bool(true))));
    else if (*type == typeid(int))
      sc = this->setProperty(lList->At( i )->GetName(),
        env.GetValue(lList->At( i )->GetName(),int(0)));
    else if (*type == typeid(float))
      sc = this->setProperty(lList->At( i )->GetName(),
        env.GetValue(lList->At( i )->GetName(),float(0)));
    else if (*type == typeid(double))
      sc = this->setProperty(lList->At( i )->GetName(),
        env.GetValue(lList->At( i )->GetName(),double(0)));
    else if (*type == typeid(std::string))
      sc = this->setProperty(lList->At( i )->GetName(),
        env.GetValue(lList->At( i )->GetName(),""));
#else
    // get type of variable with the entry name
    Property::Type type = getPropertyMgr()->getProperty(lList->At( i )->GetName())->type();

    if (type == Property::BOOL)
      sc = this->setProperty(lList->At( i )->GetName(),
        bool(env.GetValue(lList->At( i )->GetName(),bool(true))));
    else if (type == Property::INT)
      sc = this->setProperty(lList->At( i )->GetName(),
        env.GetValue(lList->At( i )->GetName(),int(0)));
    else if (type == Property::FLOAT)
      sc = this->setProperty(lList->At( i )->GetName(),
        env.GetValue(lList->At( i )->GetName(),float(0)));
    else if (type == Property::DOUBLE)
      sc = this->setProperty(lList->At( i )->GetName(),
        env.GetValue(lList->At( i )->GetName(),double(0)));
    else if (type == Property::STRING)
      sc = this->setProperty(lList->At( i )->GetName(),
        env.GetValue(lList->At( i )->GetName(),""));
#endif // XAOD_STANDALONE
    else
    {
#ifndef XAOD_STANDALONE
      ATH_MSG_FATAL("there was a problem to find the correct type enum: "<<type->name());
#else
      ATH_MSG_FATAL("there was a problem to find the correct type enum: "<<type);
#endif // XAOD_STANDALONE
      return StatusCode::FAILURE;
    }
    if (!sc.isSuccess()) {
      ATH_MSG_FATAL("failed to set property: " << lList->At( i )->GetName());
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

TauRecToolBase::TauRecToolBase(const std::string& name) :
  asg::AsgTool(name) {
  declareProperty("inTrigger", m_in_trigger=false);
  declareProperty("calibFolder", m_tauRecToolsTag="tauRecTools/00-02-00/"); 
}

StatusCode TauRecToolBase::initialize(){
  return StatusCode::SUCCESS;
}

StatusCode TauRecToolBase::eventInitialize(){
  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode TauRecToolBase::execute(xAOD::TauJet&) const {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
}

#ifdef XAOD_ANALYSIS
StatusCode TauRecToolBase::executeDev(xAOD::TauJet&) {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
}
#else
StatusCode TauRecToolBase::executePi0CreateROI(xAOD::TauJet& /*pTau*/, CaloCellContainer& /*caloCellContainer*/, std::vector<CaloCell*>& /*map*/ ) {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
}
#endif

StatusCode TauRecToolBase::executeVertexFinder(xAOD::TauJet&, const xAOD::VertexContainer*, const xAOD::TrackParticleContainer*) const {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
}

StatusCode TauRecToolBase::executeTrackFinder(xAOD::TauJet&, xAOD::TauTrackContainer&, const xAOD::TrackParticleContainer*) const {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
}

StatusCode TauRecToolBase::executeTrackClassifier(xAOD::TauJet&, xAOD::TauTrackContainer&) const {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
}

StatusCode TauRecToolBase::executeRNNTrackClassifier(xAOD::TauJet&, xAOD::TauTrackContainer&) const {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
}

StatusCode TauRecToolBase::executeShotFinder(xAOD::TauJet& /*pTau*/, xAOD::CaloClusterContainer& /*shotClusterContainer*/, xAOD::PFOContainer& /*PFOContainer*/ ) const {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
}

StatusCode TauRecToolBase::executePi0ClusterCreator(xAOD::TauJet& /*pTau*/, xAOD::PFOContainer& /*neutralPFOContainer*/, 
					      xAOD::PFOContainer& /*hadronicPFOContainer*/, 
					      xAOD::CaloClusterContainer& /*caloClusterContainer*/, 
					      const xAOD::CaloClusterContainer& /*pCaloClusterContainer*/ ) const {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
}

StatusCode TauRecToolBase::executeVertexVariables(xAOD::TauJet& /*pTau*/, xAOD::VertexContainer& /*vertexContainer*/ ) const {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
}

StatusCode TauRecToolBase::executePi0ClusterScaler(xAOD::TauJet& /*pTau*/, xAOD::PFOContainer& /*neutralPFOContainer*/, xAOD::PFOContainer& /*chargedPFOContainer*/ ) const {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
} 

StatusCode TauRecToolBase::executePi0nPFO(xAOD::TauJet& /*pTau*/, xAOD::PFOContainer& /*neutralPFOContainer*/) const {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
}

StatusCode TauRecToolBase::executePanTau(xAOD::TauJet& /*pTau*/, xAOD::ParticleContainer& /*particleContainer*/) {
  ATH_MSG_ERROR("function not implemented");
  return StatusCode::FAILURE;
}

StatusCode TauRecToolBase::eventFinalize() {
  return StatusCode::SUCCESS;
}

StatusCode TauRecToolBase::finalize(){
  return StatusCode::SUCCESS;
}
