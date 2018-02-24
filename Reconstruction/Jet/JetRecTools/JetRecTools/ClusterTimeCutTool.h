#ifndef JETRECTOOLS_CLUSTERTIMECUTTOOL_H
#define JETRECTOOLS_CLUSTERTIMECUTTOOL_H

// \class ClusterTimeCutTool
// \author Jennifer Roloff
// \date October 2016
//
// This tool places cuts on (ECal) clusters based on their timing information


#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <string>

class ClusterTimeCutTool : public JetConstituentModifierBase{
  ASG_TOOL_CLASS(ClusterTimeCutTool, IJetConstituentModifier)

  public:
  
  ClusterTimeCutTool(const std::string& name);
  ~ClusterTimeCutTool();
  StatusCode process(xAOD::IParticleContainer* cont) const; 
  StatusCode process(xAOD::CaloClusterContainer* cont) const; // MEN: Might need to rename this process

  private:
  // Properties.
  float m_lambdaCalDivide; 
  float m_qualityCut; 
  float m_timeCut1; 
  float m_timeCut2; 

		
};


#endif
