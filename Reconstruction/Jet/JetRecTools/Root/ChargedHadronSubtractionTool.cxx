#include "JetRecTools/ChargedHadronSubtractionTool.h"

using namespace std;

ChargedHadronSubtractionTool::ChargedHadronSubtractionTool(const std::string& name) : JetConstituentModifierBase(name)
{
#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif

}

StatusCode ChargedHadronSubtractionTool::initialize() {
  if(m_inputType!=xAOD::Type::ParticleFlow) {
    ATH_MSG_ERROR("ChargedHadronSubtractionTool requires PFO inputs. It cannot operate on objects of type "
		  << m_inputType);
  return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode ChargedHadronSubtractionTool::process_impl(xAOD::IParticleContainer* cont) const {
  // Type-checking happens in the JetConstituentModifierBase class
  // so it is safe just to static_cast
  xAOD::PFOContainer* pfoCont = static_cast<xAOD::PFOContainer*> (cont);
  return removePileupChargedHadrons(*pfoCont);
}

StatusCode ChargedHadronSubtractionTool::removePileupChargedHadrons(xAOD::PFOContainer& cont) const {

  SG::AuxElement::Accessor<bool> PVMatchedAcc("matchedToPV");
  for ( xAOD::PFO* ppfo : cont ) {
    if(fabs(ppfo->charge()) < 1e-9) continue;

    if (!PVMatchedAcc.isAvailable(*ppfo)){
      ATH_MSG_ERROR("Not known if PFO is matched to primary vertex.  Run CorrectPFOTool before ChargedHadronSubtractionTool");
      return StatusCode::FAILURE;
    }
	
    if(!PVMatchedAcc(*ppfo))
      (ppfo)->setP4(0,0,0,0);
  }

  return StatusCode::SUCCESS;
}
