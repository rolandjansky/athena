/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/LargeRTopologyUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

LargeRTopologyUncertaintyComponent::LargeRTopologyUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_absEta(false)
    , m_truthLabelName("")
    , m_truthLabels()
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

LargeRTopologyUncertaintyComponent::LargeRTopologyUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_absEta(CompParametrization::isAbsEta(component.parametrization))
    , m_truthLabelName(component.LargeRJetTruthLabelName)
    , m_truthLabels(component.LargeRJetTruthLabels)
{
    ATH_MSG_DEBUG(Form("Creating LargeRTopologyUncertaintyComponent named %s",m_uncHistName.Data()));
}

LargeRTopologyUncertaintyComponent::LargeRTopologyUncertaintyComponent(const LargeRTopologyUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_absEta(toCopy.m_absEta)
    , m_truthLabelName(toCopy.m_truthLabelName)
    , m_truthLabels(toCopy.m_truthLabels)
{
    ATH_MSG_DEBUG(Form("Creating copy of LargeRTopologyUncertaintyComponent named %s",m_uncHistName.Data()));
}

LargeRTopologyUncertaintyComponent* LargeRTopologyUncertaintyComponent::clone() const
{
    return new LargeRTopologyUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool LargeRTopologyUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta()));
}

double LargeRTopologyUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    // Retrieve the truth jet label from the jet
    static const SG::AuxElement::ConstAccessor<int> accTruthLabel(m_truthLabelName);
    if (!accTruthLabel.isAvailable(jet) || accTruthLabel(jet) == LargeRJetTruthLabel::UNKNOWN)
    {
        ATH_MSG_ERROR("Unable to retrieve the LargeRJetTruthLabel: " << m_truthLabelName << " from the jet.  Please use JetTruthLabelingTool before calling this function.");
        return JESUNC_ERROR_CODE;
    }
    const LargeRJetTruthLabel::TypeEnum jetTruthLabel = LargeRJetTruthLabel::intToEnum(accTruthLabel(jet));

    // We now have the truth jet label, check if it matches one of the label(s) assigned to this component
    // The uncertainty is only applied to same-labelled jets (zero otherwise)
    for (const LargeRJetTruthLabel::TypeEnum aLabel : m_truthLabels)
    {
        if (aLabel == jetTruthLabel)
            return m_uncHist->getValue(jet.pt()*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta());
    }
    return 0;
}

} // end jet namespace

