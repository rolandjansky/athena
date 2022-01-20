
/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "DisplacedJetDispHypoTool.h"

using namespace TrigCompositeUtils;

DisplacedJetDispHypoTool::DisplacedJetDispHypoTool( const std::string& type,const std::string& name,const IInterface* parent )
: AthAlgTool( type, name, parent ),
m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode DisplacedJetDispHypoTool::initialize()  {
	if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());
	return StatusCode::SUCCESS;
}

StatusCode DisplacedJetDispHypoTool::decide(  Info& info )  const {
	if ( info.previousDecisionIDs.count( m_decisionId.numeric() ) == 0 ) {
		ATH_MSG_DEBUG("Already rejected");
		return StatusCode::SUCCESS;
	}

	static const xAOD::Jet::ConstAccessor<int> ca_nprompt("djtrig_jet_nprompt_"+m_cutname);

	if(!ca_nprompt.isAvailable(*(info.jet))){
		//unranked jet
		return StatusCode::SUCCESS;
	}

	auto mon_jet_class = Monitored::Scalar<int>("jet_class", 0);
	auto mon_ndisp = Monitored::Scalar<int>("ndisp", 0);
	auto mon_nprompt = Monitored::Scalar<int>("nprompt", 0);
	auto mon_frac_other = Monitored::Scalar<float>("frac_other", 0.0);
	auto mon_pass_jet_pt = Monitored::Scalar<float>("pass_jet_pt", -1.0);
  	Monitored::Group mon_group(m_monTool, mon_jet_class, mon_ndisp, mon_nprompt, mon_frac_other, mon_pass_jet_pt);

	//apply the track counting for the displaced tracks
	int ndisp = info.jet->auxdecor<int>("djtrig_jet_ndisp_"+m_cutname);
	int nother = info.jet->auxdecor<int>("djtrig_jet_nother_prompt_"+m_cutname);
	int nprompt = ca_nprompt(*(info.jet));


	for(auto trk: *(info.lrt_tracks)){
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
			double d0sig = std::fabs(trk->d0())/std::sqrt(trk->definingParametersCovMatrixDiagVec().at(0)); //need to look into beamspot information in the trigger

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

	double nother_frac = nother*1.0/(nother + ndisp + nprompt);

	unsigned int jet_class = 0;

	ANA_MSG_DEBUG("jet pT = "<<info.jet->pt()/Gaudi::Units::GeV<<" nPrompt = "<<nprompt<<" nDisp = "<<ndisp<<" nother = "<<nother);

	mon_ndisp = ndisp;
	mon_nprompt = nprompt;
	mon_frac_other = nother_frac;


	if(ndisp >= m_mindisp_h && nprompt <= m_maxprompt_h && nother_frac <= m_nother_frac_h){
		jet_class = 2;
	}else{
		if(ndisp >= m_mindisp_l && nprompt <= m_maxprompt_l && nother_frac <= m_nother_frac_l){
			jet_class = 1;
		}
	}

	if(jet_class > 0){
		addDecisionID( m_decisionId.numeric(), info.decision );
		mon_pass_jet_pt = info.jet->pt()/Gaudi::Units::GeV;
	}

	mon_jet_class = jet_class;

	info.jet->auxdecor<int>("djtrig_jet_accept_type_"+m_cutname) = jet_class;
	ANA_MSG_DEBUG("jet pT = "<<info.jet->pt()/Gaudi::Units::GeV<<" assigned class "<<jet_class);

	return StatusCode::SUCCESS;
}
