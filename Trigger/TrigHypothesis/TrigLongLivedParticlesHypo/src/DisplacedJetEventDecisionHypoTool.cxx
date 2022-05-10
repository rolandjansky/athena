
/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "DisplacedJetEventDecisionHypoTool.h"

using namespace TrigCompositeUtils;

DisplacedJetEventDecisionHypoTool::DisplacedJetEventDecisionHypoTool( const std::string& type,const std::string& name,const IInterface* parent )
: AthAlgTool( type, name, parent ),
m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode DisplacedJetEventDecisionHypoTool::initialize()  {
	return StatusCode::SUCCESS;
}

StatusCode DisplacedJetEventDecisionHypoTool::decide( std::vector<DecisionTuple>& decs ) const{
	//loop over each decision tuple to extract those decisions which are for this tool ie which are for the chain this tool represents
	std::vector<const xAOD::Jet*> jets_with_previous;
	std::map<const xAOD::Jet*, const xAOD::TrigComposite*> jet_info;
	std::vector<TrigCompositeUtils::Decision*> jet_decisions;

	auto mon_nc2 = Monitored::Scalar<int>("n_c2", 0);
	auto mon_nc1 = Monitored::Scalar<int>("n_c1", 0);
	auto mon_flags = Monitored::Scalar<int>("flags", 0);
  	Monitored::Group mon_group(m_monTool, mon_nc2, mon_nc1, mon_flags);

	for(auto dec: decs){
		if(dec.previousDecisionIDs.count(m_decisionId.numeric()) == 0){
			//this jet has not passed this chain
			//cannot consider in this hypo tool
			continue;
		}

		jets_with_previous.push_back(dec.jet);
		jet_decisions.push_back(dec.outDecision);
		jet_info[dec.jet] = dec.info;
	}

	int n_c2_jets = 0;
	int n_c1_jets = 0;

	for(auto j: jets_with_previous){
		if(!jet_info[j]->hasDetail<int>("djtrig_jet_class_"+m_cutname)){
			ATH_MSG_WARNING("have jet decision for cut set "+m_cutname+" but cannot find jet class in the info container");
			continue;
		}

		int jet_class = jet_info[j]->getDetail<int>("djtrig_jet_class_"+m_cutname);

		ATH_MSG_DEBUG("djtrig event decision has jet pT = "<<j->pt()/Gaudi::Units::GeV<<" class= "<<jet_class);

		if(jet_class == 2){
			n_c2_jets += 1;
		}

		if(jet_class == 1){
			n_c1_jets += 1;
		}
	}

	mon_nc2 = n_c2_jets;
	mon_nc1 = n_c1_jets;

	int flags = 0;

	if(n_c2_jets > 0){
		flags |= (1<<1);
	}

	if(n_c1_jets > 0){
		flags |= 1;
	}

	if(n_c2_jets >= (m_min_h_jets.value() + m_min_l_jets.value())){
		flags |= (1<<2);
	}

	mon_flags = flags;

	if((n_c2_jets >= m_min_h_jets && (n_c1_jets + (n_c2_jets - m_min_h_jets)) >= m_min_l_jets)){
		//accept the event
		for(auto d: jet_decisions){
			addDecisionID( m_decisionId.numeric(), d );
		}
	}

	return StatusCode::SUCCESS;
}
