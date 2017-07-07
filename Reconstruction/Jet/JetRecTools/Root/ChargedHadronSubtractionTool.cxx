#include "JetRecTools/ChargedHadronSubtractionTool.h"

using namespace std;

ChargedHadronSubtractionTool::ChargedHadronSubtractionTool(const std::string& name) : JetConstituentModifierBase(name)
{

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

  const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
  for ( xAOD::PFO* ppfo : cont ) {
    if(fabs(ppfo->charge()) < 1e-9) continue;

    if (!PVMatchedAcc.isAvailable(*ppfo)){
      ATH_MSG_ERROR("Not known if PFO is matched to primary vertex.  Run CorrectPFOTool before ChargedHadronSubtractionTool");
      return StatusCode::FAILURE;
    }
	
    if(!PVMatchedAcc(*ppfo))
      // Set a -ve energy, because we actually want to differentiate
      // CHS-removed cPFOs from those with a 0 weight due to calo energy
      (ppfo)->setP4(-1e-6,0,0,0);
  }

  return StatusCode::SUCCESS;
}
