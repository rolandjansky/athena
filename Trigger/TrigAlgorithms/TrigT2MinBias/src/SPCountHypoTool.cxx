
/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaMonitoring/Monitored.h"

#include "SPCountHypoTool.h"

using namespace TrigCompositeUtils;
SPCountHypoTool::SPCountHypoTool( const std::string& type,const std::string& name,const IInterface* parent )
: AthAlgTool( type, name, parent ),
m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode SPCountHypoTool::initialize()  {
	return StatusCode::SUCCESS;
}


bool SPCountHypoTool::applyCut(const Gaudi::Property<int> &threshold, const xAOD::TrigComposite* composit) const{
	if (threshold==-1) return m_logicAnd;//If the logic is And, it should return true and otherwise it should return false.
	ATH_MSG_DEBUG ("count for = "<<threshold.name() << "=" <<(composit->getDetail<int>(threshold.name()) > threshold ));
	return ( composit->getDetail<int>(threshold.name()) > threshold );
}

StatusCode SPCountHypoTool::decide(  SPCountsInfo& spinfo )  const {
	if ( spinfo.previousDecisionIDs.count( m_decisionId.numeric() ) == 0 ) {
		ATH_MSG_DEBUG("Already rejected");
		return StatusCode::SUCCESS;
	}
	std::vector<bool> decisionCuts({
		applyCut(m_totNumPixSP,spinfo.counts),
		applyCut(m_totNumPixCL_1,spinfo.counts) ,
		applyCut(m_totNumPixCL_2,spinfo.counts),
		applyCut(m_totNumPixCLmin3,spinfo.counts),
		applyCut(m_pixClBarrel,spinfo.counts),
		applyCut(m_pixClEndcapA,spinfo.counts),
		applyCut(m_pixClEndcapC,spinfo.counts),
		applyCut(m_totNumSctSP,spinfo.counts),
		applyCut(m_SctSpBarrel,spinfo.counts),
		applyCut(m_SctSpEndcapA,spinfo.counts),
		applyCut(m_SctSpEndcapC,spinfo.counts) });

		if(m_logicAnd && !std::all_of(decisionCuts.begin(),decisionCuts.end(),[](bool k ){return k; }) ){
			return StatusCode::SUCCESS;
		}else if(m_logicAnd==false && !std::any_of(decisionCuts.begin(),decisionCuts.end(),[](bool k){return k;}) ){
			return StatusCode::SUCCESS;
		}
		else{
			addDecisionID( m_decisionId.numeric(), spinfo.decision );
			ATH_MSG_DEBUG ("EVENT ACCOUNTED SUCCESSFULLY!!");
		}
		return StatusCode::SUCCESS;
	}
