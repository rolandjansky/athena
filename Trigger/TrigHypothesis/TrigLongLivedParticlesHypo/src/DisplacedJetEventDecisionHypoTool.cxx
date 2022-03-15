
/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
	int c3_total_jets = -1;
	for(auto dec: decs){
		if(dec.previousDecisionIDs.count(m_decisionId.numeric()) == 0){
			//this jet has not passed this chain
			//cannot consider in this hypo tool
			continue;
		}

		jets_with_previous.push_back(dec.jet);
		jet_decisions.push_back(dec.outDecision);
		jet_info[dec.jet] = dec.info;

		c3_total_jets = dec.counts->getDetail<int>("c3_total_jets_"+m_cutname);
	}

	bool pass_single_jet = false;

	if(m_c3_total_jets > 0){
		//this cut is to be applied
		if(c3_total_jets == m_c3_total_jets){
			//all jets must pass the c3 mode
			for(auto j: jets_with_previous){
				auto i = jet_info[j];

				if(!i->hasDetail<int>("c3_pass_"+m_cutname)){
					pass_single_jet = false;
					break;
				}

				if(i->getDetail<int>("c3_pass_"+m_cutname) == 1){
					pass_single_jet = true;
				}else{
					pass_single_jet = false;
					break;
				}
			}
		}
	}

	int n_h_jets = 0;
	int n_l_jets = 0;

	for(auto j: jets_with_previous){
		if(!jet_info[j]->hasDetail<int>("djtrig_jet_class_"+m_cutname)){
			ATH_MSG_WARNING("have jet decision for cut set "+m_cutname+" but cannot find jet class in the info container");
			continue;
		}

		int jet_class = jet_info[j]->getDetail<int>("djtrig_jet_class_"+m_cutname);

		ATH_MSG_DEBUG("djtrig event decision has jet pT = "<<j->pt()/Gaudi::Units::GeV<<" class= "<<jet_class);

		if(jet_class == 2){
			n_h_jets += 1;
		}

		if(jet_class == 1){
			n_l_jets += 1;
		}
	}

	if((n_h_jets >= m_min_h_jets && (n_l_jets + (n_h_jets - m_min_h_jets)) >= m_min_l_jets) || pass_single_jet){
		//accept the event
		for(auto d: jet_decisions){
			addDecisionID( m_decisionId.numeric(), d );
		}
	}

	return StatusCode::SUCCESS;
}
