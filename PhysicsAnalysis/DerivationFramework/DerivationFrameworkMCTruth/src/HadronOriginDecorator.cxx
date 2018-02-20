/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkMCTruth/HadronOriginDecorator.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "DerivationFrameworkMCTruth/HadronOriginClassifier.h"

namespace DerivationFramework {

  HadronOriginDecorator::HadronOriginDecorator(const std::string& t, const std::string& n, const IInterface* p):
    AthAlgTool(t,n,p),
    m_Tool("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("ToolName",m_Tool);
    declareProperty("TruthEventName",m_TruthEventName="TruthParticles");
  }

  HadronOriginDecorator::~HadronOriginDecorator(){}

  StatusCode HadronOriginDecorator::initialize(){
    ATH_MSG_INFO("Initialize " );

    if(m_Tool.retrieve().isFailure()){
      ATH_MSG_ERROR("unable to retrieve the tool " <<m_Tool);
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode HadronOriginDecorator::finalize(){
    return StatusCode::SUCCESS;
  }

  StatusCode HadronOriginDecorator::addBranches() const{
    const xAOD::TruthParticleContainer* xTruthParticleContainer = 0;
    if (evtStore()->retrieve(xTruthParticleContainer,m_TruthEventName).isFailure()) {
      ATH_MSG_WARNING("could not retrieve TruthParticleContainer " <<m_TruthEventName);
      return StatusCode::FAILURE;
    }

    std::map<const xAOD::TruthParticle*, DerivationFramework::HadronOriginClassifier::HF_id>  hadronMap=m_Tool->GetOriginMap();

    for(xAOD::TruthParticleContainer::const_iterator PItr = xTruthParticleContainer->begin(); PItr!=xTruthParticleContainer->end(); ++PItr){
      int flavortype=6;
      if(hadronMap.find((*PItr))!=hadronMap.end()){
        flavortype= static_cast<int>(hadronMap[(*PItr)]);
      }
      SG::AuxElement::Decorator< int > decoration("TopHadronOriginFlag");
      decoration(**PItr) = flavortype;
    }

    return StatusCode::SUCCESS;
  }

} /// namespace
