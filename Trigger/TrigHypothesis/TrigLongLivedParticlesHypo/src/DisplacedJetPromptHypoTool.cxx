
/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

#include "DisplacedJetPromptHypoTool.h"

using namespace TrigCompositeUtils;

DisplacedJetPromptHypoTool::DisplacedJetPromptHypoTool( const std::string& type,const std::string& name,const IInterface* parent )
: AthAlgTool( type, name, parent ),
m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode DisplacedJetPromptHypoTool::initialize()  {
	if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());
	return StatusCode::SUCCESS;
}

StatusCode DisplacedJetPromptHypoTool::decide(  Info& info )  const {
	//setup monitoring
	std::vector<unsigned int> mon_vec_nprompt;
	std::vector<unsigned int> mon_vec_nother;
	std::vector<float> mon_vec_pass_jet_pt;
	std::vector<unsigned int> mon_vec_pass_nprompt;
	std::vector<unsigned int> mon_vec_pass_nother;

	auto mon_pass_jet_pt = Monitored::Collection("pass_jet_pt", mon_vec_pass_jet_pt);
	auto mon_nprompt = Monitored::Collection("nprompt", mon_vec_nprompt);
	auto mon_nother = Monitored::Collection("nother", mon_vec_nother);
	auto mon_pass_nprompt = Monitored::Collection("pass_nprompt", mon_vec_pass_nprompt);
	auto mon_pass_nother = Monitored::Collection("pass_nother", mon_vec_pass_nother);
	Monitored::Group mon_group(m_monTool, mon_pass_jet_pt, mon_nprompt, mon_nother, mon_pass_nprompt, mon_pass_nother);


	if ( info.previousDecisionIDs.count( m_decisionId.numeric() ) == 0 ) {
		ATH_MSG_DEBUG("Already rejected");
		return StatusCode::SUCCESS;
	}

	//generate ranks
	std::vector<const xAOD::Jet*> input_jets;

	std::map<const xAOD::Jet*, int> map_nprompt;
	std::map<const xAOD::Jet*, int> map_ndisp;
	std::map<const xAOD::Jet*, int> map_nother;

	for(auto j: *(info.jets)){
		if(j->pt()/Gaudi::Units::GeV < m_min_jet_pt) continue; //reject low pt jets
		if(std::abs(j->eta()) > 2.4) continue; //reject jets outside the barrel region
		//get the number of prompt tracks in the jet
		int nprompt = 0;
		int nother = 0;
		int ndisp = 0;

		for(auto trk: info.jets_to_tracks[j]){
			if(trk->pt()/Gaudi::Units::GeV < m_min_trk_pt) continue;
			unsigned int track_class = 0;

			if(std::abs(trk->d0()) < m_d0cut){
				double dz0 = trk->z0() + trk->vz() - info.primary_vertex->z();
			 	double dz0st = std::abs(std::sin(trk->theta()) * dz0);

			  	if(dz0st <= m_z0stcut){
			  		track_class = 1; //prompt
			  	}
			}else{
				//candidate displaced
				double d0sig = xAOD::TrackingHelpers::d0significance(trk); //need to look into beamspot information in the trigger

				if(d0sig >= m_d0sigcut){
					track_class = 2; //displaced
				}
			}

			if(track_class == 1){
				nprompt += 1;
			}else if(track_class == 2){
				ndisp += 1;
			}else if(track_class == 0){
				nother += 1;
			}
		}

		mon_vec_nother.push_back(nother);
		mon_vec_nprompt.push_back(nprompt);

		if(nprompt > m_maxprompt){
		  //fail the jet
		  continue;
		}

		input_jets.push_back(j); //insert the jet for ranking
		//also attach the number of prompt & other tracks found
		//saves having to redetermine this later

		map_nprompt[j] = nprompt;
		map_ndisp[j] = ndisp;
		map_nother[j] = nother;
	}

	//rank jets
	std::sort(input_jets.begin(), input_jets.end(), [](const xAOD::Jet* a, const xAOD::Jet* b) -> bool
		{ 
	  		return a->pt() > b->pt();
		}
	);

	int n_ok_jets = 0;
	int n_c3_ok_jets = 0;

	for(size_t i = 0; i<input_jets.size(); i++){
		if(i < m_rankcut){
			n_ok_jets += 1;

			if(input_jets[i]->pt()/Gaudi::Units::GeV >= m_c3_min_jet_pt){
				n_c3_ok_jets += 1;
			}	
		}
	}


	//reject early if it cannot pass the trigger
	if(n_ok_jets < m_minnjets && n_c3_ok_jets != m_n_c3_jets){
		//reject the event
		ATH_MSG_DEBUG("Event failed to few jets passed rank");
		return StatusCode::SUCCESS;
	}

	
	for(size_t i = 0; i<input_jets.size(); i++){
		if(i < m_rankcut){
			//this jet is okay to proceed
			//get the decision object for this jet and pass it
			auto j = input_jets[i];
			
			auto d = info.jet_decisions[j];
			ATH_MSG_DEBUG("Passing Jet pT = "<<j->pt()/Gaudi::Units::GeV<<" rank= "<<i<<" dec_ptr="<<d);
			addDecisionID(m_decisionId.numeric(), d);

			//add info on this jet to the trig info composite container
			//then add a link on the decision object linking to this object
			auto ic = info.counts[j];
			ic->setDetail<int>("nprompt_"+m_cutname, map_nprompt[j]);
			ic->setDetail<int>("ndisp_"+m_cutname, map_ndisp[j]);
			ic->setDetail<int>("nother_"+m_cutname, map_nother[j]);
			ic->setDetail<int>("c3_total_jets_"+m_cutname, n_c3_ok_jets);

			mon_vec_pass_jet_pt.push_back(j->pt()/Gaudi::Units::GeV);
			mon_vec_pass_nprompt.push_back(map_nprompt[j]);
			mon_vec_pass_nother.push_back(map_nother[j]);

			//single jet mode
		}
	}

	return StatusCode::SUCCESS;
}
