
/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
	auto mon_nprompt = Monitored::Scalar("nprompt", 0);
	
	std::vector<float> monvec_trk_d0sig;
	std::vector<float> monvec_trk_z0st;
	std::vector<float> monvec_trk_d0;

	auto mon_trk_d0sig = Monitored::Collection("trk_d0sig", monvec_trk_d0sig);
	auto mon_trk_z0st = Monitored::Collection("trk_z0st", monvec_trk_z0st);
	auto mon_trk_d0 = Monitored::Collection("trk_d0", monvec_trk_d0);
	Monitored::Group mon_group(m_monTool, mon_nprompt, mon_trk_z0st, mon_trk_d0sig, mon_trk_d0);


	if ( info.previousDecisionIDs.count( m_decisionId.numeric() ) == 0 ) {
		ATH_MSG_DEBUG("Already rejected");
		return StatusCode::SUCCESS;
	}

	//apply jet cuts
	//jet pt cut
	if(info.jet->pt()/Gaudi::Units::GeV < m_min_jet_pt){
		return StatusCode::SUCCESS;
	}

	//reject jets outside the barrel region
	if(std::abs(info.jet->eta()) > 2.4){
		return StatusCode::SUCCESS;
	}

	//apply track counts
	int nprompt = 0;
	int nother = 0;
	int ndisp = 0;

	for(auto trk: info.tracks){
		if(trk->pt()/Gaudi::Units::GeV < m_min_trk_pt) continue;
		unsigned int track_class = 0;

		monvec_trk_d0.push_back(std::abs(trk->d0()));

		if(std::abs(trk->d0()) < m_d0cut){
			double dz0 = trk->z0() + trk->vz() - info.primary_vertex->z();
		 	double dz0st = std::abs(std::sin(trk->theta()) * dz0);

		 	monvec_trk_z0st.push_back(dz0st);

		  	if(dz0st <= m_z0stcut){
		  		track_class = 1; //prompt
		  	}
		}else{
			//candidate displaced
			double d0sig = 0.0;

			if(m_usebeamspot){
				d0sig = std::abs(xAOD::TrackingHelpers::d0significance(trk, info.beamspot.sigmaX(), info.beamspot.sigmaY(), info.beamspot.sigmaXY()));
			}else{
				d0sig = std::abs(xAOD::TrackingHelpers::d0significance(trk));
			}

			monvec_trk_d0sig.push_back(d0sig);

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

	mon_nprompt = nprompt;

	if(nprompt > m_maxprompt){
		//reject the jet
		return StatusCode::SUCCESS;
	}

	//store in counts container
	info.counts->setDetail<int>("nprompt_"+m_cutname, nprompt);
	info.counts->setDetail<int>("ndisp_"+m_cutname, ndisp);
	info.counts->setDetail<int>("nother_"+m_cutname, nother);

	ATH_MSG_DEBUG("Passing Jet pT = "<<info.jet->pt()/Gaudi::Units::GeV);
	addDecisionID(m_decisionId.numeric(), info.output_decision);

	return StatusCode::SUCCESS;
}
