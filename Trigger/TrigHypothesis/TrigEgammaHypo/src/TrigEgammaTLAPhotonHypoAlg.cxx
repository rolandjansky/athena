#include <algorithm>
#include "TrigEgammaTLAPhotonHypoAlg.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthViews/ViewHelper.h"
//#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

using namespace TrigCompositeUtils;

using xAOD::PhotonContainer;


TrigEgammaTLAPhotonHypoAlg::TrigEgammaTLAPhotonHypoAlg( const std::string& name, ISvcLocator* pSvcLocator ) :
  :: HypoBase (name, pSvcLocator) {}


StatusCode TrigEgammaTLAPhotonHypoAlg::initialize() {

  ATH_CHECK( m_hypoTools.retrieve() );
  ATH_CHECK( m_TLAPhotonsKey.initialize() );
  ATH_CHECK( m_OriginalPhotonsKey.initialize());
  
  // ATTEMPT TO FOLLOW EXAMPLE OF https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/Trigger/TrigHypothesis/TrigEgammaHypo/src/TrigEgammaPrecisionCaloHypoAlg.cxx#0013
  renounce(m_TLAPhotonsKey); // TLA Photons are made in views, so they are not in the EvtStore. Hide them (?)
  renounce(m_OriginalPhotonsKey);

  ATH_MSG_DEBUG("Initializing TrigEgammaTLAPhotonHypoAlg");
  return StatusCode::SUCCESS;

}

StatusCode TrigEgammaTLAPhotonHypoAlg::execute( const EventContext& ctx) const
{
  ATH_MSG_DEBUG("Executing " << name() << "...");

  ATH_MSG_DEBUG("Retrieving 'HLT' decision\" " << decisionInput().key() << "\"");


  // retrieves the HLT decision container for the TLA photons
  auto previousDecisionHandle = SG::makeHandle(decisionInput(), ctx);
  ATH_CHECK(previousDecisionHandle.isValid());
  const DecisionContainer* prevDecisions = previousDecisionHandle.get();

  // get a pointer to the TLA Photons
  ATH_MSG_DEBUG("Retrieving photons from the TLA container \"" <<m_TLAPhotonsKey << "\" ");
  
  
  

// check if we have at least one decision associated to the TLA Photon Container (the decision should be from the getFastPhoton alg?)
bool atLeastOneDecision = false;

// creates (via SG handle) a DecisionContainer for the output decision
SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), ctx);
DecisionContainer* outputDecisions = outputHandle.ptr();

// the HypoTool needs a pair of <object*, decision*> as input
std::vector<std::pair<const xAOD::Photon*, Decision*> > photonHypoInputs;

// loops over previous decisions
for (const Decision* currDecision : *prevDecisions)
{

  // retrieve a link to the EventView associated with this photon
    auto roiELInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( currDecision, TrigCompositeUtils::roiString() );
    ATH_CHECK( roiELInfo.isValid() );
    const auto viewEL = currDecision->objectLink<ViewContainer>( TrigCompositeUtils::viewString() );
    ATH_CHECK( viewEL.isValid() );

    // retrieves normal photons

    auto hltPhotonsCollectionHandle = ViewHelper::makeHandle(*viewEL, m_OriginalPhotonsKey, ctx);
    ATH_CHECK(hltPhotonsCollectionHandle.isValid());

    // retrieves the TLA Photon Handle (and later container) associated to the view that's being processed
    auto tlaPhotonsHandle = ViewHelper::makeHandle( *(viewEL), m_TLAPhotonsKey, ctx);
    ATH_CHECK(tlaPhotonsHandle.isValid());
    const PhotonContainer* TLAPhotons = tlaPhotonsHandle.get();
    
    for (const xAOD::Photon* currPhoton : *hltPhotonsCollectionHandle)
    {
      

      ATH_MSG_DEBUG("Original Photon Printout: " << currPhoton->p4().Pt() << " " << currPhoton->p4().Eta() << " " << currPhoton->p4().Phi() );

      // now loop over the TLAPhotons container and look for a photon matching the photon associated to the current decision
      for (const xAOD::Photon* TLAPhoton : *TLAPhotons)
      {
        
        ATH_MSG_DEBUG("TLA Photon Printout: " << TLAPhoton->p4().Pt() << " " << TLAPhoton->p4().Eta() << " " << TLAPhoton->p4().Phi() );
        
        // verify match between TLAPhoton and currPhoton
        if (TLAPhoton->p4().Pt() == currPhoton->p4().Pt() and TLAPhoton->p4().Eta() == currPhoton->p4().Eta()
              and TLAPhoton->p4().Phi() == currPhoton->p4().Phi()) // they are the same photon 
        {

          ATH_MSG_DEBUG("Matched a decision to a TLA Photon!");

          matchedWithPrevDecision = true; 
          atLeastOneDecision = true;

          // now create a new Decision object, have it be linked to the current TLAPhoton, then create the pair with photon,decision
          // to feed the HypoTool
          Decision* newDecision = TrigCompositeUtils::newDecisionIn(outputDecisions, currDecision, hypoAlgNodeName(), ctx);
          
          // create the link
          ElementLink<xAOD::PhotonContainer> photonLink = ElementLink<xAOD::PhotonContainer>(*TLAPhotons, TLAPhoton->index());
          ATH_CHECK(photonLink.isValid());
          newDecision->setObjectLink<xAOD::PhotonContainer>(featureString(), photonLink);

          photonHypoInputs.push_back( std::make_pair(TLAPhoton, newDecision) );

          break; // the Original Photon has been matched with a TLA Photon, no need to continue the TLA loop
        }
      }
    }
  if (atLeastOneDecision) break; // found a positive decision, no need to continue the Decision  loop
}




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
