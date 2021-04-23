#include <algorithm>
#include "TrigEgammaTLAPhotonHypoAlgMT.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
//#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

using namespace TrigCompositeUtils;

using xAOD::PhotonContainer;


TrigEgammaTLAPhotonHypoAlgMT::TrigEgammaTLAPhotonHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator ) :
  :: HypoBase (name, pSvcLocator) {}


StatusCode TrigEgammaTLAPhotonHypoAlgMT::initialize() {

  ATH_CHECK( m_hypoTools.retrieve() );
  ATH_CHECK( m_TLAPhotonsKey.initialize() );
  return StatusCode::SUCCESS;

}

StatusCode TrigEgammaTLAPhotonHypoAlgMT::execute( const EventContext& ctx) const
{
  ATH_MSG_DEBUG("Executing " << name() << "...");

  ATH_MSG_DEBUG("Retrieving 'HLT' decision\" " << decisionInput().key() << "\"");


  // retrieves the HLT decision container for the TLA photons
  auto previousDecisionHandle = SG::makeHandle(decisionInput(), ctx);
  ATH_CHECK(previousDecisionHandle.isValid());
  const DecisionContainer* prevDecisions = previousDecisionHandle.get();

  // get a pointer to the TLA Photons
  ATH_MSG_DEBUG("Retrieving photons from the TLA container \"" <<m_TLAPhotonsKey << "\" ");
  auto TLAPhotonsHandle = SG::makeHandle(m_TLAPhotonsKey, ctx);
  ATH_CHECK(TLAPhotonsHandle.isValid());
  const PhotonContainer* TLAPhotons = TLAPhotonsHandle.get();

  // check if we have at least one decision associated to the TLA Photon Container (the decision should be from the getFastPhoton alg?)
  bool atLeastOneDecision = false;

  // creates (via SG handle) a DecisionContainer for the output decision
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), ctx);
  DecisionContainer* outputDecisions = outputHandle.ptr();

  // the HypoTool needs a pair of <object*, decision*> as input
  std::vector<std::pair<const xAOD::Photon*, Decision*> > photonHypoInputs;

  // match the TLA photon with the decision from the input container that pertains to the SAME photon (this is necessary because the indices are not necessarily the same in both containers)
  for (const xAOD::Photon* TLAPhoton : *TLAPhotons)
  {
    bool matchedWithPrevDecision = false;
    const Decision* previousDecision = nullptr;
    for (const Decision* currDecision : *prevDecisions)
    {
      // retrieve the photon associated to currDecision via Element Link
      TrigCompositeUtils::LinkInfo< xAOD::PhotonContainer > myFeature = TrigCompositeUtils::findLink< xAOD::PhotonContainer >( currDecision, TrigCompositeUtils::featureString());
      ATH_CHECK(myFeature.isValid());
      const xAOD::Photon* testPhoton = *(myFeature.link);

      // verify match between TLAPhoton and testPhoton via DR
      if (testPhoton->p4().DeltaR(TLAPhoton->p4()) < 0.1)
      {
        previousDecision = currDecision;
        matchedWithPrevDecision = true;
        atLeastOneDecision = true;

        // now create a new Decision object, have it be linked to the current TLAPhoton, then create the pair with photon,decision
        // to feed the HypoTool
        Decision* newDecision = TrigCompositeUtils::newDecisionIn(outputDecisions, previousDecision, hypoAlgNodeName(), ctx);
        // create the link
        ElementLink<xAOD::PhotonContainer> photonLink = ElementLink<xAOD::PhotonContainer>(*TLAPhotons, TLAPhoton->index());
        ATH_CHECK(photonLink.isValid());
        newDecision->setObjectLink<xAOD::PhotonContainer>(featureString(), photonLink);

        photonHypoInputs.push_back( std::make_pair(TLAPhoton, newDecision) );
      } // TLAphoton/decision-linked-photon matching loop

      if ( matchedWithPrevDecision ) break;

    }// loops over decision in inputContainer

  }// TLAPhotons loop

  if (!atLeastOneDecision)
  {
    ATH_MSG_ERROR("Unable to associate a previous decision to any of the photons in the input TLA Photon container.");
    return StatusCode::FAILURE;
  }

  for (const auto& tool : m_hypoTools)
  {
    ATH_MSG_DEBUG("Now computing decision for HypoTool: " << tool->name());
    ATH_CHECK(tool->decide(photonHypoInputs));
  }

  ATH_CHECK( hypoBaseOutputProcessing(outputHandle ));


  return StatusCode::SUCCESS;
}
