#include <vector>
#include "JetRecTools/ClusterTimeCutTool.h"

using namespace std;


ClusterTimeCutTool::ClusterTimeCutTool(const std::string& name) : JetConstituentModifierBase(name)
                                                                , m_lambdaCalDivide(0)
                                                                , m_qualityCut(0)
                                                                , m_timeCut1(0)
                                                                , m_timeCut2(0)
{
#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty("LambdaCalDivide", m_lambdaCalDivide = 317);
  declareProperty("QualityCut", m_qualityCut = 0.02);
  declareProperty("TimeCut1", m_timeCut1 = 5);
  declareProperty("TimeCut2", m_timeCut2 = 15);
}


StatusCode ClusterTimeCutTool::process(xAOD::CaloClusterContainer* cont) const {
  xAOD::CaloClusterContainer::iterator cl = cont->begin();
  xAOD::CaloClusterContainer::iterator cl_end = cont->end();

  for( ; cl != cl_end; ++cl ) {
		if(abs( (*cl)->eta() ) < 2.5){
			double quality =(*cl)->auxdata<float>("AVG_LAR_Q");
                        quality /= 65535; //quality is on [0,2^16-1] scale
			double lambda_center =(*cl)->auxdata<float>("CENTER_LAMBDA");
			double time = (*cl)->time();
			if(lambda_center > m_lambdaCalDivide) continue;
			if( quality > m_qualityCut && abs(time) > m_timeCut1){
				(*cl)->setE( 0 );
			}
			if( quality <= m_qualityCut && abs(time) > m_timeCut2){
				(*cl)->setE( 0 );
			}
		}

  }

  return StatusCode::SUCCESS;
}

StatusCode ClusterTimeCutTool::process(xAOD::IParticleContainer* cont) const {
  xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container
  if(clust) return process(clust);
  return StatusCode::FAILURE;
}


ClusterTimeCutTool::~ClusterTimeCutTool(){
}


