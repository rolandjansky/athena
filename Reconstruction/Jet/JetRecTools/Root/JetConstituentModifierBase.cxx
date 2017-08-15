/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Source file for the JetConstituentModifierBase.h
// // Michael Nelson, CERN & University of Oxford 

#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODPFlow/PFO.h"

JetConstituentModifierBase::JetConstituentModifierBase(const std::string & name): asg::AsgTool(name) {
#ifdef ASGTOOL_ATHENA  
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty("InputType", m_inputType);
  declareProperty("ApplyToChargedPFO", m_applyToChargedPFO);
  declareProperty("ApplyToNeutralPFO", m_applyToNeutralPFO);
}

StatusCode JetConstituentModifierBase::process(xAOD::IParticleContainer* cont) const
{
  // Test that we are operating on the type of object that
  // we will be writing out.
  // By implication, any supporting containers should not
  // be the ones passed to this method...
  if(!cont->empty() && cont->front()->type() != m_inputType) {
    ATH_MSG_ERROR("Object type mismatch! This tool expects " << m_inputType
		  << ", but received " << cont->front()->type());
    return StatusCode::FAILURE;
  }

  ATH_CHECK(process_impl(cont));

  return StatusCode::SUCCESS;
}


StatusCode JetConstituentModifierBase::setEtaPhi(xAOD::IParticle* obj, float eta, float phi) const
{
  switch(m_inputType) {
    // The main (only?) application is origin-correcting LC topoclusters
    // By convention we leave the raw p4 unmodified
  case xAOD::Type::CaloCluster:
    {
      xAOD::CaloCluster* clus = static_cast<xAOD::CaloCluster*>(obj);
      clus->setCalEta(eta);
      clus->setCalPhi(phi);
    }
    break;
  default:
    // Should not get here, because type-checking should happen in process()
    ATH_MSG_ERROR("No specialisation for object type " << m_inputType);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode JetConstituentModifierBase::setEnergyPt(xAOD::IParticle* obj, float e, float pt,
						   const SG::AuxElement::Accessor<float>* weightAcc) const
{
  switch(m_inputType) {
  case xAOD::Type::CaloCluster:
    {
      xAOD::CaloCluster* clus = static_cast<xAOD::CaloCluster*>(obj);
      // Clusters get pt via the energy
      // This currently leaves the mass unaltered.
      // if(weightAcc) (*weightAcc)(*clus) = clus->calE() > FLT_MIN ? e / clus->calE() : 0.;
      clus->setCalE(e);
    }
    break;
  case xAOD::Type::ParticleFlow:
    {
      xAOD::PFO* pfo = static_cast<xAOD::PFO*>(obj);
      // Use accessors explicitly because PFO only has an interface for setting the full p4
      const static SG::AuxElement::Accessor<float> accPt("pt");
      const static SG::AuxElement::Accessor<float> accE("e");
      if( (m_applyToChargedPFO && fabs(pfo->charge())>=1e-9) || 
	  (m_applyToNeutralPFO && fabs(pfo->charge())<1e-9) ) {
	// if(weightAcc) (*weightAcc)(*pfo) = pfo->pt() > FLT_MIN ? pt / pfo->pt() : 0.;
	accPt(*pfo) = pt;
	accE(*pfo) = e;
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

StatusCode JetConstituentModifierBase::setP4(xAOD::IParticle* obj, const xAOD::JetFourMom_t& p4,
					     const SG::AuxElement::Accessor<float>* weightAcc) const {
  switch(m_inputType) {
  case xAOD::Type::CaloCluster:
    {
      xAOD::CaloCluster* clus = static_cast<xAOD::CaloCluster*>(obj);
      // This currently leaves the mass unaltered
      // if(weightAcc) (*weightAcc)(*clus) = clus->calE() > FLT_MIN ? p4.e() / clus->calE() : 0.;
      clus->setCalE(p4.e());
      clus->setCalEta(p4.eta());
      clus->setCalPhi(p4.phi());
    }
    break;
  case xAOD::Type::ParticleFlow:
    {
      xAOD::PFO* pfo = static_cast<xAOD::PFO*>(obj);
      // The PFO setter defaults to m=0
      if( (m_applyToChargedPFO && fabs(pfo->charge())>=1e-9) || 
	  (m_applyToNeutralPFO && fabs(pfo->charge())<1e-9) ) {
	// if(weightAcc) (*weightAcc)(*pfo) = pfo->pt() > FLT_MIN ? p4.pt() / pfo->pt() : 0.;
	pfo->setP4(p4.pt(),p4.eta(),p4.phi(),p4.mass());
      }
      break;
    }
  default:
    // Should not get here, because type-checking should happen in process()
    ATH_MSG_ERROR("No specialisation for object type " << m_inputType);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
