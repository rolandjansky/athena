
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
	std::vector<TrigCompositeUtils::Decision*> jet_decisions;
	for(auto dec: decs){
		if(dec.previousDecisionIDs.count(m_decisionId.numeric()) == 0){
			//this jet has not passed this chain
			//cannot consider in this hypo tool
			continue;
		}

		jets_with_previous.push_back(dec.jet);
		jet_decisions.push_back(dec.outDecision);
	}

	int n_h_jets = 0;
	int n_l_jets = 0;

	static const xAOD::Jet::ConstAccessor<int> ad_jetclass("djtrig_jet_accept_type_"+m_cutname);

	for(auto j: jets_with_previous){
		if(!ad_jetclass.isAvailable(*j)) continue; //jet rank not available so ignore the jet
		int jet_class = ad_jetclass(*j);

		ATH_MSG_DEBUG("djtrig event decision has jet pT = "<<j->pt()/Gaudi::Units::GeV<<" class= "<<jet_class);

		if(jet_class == 2){
			n_h_jets += 1;
		}

		if(jet_class == 1){
			n_l_jets += 1;
		}
	}

	if(n_h_jets >= m_min_h_jets && n_l_jets >= m_min_l_jets){
		//accept the event
		for(auto d: jet_decisions){
			addDecisionID( m_decisionId.numeric(), d );
		}
	}

	return StatusCode::SUCCESS;
}
