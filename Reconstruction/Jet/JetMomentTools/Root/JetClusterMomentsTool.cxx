/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetMomentTools/JetClusterMomentsTool.h"

JetClusterMomentsTool::JetClusterMomentsTool(const std::string& name)
    : JetModifierBase(name)
{
    declareProperty( "DoClsPt"           , m_doClsPt           = true );
    declareProperty( "DoClsSecondLambda" , m_doClsSecondLambda = true );
    declareProperty( "DoClsCenterLambda" , m_doClsCenterLambda = true );
    declareProperty( "DoClsSecondR"      , m_doClsSecondR      = true );
}

int JetClusterMomentsTool::modifyJet(xAOD::Jet& jet) const {
    // Find leading constituent cluster
    const xAOD::CaloCluster * leadingCluster = findLeadingCluster(jet);
    ATH_MSG_DEBUG("Leading cluster retrieving finished.");
    if (!leadingCluster){
        ATH_MSG_WARNING("Jet has no CaloCluster constituents, leading cluster not found");
        return 0;
    }
    // Collate info
    float LeadingClusterPt           = (m_doClsPt           ) ? leadingCluster->          pt()                             :  0.0;
    float LeadingClusterSecondLambda = (m_doClsSecondLambda ) ? getMoment(leadingCluster, xAOD::CaloCluster::SECOND_LAMBDA ): 0.0;
    float LeadingClusterCenterLambda = (m_doClsCenterLambda ) ? getMoment(leadingCluster, xAOD::CaloCluster::CENTER_LAMBDA ): 0.0;
    float LeadingClusterSecondR      = (m_doClsSecondR      ) ? getMoment(leadingCluster, xAOD::CaloCluster::SECOND_R      ): 0.0;
    // Set info
    if (m_doClsPt           ) jet.setAttribute( "LeadingClusterPt"           , LeadingClusterPt           );
    if (m_doClsSecondLambda ) jet.setAttribute( "LeadingClusterSecondLambda" , LeadingClusterSecondLambda );
    if (m_doClsCenterLambda ) jet.setAttribute( "LeadingClusterCenterLambda" , LeadingClusterCenterLambda );
    if (m_doClsSecondR      ) jet.setAttribute( "LeadingClusterSecondR"      , LeadingClusterSecondR      );
    return 0;
}




// Factorize the cluster finding out of the class.
// The only reason to do that is to be able to write a function taking a const argument
//  without having to change the tool interface. And this just to make sure the corresponding
//  tag can be collected in a cache release. 
// (on longer term there's no reason to do this and we can just redefine findLeadingCluster() to
//  take a const arg).
//
const xAOD::CaloCluster * findLeadingClusterImpl(const xAOD::Jet& jet)  {

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
      const xAOD::CaloCluster * cl_current = findLeadingClusterImpl( *jet_constit ) ;
      if(cl_current)  if (!cl_leading || cl_leading->pt() < cl_current->pt() ) cl_leading = cl_current;
    }
    
  } break;
  default:
    break;
  } // end switch
  return cl_leading;
}

const xAOD::CaloCluster * JetClusterMomentsTool::findLeadingCluster(xAOD::Jet& jet) const {
  return findLeadingClusterImpl(jet);
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
