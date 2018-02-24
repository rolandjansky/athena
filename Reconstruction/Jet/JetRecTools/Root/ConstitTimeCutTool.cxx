#include <vector>
#include "JetRecTools/ConstitTimeCutTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODPFlow/PFOContainer.h"

using namespace std;


ConstitTimeCutTool::ConstitTimeCutTool(const std::string& name) : JetConstituentModifierBase(name)
{
  declareProperty("LambdaCalDivide", m_lambdaCalDivide = 317);
  declareProperty("QualityCut", m_qualityCut = 0.02);
  declareProperty("TimeCutLargeQ", m_timeCutLargeQ = 5);
  declareProperty("TimeCutSmallQ", m_timeCutSmallQ = 15);
  declareProperty("MaxEtaForCut", m_etaMax = 2.5);
}

StatusCode ConstitTimeCutTool::initialize() {

  if(m_inputType==xAOD::Type::ParticleFlow) {
    if(!m_applyToNeutralPFO) {
      ATH_MSG_ERROR("Incompatible configuration: ApplyToNeutralPFO=False -- what kind of pileup do you wish to suppress?");
      return StatusCode::FAILURE;
    }
  } else {
    if(m_inputType!=xAOD::Type::CaloCluster) {
      ATH_MSG_ERROR("Incompatible configuration: ConstitTimeCutTool is not specialised for inputs of type "
		    << m_inputType);
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

// Determine the appropriate input type and extract the timing, then
// call the applyTimingCut method
StatusCode ConstitTimeCutTool::process_impl(xAOD::IParticleContainer* cont) const {
  switch(m_inputType) {
  case xAOD::Type::CaloCluster:
    {
      xAOD::CaloClusterContainer* clusters = static_cast<xAOD::CaloClusterContainer*> (cont);
      for(xAOD::CaloCluster* cl : *clusters) {
	float time = cl->time();
	ATH_CHECK( applyTimingCut(cl, time) );
      }
    }
    break;
  case xAOD::Type::ParticleFlow:
    {
      xAOD::PFOContainer* pfos = static_cast<xAOD::PFOContainer*> (cont);
      for(xAOD::PFO* pfo : *pfos) {
	if(fabs(pfo->charge())<FLT_MIN) { // only apply to neutrals
	  float time(0);
	  // Only apply cut if retrieval succeeded, else warn
	  if(pfo->attribute<float>(xAOD::PFODetails::eflowRec_TIMING,time)) {
	    ATH_CHECK( applyTimingCut(pfo, time) );
	  } else {
	    ATH_MSG_WARNING("Failed to retrieve PFO timing for PFO #" << pfo->index());
	  }
	}
      }
    }
    break;
  default:
    // Should not get here, because type-checking should happen in process()
    ATH_MSG_ERROR("No specialisation for object type " << m_inputType);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode ConstitTimeCutTool::applyTimingCut(xAOD::IParticle* part, float time) const {
  const static SG::AuxElement::ConstAccessor<float> acc_larq("AVG_LAR_Q");
  const static SG::AuxElement::ConstAccessor<float> acc_clambda("CENTER_LAMBDA");

  if(abs( part->eta() ) < m_etaMax){
    //quality is on [0,2^16-1] scale
    float quality = acc_larq(*part) / 65535;
    float lambda_center = acc_clambda(*part);

    // Only apply to ECal by default (with depth cut)
    // Should switch to a test on the dominant sampling??
    if(lambda_center < m_lambdaCalDivide) {
      // Apply variable timing cut based on he cluster quality
      // -- a larger spread is observed for clusters with small Q
      float timeCut = quality > m_qualityCut ? m_timeCutLargeQ : m_timeCutSmallQ;
      if( abs(time) > timeCut ) {setEnergyPt( part, 0., 0. );}
    }
  }

  return StatusCode::SUCCESS;
}
