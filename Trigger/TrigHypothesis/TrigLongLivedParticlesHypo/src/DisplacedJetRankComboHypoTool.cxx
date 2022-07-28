#include "DisplacedJetRankComboHypoTool.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

DisplacedJetRankComboHypoTool::DisplacedJetRankComboHypoTool(const std::string& type, const std::string& name, const IInterface* parent): ComboHypoToolBase(type, name, parent) {}

StatusCode DisplacedJetRankComboHypoTool::initialize(){
	return StatusCode::SUCCESS;
}

StatusCode DisplacedJetRankComboHypoTool::decide(Combo::LegDecisionsMap& passingLegs, const EventContext& /*context*/) const{
	// if no combinations passed, then exit 
	if (passingLegs.size() == 0) {
		return StatusCode::SUCCESS;
	}

	std::vector<std::vector<Combo::LegDecision>> legDecisions;
  	ATH_CHECK(selectLegs(passingLegs, legDecisions));

	//get all the passing jets across all legs
	//also fill a map of jet -> decisions
	//derive ranks for these jets
	//get the list of decision objects which can go on

	//need to establish which leg ids come from the displaced jet trigger
	std::unordered_set<TrigCompositeUtils::DecisionID> dispj_leg_ids;

	ATH_MSG_DEBUG("Have "<<passingLegs.size()<<" passing legs in");

	for(auto it : passingLegs){
		if(it.second.size() == 0) continue; //empty set of decisions
		//only need to look at the first decision
		const TrigCompositeUtils::Decision* decision(*(it.second[0]));
		if(decision->hasObjectLink("djtrig_counts")){
			dispj_leg_ids.insert(it.first);
		}
	}

	ATH_MSG_DEBUG("Have "<<dispj_leg_ids.size()<<" dispjet legs");

	std::vector<const xAOD::Jet*> input_jets;
	std::map<const xAOD::Jet*, std::vector<Combo::LegDecision>> jet_decisions;

	//populate jet decision map first
	for(auto leg_decs : legDecisions){
		for(auto dec_pair : leg_decs){
			const TrigCompositeUtils::Decision* decision(*(dec_pair.second));

			//this leg is not a displaced jet leg
			//this tool should ignore it
			if(dispj_leg_ids.count(dec_pair.first) == 0) continue;

			//find the jet feature
			std::vector<TrigCompositeUtils::LinkInfo<xAOD::JetContainer>> jet_feature_links = TrigCompositeUtils::findLinks<xAOD::JetContainer>(decision, TrigCompositeUtils::featureString(), TrigDefs::lastFeatureOfType);
			if(jet_feature_links.size() == 0) continue; //verify that we get a jet
		    ATH_CHECK(jet_feature_links.size() == 1); //ensure we only have 1 link
		    const TrigCompositeUtils::LinkInfo<xAOD::JetContainer> jet_feature_link = jet_feature_links.at(0);
		    //verify if the feature link is valid
		    ATH_CHECK(jet_feature_link.isValid());
		    const xAOD::Jet* jet = *(jet_feature_link.link);

		    jet_decisions[jet].push_back(dec_pair);
		}
	}

	ATH_MSG_DEBUG("Have "<<jet_decisions.size()<<" jets");


	//fill the input_jets vector
	//filled from the map key which ensures it only gets one entry per jet
	for(auto pair: jet_decisions){
		input_jets.push_back(pair.first);
	}

	if(input_jets.size() == 0){
		//reject all legs
		//this should not happen
		ATH_MSG_DEBUG("No input jets, rejecting all legs");
		eraseFromLegDecisionsMap(passingLegs);
		return StatusCode::SUCCESS;
	}
	
	//apply max jet cut to the number of input passing jets
	if(m_maxjets > 0 && input_jets.size() > m_maxjets){
		//max jets cut
		//event fails
		ATH_MSG_DEBUG("Reject event as it has "<<input_jets.size()<<" input passing jets but max jet cut is "<<m_maxjets);
		eraseFromLegDecisionsMap(passingLegs);
		return StatusCode::SUCCESS;
	}


	//rank jets by pt
	std::sort(input_jets.begin(), input_jets.end(), [](const xAOD::Jet* a, const xAOD::Jet* b) -> bool
		{ 
	  		return a->pt() > b->pt();
		}
	);

	//map of legid -> vector of passing decisions
	Combo::LegDecisionsMap accepted_decisions_by_leg;

	unsigned int n_jets_passed = 0;
	//make a list of all decisions which are associated with passed jets
	for(size_t i = 0; i<input_jets.size(); i++){
		if(i > m_rankcut) continue; //skip jet as it is outside the rank cut
		n_jets_passed += 1;

		//get all the decisions pairs [legId, decision] for this jet
		for(auto leg_dec_pair: jet_decisions.at(input_jets.at(i))){
			accepted_decisions_by_leg[leg_dec_pair.first].push_back(leg_dec_pair.second);
			ATH_MSG_DEBUG("Accepting decision for jet "<<input_jets.at(i)->pt()/1000.0);
		}
	}


	if(n_jets_passed < input_jets.size()){
		//check that the number of jets has changed
		//this is an optimisation to avoid a needless loop when all jets pass
		//update the legs to only pass the decisions which are associated with passing jets
		for (auto& legIt : passingLegs) {
			if(dispj_leg_ids.count(legIt.first) == 0) continue; //do not do anything with none dispj legs

			auto accepted_decisions_on_leg = accepted_decisions_by_leg.find(legIt.first);

			if(accepted_decisions_on_leg != accepted_decisions_by_leg.end()){
				//update the list of decisions
				legIt.second = accepted_decisions_on_leg->second;
				ATH_MSG_DEBUG("Updating leg "<<legIt.first<<" with "<<accepted_decisions_on_leg->second.size()<<" decisions");
			}else{
				//no jets pass this leg and the pt rank cut
				//empty the leg
				ATH_MSG_DEBUG("Updating leg "<<legIt.first<<"; clearing decisions");
				legIt.second.clear();
			}
		}
	}

	//apply the multiplicity rules
	auto lm = legMultiplicity();
	for(size_t i=0; i<lm.size(); i++){
		auto legId = legDecisionId(i);

		if(passingLegs[legId].size() < (size_t)(lm.at(i))){
			//leg has not got enough passes
			//whole event fails as a leg does not pass
			ATH_MSG_DEBUG("Leg "<<legId<<" fails mulitplicity check, rejecting event");
			eraseFromLegDecisionsMap(passingLegs);
			return StatusCode::SUCCESS;
		}
	}

	return StatusCode::SUCCESS;
}
