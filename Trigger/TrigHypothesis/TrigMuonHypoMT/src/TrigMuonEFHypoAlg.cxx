/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFHypoAlg.h"
#include "AthViews/ViewHelper.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuonEFHypoAlg::TrigMuonEFHypoAlg( const std::string& name,
						  ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator )
{

} 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFHypoAlg::initialize()
{
  ATH_CHECK(m_hypoTools.retrieve());

  renounce(m_muonKey);
  ATH_CHECK(m_muonKey.initialize());

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFHypoAlg::execute( const EventContext& context ) const
{
  ATH_MSG_DEBUG("StatusCode TrigMuonEFHypoAlg::execute start");

  // common for all hypos, to move in the base class
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();
  // end of common
  
  std::vector<TrigMuonEFHypoTool::MuonEFInfo> toolInput;
  size_t counter = 0;  // view counter

  // loop over previous decisions
  for ( const auto previousDecision: *previousDecisionsHandle ) {
     // get RoIs
    auto roiInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString() );
    auto roiEL = roiInfo.link;
    ATH_CHECK( roiEL.isValid() );
    const TrigRoiDescriptor* roi = *roiEL;

    // get View
    auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
    ATH_CHECK( viewEL.isValid() );

    // get muons
    auto muonHandle = ViewHelper::makeHandle( *viewEL, m_muonKey, context );
    ATH_CHECK( muonHandle.isValid() );
    ATH_MSG_DEBUG( "Muinfo handle size: " << muonHandle->size() << " ..." );

    // It is posisble that no muons are found, in this case we go to the next decision
    if(muonHandle->size()==0) continue;

    //In cases where IM is using mergeByFeature, and since we can have multiple 
    //muons per view, need to check that we are considering the corresponding muon 
    //from the previous decision. Relevant for combined muons only.

    const xAOD::Muon *muonPrev = nullptr;
    if(m_mapToPrevDec){
      auto prevMuInfo = TrigCompositeUtils::findLinks<xAOD::MuonContainer>(previousDecision, TrigCompositeUtils::featureString(), TrigDefs::lastFeatureOfType);
      ATH_CHECK(prevMuInfo.size()==1);
      auto prevMuLink = prevMuInfo.at(0).link;
      ATH_CHECK(prevMuLink.isValid());
      muonPrev = *prevMuLink;
    }

    //loop over muons (more than one muon can be found by EF algos)
    for(uint i=0; i<muonHandle->size(); i++){
      auto muonEL = ViewHelper::makeLink( *viewEL, muonHandle, i );
      ATH_CHECK( muonEL.isValid() );

      const xAOD::Muon* muon = *muonEL;

      //Map muons to the correct decisions from previous step
      bool matchedToDec = false;
      if(m_mapToPrevDec){
	//First try to check if the combined muon has an extrapolated track particle
	//and if so whether it matches the SA track from the previous decision's muon
	auto trk1 = muon->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
	auto trk2 = muonPrev->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
	if(trk1 && trk2){
	  if(trk1->p4()==trk2->p4()){
	    matchedToDec = true;
	  }
	}
	else{
	  //If no extrapolated track particle (combined muon reconstructed by InsdieOutAlg)
	  //do dR matching. dR<0.05 should be sufficient to get correct matching
	  double dr = muon->p4().DeltaR(muonPrev->p4());
	  if(dr<0.05) matchedToDec = true;
	}
      }
      else matchedToDec = true; 

      //skip muons not corresponding to previous decisions
      if(!matchedToDec) continue;

      // create new decisions
      auto newd = newDecisionIn( decisions );
      
      // pussh_back to toolInput
      toolInput.emplace_back( newd, roi, muon, previousDecision );

      newd -> setObjectLink( featureString(), muonEL );
      TrigCompositeUtils::linkToPrevious( newd, previousDecision, context );

      ATH_MSG_DEBUG("REGTEST: " << m_muonKey.key() << " pT = " << (*muonEL)->pt() << " GeV");
      ATH_MSG_DEBUG("REGTEST: " << m_muonKey.key() << " eta/phi = " << (*muonEL)->eta() << "/" << (*muonEL)->phi());
      ATH_MSG_DEBUG("REGTEST:  RoI  = eta/phi = " << (*roiEL)->eta() << "/" << (*roiEL)->phi());
      ATH_MSG_DEBUG("Added view, roi, feature, previous decision to new decision "<<counter <<" for view "<<(*viewEL)->name()  );

      //found muon partnered to previous decision, so don't need to consider the other muons
      if(m_mapToPrevDec) break;
    }
    counter++;
  }

  ATH_MSG_DEBUG("Found "<<toolInput.size()<<" inputs to tools");

  // to TrigMuonEFHypoTool
  StatusCode sc = StatusCode::SUCCESS;
  for ( auto& tool: m_hypoTools ) {
    ATH_MSG_DEBUG("Go to " << tool );
    sc = tool->decide(toolInput);
    if (!sc.isSuccess()) {
      ATH_MSG_ERROR("MuonHypoTool is failed");
      return StatusCode::FAILURE;
    }
  } // End of tool algorithms */	

  ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

  ATH_MSG_DEBUG("StatusCode TrigMuonEFHypoAlg::execute success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

