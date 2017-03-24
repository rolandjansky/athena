#include "JetRecTools/PuppiWeightTool.h"

#include "fastjet/PseudoJet.hh"
#include <vector>

#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "JetRecTools/Puppi.h"

using namespace std;
using namespace fastjet;

//------------------------------------------------------------------------------
PuppiWeightTool::PuppiWeightTool(const std::string& name) : JetConstituentModifierBase(name) {
#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif

  declareProperty("R0", m_R0 = 0.3);
  declareProperty("Rmin", m_Rmin = 0.001);
  declareProperty("Exponent", m_exponent = 1);
  declareProperty("WeightCut",m_weightCut = 0);
  declareProperty("ScaleFactorA", m_scaleFactorA = 0);
  declareProperty("ScaleFactorB", m_scaleFactorB = 0);
  declareProperty("ForwardScaleFactor",m_forwardSF = 1);
  declareProperty("EtaForward",m_etaForward = 2.5);
  declareProperty("ApplyWeight",m_applyWeight=true);

  // Where we no longer have charged and neutral information
  //m_etaForward = 2.5;
}

//------------------------------------------------------------------------------

StatusCode PuppiWeightTool::process(xAOD::IParticleContainer* cont) const {
  xAOD::PFOContainer* pfoCont = dynamic_cast<xAOD::PFOContainer*> (cont);
  if(pfoCont) return process(pfoCont);
  else{
    ATH_MSG_ERROR("Unable to dynamic cast IParticleContainer to PFOContainer");
    return StatusCode::FAILURE;
  }
}

//------------------------------------------------------------------------------

StatusCode PuppiWeightTool::process(xAOD::PFOContainer* cont) const{
  ATH_MSG_WARNING("Testing PUPPI");
 
  SG::AuxElement::Accessor<bool> PVMatchedAcc("matchedToPV");
  SG::AuxElement::Accessor<double> alphaAcc("PUPPI_alpha");
  SG::AuxElement::Accessor<double> weightAcc("PUPPI_weight");

  std::vector<fastjet::PseudoJet> chargedHSVector;
  std::vector<fastjet::PseudoJet> chargedPUVector;
  std::vector<fastjet::PseudoJet> neutralAndForwardVector;

  // Fill input particle vectors for puppi
  for ( xAOD::PFO* ppfo : *cont ) {
    if (!PVMatchedAcc.isAvailable(*ppfo)){
      ATH_MSG_ERROR("Not known if PFO is matched to primary vertex.  Run CorrectPFOTool before ChargedHadronSubtractionTool");
      return StatusCode::FAILURE;
    }

    if (ppfo->pt()<=0) continue;

    float charge = ppfo->charge();

    if(charge == 0)  neutralAndForwardVector.push_back( fastjet::PseudoJet( ppfo->p4() ));
		
    bool matchedToPrimaryVertex=PVMatchedAcc(*ppfo);

    if(charge != 0){
      if(matchedToPrimaryVertex) chargedHSVector.push_back( fastjet::PseudoJet( ppfo->p4() ));
      else chargedPUVector.push_back( fastjet::PseudoJet( ppfo->p4() ));
    }
  }	

  Puppi *m_Puppi = new Puppi(m_R0, m_Rmin, m_exponent, m_scaleFactorA, m_scaleFactorB, m_etaForward, m_forwardSF);
  m_Puppi->SetParticles(chargedHSVector, chargedPUVector, neutralAndForwardVector, 1);

  for ( xAOD::PFO* ppfo : *cont ) {
    fastjet::PseudoJet pj(ppfo->p4());

    double weight = m_Puppi->getWeight(pj);
    double alpha = m_Puppi->getAlpha(pj);

    if (ppfo->charge() == 0 && m_applyWeight) ppfo->setP4(weight*ppfo->p4());
    alphaAcc(*ppfo) = alpha;
    weightAcc(*ppfo) = weight;
  }

  ATH_MSG_INFO("Leaving execute");
  return StatusCode::SUCCESS;
}