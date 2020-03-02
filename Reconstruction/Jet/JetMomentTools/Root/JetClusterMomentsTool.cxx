/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMomentTools/JetClusterMomentsTool.h"
#include "StoreGate/WriteDecorHandle.h"

JetClusterMomentsTool::JetClusterMomentsTool(const std::string& name)
    : asg::AsgTool(name)
{
  declareInterface<IJetDecorator>(this);
}

StatusCode JetClusterMomentsTool::initialize(){
  if(m_jetContainerName.empty()){
    ATH_MSG_ERROR("JetBadChanCorrTool needs to have its input jet container name configured!");
    return StatusCode::FAILURE;
  }

  // Initialize whichever DecorHandleKeys we're configured for
  if(!m_clsPtKey.empty()){
    m_clsPtKey = m_jetContainerName + "." + m_clsPtKey.key();
    ATH_CHECK(m_clsPtKey.initialize());
  }
  if(!m_clsSecondLambdaKey.empty()){
    m_clsSecondLambdaKey = m_jetContainerName + "." + m_clsSecondLambdaKey.key();
    ATH_CHECK(m_clsSecondLambdaKey.initialize());
  }
  if(!m_clsCenterLambdaKey.empty()){
    m_clsCenterLambdaKey = m_jetContainerName + "." + m_clsCenterLambdaKey.key();
    ATH_CHECK(m_clsCenterLambdaKey.initialize());
  }
  if(!m_clsSecondRKey.empty()){
    m_clsSecondRKey = m_jetContainerName + "." + m_clsSecondRKey.key();
    ATH_CHECK(m_clsSecondRKey.initialize());
  }
  return StatusCode::SUCCESS;
}

StatusCode JetClusterMomentsTool::decorate(const xAOD::JetContainer& jets) const {

  // Use pointers here so we can create only the ones we're configured for
  SG::WriteDecorHandle<xAOD::JetContainer, float>* handlePt(nullptr);
  SG::WriteDecorHandle<xAOD::JetContainer, float>* handleSecondLambda(nullptr);
  SG::WriteDecorHandle<xAOD::JetContainer, float>* handleCenterLambda(nullptr);
  SG::WriteDecorHandle<xAOD::JetContainer, float>* handleSecondR(nullptr);

  if(!m_clsPtKey.empty())           handlePt           = new SG::WriteDecorHandle<xAOD::JetContainer, float>(m_clsPtKey);
  if(!m_clsSecondLambdaKey.empty()) handleSecondLambda = new SG::WriteDecorHandle<xAOD::JetContainer, float>(m_clsSecondLambdaKey);
  if(!m_clsCenterLambdaKey.empty()) handleCenterLambda = new SG::WriteDecorHandle<xAOD::JetContainer, float>(m_clsCenterLambdaKey);
  if(!m_clsSecondRKey.empty())      handleSecondR      = new SG::WriteDecorHandle<xAOD::JetContainer, float>(m_clsSecondRKey);

  for(const xAOD::Jet* jet : jets){
    // Find leading constituent cluster
    const xAOD::CaloCluster* leadingCluster = findLeadingCluster(*jet);
    ATH_MSG_DEBUG("Leading cluster retrieving finished.");
    if(!leadingCluster){
      ATH_MSG_WARNING("Jet has no CaloCluster constituents, leading cluster not found");
      if(handlePt)           delete handlePt;
      if(handleSecondLambda) delete handleSecondLambda;
      if(handleCenterLambda) delete handleCenterLambda;
      if(handleSecondR)      delete handleSecondR;
      return StatusCode::FAILURE;
    }
    // Set info
    if(handlePt)           (*handlePt)(*jet)           = leadingCluster->pt();
    if(handleSecondLambda) (*handleSecondLambda)(*jet) = getMoment(leadingCluster, xAOD::CaloCluster::SECOND_LAMBDA);
    if(handleCenterLambda) (*handleCenterLambda)(*jet) = getMoment(leadingCluster, xAOD::CaloCluster::CENTER_LAMBDA);
    if(handleSecondR)      (*handleSecondR)(*jet)      = getMoment(leadingCluster, xAOD::CaloCluster::SECOND_R);
  }
  if(handlePt)           delete handlePt;
  if(handleSecondLambda) delete handleSecondLambda;
  if(handleCenterLambda) delete handleCenterLambda;
  if(handleSecondR)      delete handleSecondR;
  return StatusCode::SUCCESS;
}

const xAOD::CaloCluster* JetClusterMomentsTool::findLeadingCluster(const xAOD::Jet& jet) const{

    // Retrieve the associated clusters.

  if ( jet.numConstituents() == 0 ) return NULL;
  const xAOD::CaloCluster * cl_leading = NULL;


  switch( jet.rawConstituent(0)->type() ) {
  case xAOD::Type::CaloCluster : {
    for (size_t i_cl = 0; i_cl < jet.numConstituents(); i_cl++){ // loop all constituents
      const xAOD::CaloCluster * cl_current = dynamic_cast<const xAOD::CaloCluster*> (jet.rawConstituent(i_cl));
      if (!cl_leading || cl_leading->pt() < cl_current->pt() ) cl_leading = cl_current;
    }
    
  } break;
    
  case xAOD::Type::Jet: {
    for (size_t i_cl = 0; i_cl < jet.numConstituents(); i_cl++){ // loop all constituents
      const xAOD::Jet * jet_constit = dynamic_cast<const xAOD::Jet*> (jet.rawConstituent(i_cl));
      if (!jet_constit) continue;
      const xAOD::CaloCluster * cl_current = findLeadingCluster( *jet_constit ) ;
      if(cl_current)  if (!cl_leading || cl_leading->pt() < cl_current->pt() ) cl_leading = cl_current;
    }
    
  } break;
  default:
    break;
  } // end switch
  return cl_leading;
}

float JetClusterMomentsTool::getMoment(const xAOD::CaloCluster* cluster, const xAOD::CaloCluster::MomentType& momentType) const {
    if (cluster){ // if we have a pointer than read the moment
        double moment = 0.0;
        bool isRetrieved = cluster->retrieveMoment(momentType, moment);
        if (isRetrieved == true) return (float) moment;
    }
    ATH_MSG_WARNING("Can not retrieved moment from cluster");
    return 0.0;
}
