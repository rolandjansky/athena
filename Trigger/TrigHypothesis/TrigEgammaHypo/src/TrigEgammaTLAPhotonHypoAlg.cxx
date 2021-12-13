#include <algorithm>
#include "TrigEgammaTLAPhotonHypoAlg.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthViews/ViewHelper.h"
//#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"

using namespace TrigCompositeUtils;

using xAOD::PhotonContainer;


TrigEgammaTLAPhotonHypoAlg::TrigEgammaTLAPhotonHypoAlg( const std::string& name, ISvcLocator* pSvcLocator ) :
  :: HypoBase (name, pSvcLocator) {}


StatusCode TrigEgammaTLAPhotonHypoAlg::initialize() {

  ATH_CHECK( m_hypoTools.retrieve() );
  ATH_CHECK( m_TLAPhotonsKey.initialize() );

  ATH_MSG_DEBUG("Initializing TrigEgammaTLAPhotonHypoAlg");
  return StatusCode::SUCCESS;

}

StatusCode TrigEgammaTLAPhotonHypoAlg::execute( const EventContext& ctx) const
{
  ATH_MSG_DEBUG("Executing " << name() << "...");

  ATH_MSG_DEBUG("Retrieving 'HLT' decision\" " << decisionInput().key() << "\"");


  // create handles for TLA photons
  SG::WriteHandle<PhotonContainer> h_TLAPhotons = SG::makeHandle(m_TLAPhotonsKey, ctx);
  //make the output photon container
  ATH_CHECK(h_TLAPhotons.record(std::make_unique<xAOD::PhotonContainer>(),
                              std::make_unique<xAOD::PhotonAuxContainer>()));


  // retrieves the HLT decision container for the TLA photons
  auto previousDecisionHandle = SG::makeHandle(decisionInput(), ctx);
  ATH_CHECK(previousDecisionHandle.isValid());
  

  // get a pointer to the TLA Photons
  ATH_MSG_DEBUG("Retrieving photons from the TLA container \"" <<m_TLAPhotonsKey << "\" ");
  
  
  // creates (via SG handle) a DecisionContainer for the output decision
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), ctx);
  DecisionContainer* outputDecisions = outputHandle.ptr();

  // loops over previous decisions
  int nDecision = 0;
  for (const auto previousDecision : *previousDecisionHandle)
    {

    // get photons from the decision
        const xAOD::Photon *photonPrev = nullptr;
        auto prevPhotons = TrigCompositeUtils::findLinks<xAOD::PhotonContainer>(previousDecision, TrigCompositeUtils::featureString(), TrigDefs::lastFeatureOfType);
        ATH_MSG_DEBUG("This decision has " << prevPhotons.size() << " photons");
        
        
        if (prevPhotons.size() != 1) {
          ATH_MSG_ERROR("Did not locate exactly one photon for this Decision Object, found " << prevPhotons.size());
          return StatusCode::FAILURE;
        }


        auto prevPhotonLink = prevPhotons.at(0).link;
        ATH_CHECK(prevPhotonLink.isValid());
        photonPrev = *prevPhotonLink;

        xAOD::Photon *copiedPhoton = new xAOD::Photon();            
        h_TLAPhotons->push_back(copiedPhoton);
        *copiedPhoton = *photonPrev;

        // now go on with the normal Hypo, linking new decision with previous one
        auto newDecision = newDecisionIn( outputDecisions, hypoAlgNodeName() );
        TrigCompositeUtils::linkToPrevious( newDecision, previousDecision, ctx );
        // do we need to re-link the feature?
        newDecision->setObjectLink(featureString(), ElementLink<xAOD::PhotonContainer>(*h_TLAPhotons, h_TLAPhotons->size() - 1, ctx)); 
        

        ATH_MSG_DEBUG("Copied photon with pT: " << copiedPhoton->pt() << " from decision " << nDecision);
        

    }



  for (const auto& tool : m_hypoTools)
  {
    ATH_MSG_DEBUG("Now computing decision for HypoTool: " << tool->name());
    ATH_CHECK(tool->decide(outputDecisions));
  }

  ATH_CHECK( hypoBaseOutputProcessing(outputHandle ));


  return StatusCode::SUCCESS;
}
