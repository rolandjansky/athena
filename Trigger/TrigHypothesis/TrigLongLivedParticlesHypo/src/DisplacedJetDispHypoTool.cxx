
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

	auto mon_jet_class = Monitored::Scalar<int>("jet_class", 0);
	auto mon_ndisp = Monitored::Scalar<int>("ndisp", 0);
	auto mon_nprompt = Monitored::Scalar<int>("nprompt", 0);
	auto mon_frac_other = Monitored::Scalar<float>("frac_other", 0.0);
	auto mon_pass_jet_pt = Monitored::Scalar<float>("pass_jet_pt", -1.0);
  	Monitored::Group mon_group(m_monTool, mon_jet_class, mon_ndisp, mon_nprompt, mon_frac_other, mon_pass_jet_pt);

	//apply the track counting for the displaced tracks
	int ndisp = info.counts->getDetail<int>("ndisp_"+m_cutname);
	int nother = info.counts->getDetail<int>("nother_"+m_cutname);
	int nprompt = info.counts->getDetail<int>("nprompt_"+m_cutname);


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
			double d0sig = 0.0;

			if(m_usebeamspot){
				d0sig = std::abs(xAOD::TrackingHelpers::d0significance(trk, info.beamspot.sigmaX(), info.beamspot.sigmaY(), info.beamspot.sigmaXY()));
			}else{
				d0sig = std::abs(xAOD::TrackingHelpers::d0significance(trk));
			}

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

	unsigned int jet_class = 0;

	ANA_MSG_DEBUG("jet pT = "<<info.jet->pt()/Gaudi::Units::GeV<<" nPrompt = "<<nprompt<<" nDisp = "<<ndisp<<" nother = "<<nother);

	mon_ndisp = ndisp;
	mon_nprompt = nprompt;
	mon_frac_other = nother_frac;


	if(ndisp >= m_mindisp_c2 && nprompt <= m_maxprompt_c2 && nother_frac <= m_nother_frac_c2){
		jet_class = 2;
	}else{
		if(ndisp >= m_mindisp_c1 && nprompt <= m_maxprompt_c1 && nother_frac <= m_nother_frac_c1){
			jet_class = 1;
		}
	}

	if(jet_class > 0){
		addDecisionID( m_decisionId.numeric(), info.decision );
		mon_pass_jet_pt = info.jet->pt()/Gaudi::Units::GeV;
	}

	info.info->setDetail<int>("djtrig_jet_class_"+m_cutname, jet_class);

	mon_jet_class = jet_class;

	ANA_MSG_DEBUG("jet pT = "<<info.jet->pt()/Gaudi::Units::GeV<<" assigned class "<<jet_class);

	return StatusCode::SUCCESS;
}
