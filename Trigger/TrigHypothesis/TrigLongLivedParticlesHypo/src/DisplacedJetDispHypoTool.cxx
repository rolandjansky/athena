
/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "DisplacedJetDispHypoTool.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
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

	auto mon_ndisp = Monitored::Scalar<int>("ndisp", 0);
	auto mon_nprompt = Monitored::Scalar<int>("nprompt", 0);
	auto mon_frac_other = Monitored::Scalar<float>("frac_other", 0.0);
	
	std::vector<float> monvec_trk_d0sig;
	std::vector<float> monvec_trk_z0st;
	std::vector<float> monvec_trk_d0;

	auto mon_trk_d0sig = Monitored::Collection("trk_d0sig", monvec_trk_d0sig);
	auto mon_trk_z0st = Monitored::Collection("trk_z0st", monvec_trk_z0st);
	auto mon_trk_d0 = Monitored::Collection("trk_d0", monvec_trk_d0);

  	Monitored::Group mon_group(m_monTool, mon_ndisp, mon_nprompt, mon_frac_other, mon_trk_d0sig, mon_trk_z0st, mon_trk_d0);

	//apply the track counting for the displaced tracks
	int ndisp = info.counts->getDetail<int>("ndisp_"+m_cutname);
	int nother = info.counts->getDetail<int>("nother_"+m_cutname);
	int nprompt = info.counts->getDetail<int>("nprompt_"+m_cutname);


	for(auto trk: *(info.lrt_tracks)){
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
			double d0sig = 0.0;

			if(m_usebeamspot){
				d0sig = std::abs(xAOD::TrackingHelpers::d0significance(trk, info.beamspot.sigmaX(), info.beamspot.sigmaY(), info.beamspot.sigmaXY()));
			}else{
				d0sig = std::abs(xAOD::TrackingHelpers::d0significance(trk));
			}

			monvec_trk_d0sig.push_back(d0sig);

			//candidate displaced
			if(d0sig >= m_d0sigcut){
				track_class = 2; //displaced
				ATH_MSG_DEBUG("disp_trk in jet "<<info.jet->pt()/Gaudi::Units::GeV<<" accepted pT: "<<trk->pt()/Gaudi::Units::GeV<<" d0: "<<std::abs(trk->d0())<< " d0sig: "<<d0sig);
			}else{
				ATH_MSG_DEBUG("disp_trk in jet "<<info.jet->pt()/Gaudi::Units::GeV<<" dropped for d0sig pT: "<<trk->pt()/Gaudi::Units::GeV<<" d0: "<<std::abs(trk->d0())<< " d0sig: "<<d0sig);
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
	double nother_frac = 0.0;
	//ensure divide by zero does not occur
	if((nother + ndisp + nprompt) > 0){
		nother_frac = nother*1.0/(nother + ndisp + nprompt);
	}

	ANA_MSG_DEBUG("jet pT = "<<info.jet->pt()/Gaudi::Units::GeV<<" nPrompt = "<<nprompt<<" nDisp = "<<ndisp<<" nother = "<<nother);

	mon_ndisp = ndisp;
	mon_nprompt = nprompt;
	mon_frac_other = nother_frac;

	if(ndisp >= m_mindisp && nprompt <= m_maxprompt && nother_frac <= m_nother_frac){
		addDecisionID( m_decisionId.numeric(), info.decision );
	}

	return StatusCode::SUCCESS;
}
