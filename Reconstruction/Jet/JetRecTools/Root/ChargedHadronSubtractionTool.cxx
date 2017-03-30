#include <vector>

#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/Selector.hh"

#include "JetRecTools/ChargedHadronSubtractionTool.h"
#include "fastjet/contrib/SoftKiller.hh"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowAuxContainer.h"

using namespace std;

ChargedHadronSubtractionTool::ChargedHadronSubtractionTool(const std::string& name) : JetConstituentModifierBase(name)
{
#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif

}

StatusCode ChargedHadronSubtractionTool::process(xAOD::IParticleContainer* cont) const {
  xAOD::PFOContainer* pfoCont = dynamic_cast<xAOD::PFOContainer*> (cont);
  if(pfoCont) return process(pfoCont);
  else{
    ATH_MSG_ERROR("Unable to dynamic cast IParticleContainer to PFOContainer");
    return StatusCode::FAILURE;
  }
}

StatusCode ChargedHadronSubtractionTool::process(xAOD::PFOContainer* cont) const {

  SG::AuxElement::Accessor<bool> PVMatchedAcc("matchedToPV");
  for ( xAOD::PFO* ppfo : *cont ) {
    if(ppfo->charge() == 0) continue;

    if (!PVMatchedAcc.isAvailable(*ppfo)){
      ATH_MSG_ERROR("Not known if PFO is matched to primary vertex.  Run CorrectPFOTool before ChargedHadronSubtractionTool");
      return StatusCode::FAILURE;
    }
	
    if(!PVMatchedAcc(*ppfo))
      (ppfo)->setP4(0,0,0,0);
  }

  return StatusCode::SUCCESS;
}
