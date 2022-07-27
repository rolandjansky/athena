#ifndef TRIGDJRANKCOMBOHYPO_H
#define TRIGDJRANKCOMBOHYPO_H

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/ComboHypoToolBase.h"


class DisplacedJetRankComboHypoTool: public ComboHypoToolBase{
	//tool to apply jet ranking 
public:
	DisplacedJetRankComboHypoTool(const std::string& type, const std::string& name, const IInterface* parent);
	virtual StatusCode initialize() override;
	//need to see all jets at once not a series of combinations
	virtual StatusCode decide(Combo::LegDecisionsMap& passingLegs, const EventContext& ctx) const override;

private:
	Gaudi::Property<unsigned int> m_rankcut{this, "max_jet_rank",{2}, "Maximum jet rank which will be processed, counts from 0"};
	Gaudi::Property<unsigned int> m_maxjets{this, "max_jets",{0}, "Maximum number of jets allowed in the event"};
};

#endif