/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include "JetRecTools/ConstitTimeCutTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODPFlow/PFODefs.h"
#include "PFlowUtils/FEHelpers.h"

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
    if(m_inputType!=xAOD::Type::CaloCluster && m_inputType!= xAOD::Type::FlowElement) {
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
     const static SG::AuxElement::ConstAccessor<float> acc_larq("AVG_LAR_Q");
     const static SG::AuxElement::ConstAccessor<float> acc_clambda("CENTER_LAMBDA");

     for(xAOD::CaloCluster* cl : *clusters) {
        //quality is on [0,2^16-1] scale
        ATH_CHECK( applyTimingCut(cl, cl->time(), acc_larq(*cl)/65535, acc_clambda(*cl)));
      }
    }
    break;
  case xAOD::Type::ParticleFlow:
    {
      xAOD::PFOContainer* pfos = static_cast<xAOD::PFOContainer*> (cont);
      for(xAOD::PFO* pfo : *pfos) {
        if(fabs(pfo->charge())<FLT_MIN || m_applyToChargedPFO) { // only apply to neutrals if m_applyToChargedPFO is false. If m_applyToChargedPFO is true then apply to all POs.
          float time(0.);
          float quality(0.);
          float lambda_center(0.);
          // Only apply cut if retrieval succeeded, else warn
          if(pfo->attribute(xAOD::PFODetails::eflowRec_TIMING,time) &&
             pfo->attribute(xAOD::PFODetails::eflowRec_AVG_LAR_Q,quality) &&
             pfo->attribute(xAOD::PFODetails::eflowRec_CENTER_LAMBDA,lambda_center)
            ) {
            //quality is on [0,2^16-1] scale
            ATH_CHECK( applyTimingCut(pfo, time, quality/65535, lambda_center) );
          } else {
            ATH_MSG_WARNING("Failed to retrieve the PFO informations necessary for timing cut at PFO #" << pfo->index());
          }
        }
      }
    }
    break;
  case xAOD::Type::FlowElement:
    {
      xAOD::FlowElementContainer* fes = static_cast<xAOD::FlowElementContainer*>(cont);
      if(!fes->empty() && !(fes->front()->signalType() & xAOD::FlowElement::PFlow)){
        ATH_MSG_ERROR("ConstitTimeCutTool received FlowElements that aren't PFOs, this isn't supported!");
        return StatusCode::FAILURE;
      }
      for(xAOD::FlowElement* fe : *fes){
        if(!fe->isCharged() || m_applyToChargedPFO){
          const static SG::AuxElement::ConstAccessor<float> acc_timing("TIMING");
          const static SG::AuxElement::ConstAccessor<float> acc_larq("AVG_LAR_Q");
          const static SG::AuxElement::ConstAccessor<float> acc_clambda("CENTER_LAMBDA");
          float time = acc_timing(*fe);
          float quality = acc_larq(*fe);
          float lambda_center = acc_clambda(*fe);
          ATH_CHECK( applyTimingCut(fe, time, quality/65535, lambda_center) );
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

StatusCode ConstitTimeCutTool::applyTimingCut(xAOD::IParticle* part, const float& time, const float& quality, const float& lambda_center) const {
  if(abs( part->eta() ) < m_etaMax){

    // Only apply to ECal by default (with depth cut)
    // Should switch to a test on the dominant sampling??
    if(lambda_center < m_lambdaCalDivide) {
      // Apply variable timing cut based on he cluster quality
      // -- a larger spread is observed for clusters with small Q
      float timeCut = quality > m_qualityCut ? m_timeCutLargeQ : m_timeCutSmallQ;
      if( abs(time) > timeCut ) { ATH_CHECK( setEnergyPt( part, 0., 0. )); }
    }
  }

  return StatusCode::SUCCESS;
}