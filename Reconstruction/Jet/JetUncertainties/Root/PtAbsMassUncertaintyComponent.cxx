/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/PtAbsMassUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

PtAbsMassUncertaintyComponent::PtAbsMassUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_massDef(CompMassDef::UNKNOWN)
    , m_truthLabelName("")
    , m_truthLabels()
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PtAbsMassUncertaintyComponent::PtAbsMassUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_massDef(component.massDef)
    , m_truthLabelName(component.LargeRJetTruthLabelName)
    , m_truthLabels(component.LargeRJetTruthLabels)
{
    ATH_MSG_DEBUG(Form("Creating PtAbsMassUncertaintyComponent named %s",m_uncHistName.Data()));
}

PtAbsMassUncertaintyComponent::PtAbsMassUncertaintyComponent(const PtAbsMassUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_massDef(toCopy.m_massDef)
    , m_truthLabelName(toCopy.m_truthLabelName)
    , m_truthLabels(toCopy.m_truthLabels)
{
    ATH_MSG_DEBUG(Form("Creating copy of PtAbsMassUncertaintyComponent named %s",m_uncHistName.Data()));
}

PtAbsMassUncertaintyComponent* PtAbsMassUncertaintyComponent::clone() const
{
    return new PtAbsMassUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool PtAbsMassUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,getAbsMass(jet,m_massDef)*m_energyScale));
}

double PtAbsMassUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    // Check if we need to do anything at all - this uncertainty may be zero based on the jet truth label
    // Truth labels are usually not expected for this component, so check if we need to do this
    if (m_truthLabels.size())
    {
        // Truth labels are specified, so we need to check if this jet is labelled appropriately or not
        const SG::AuxElement::ConstAccessor<int> accTruthLabel(m_truthLabelName);
        if (!accTruthLabel.isAvailable(jet) || accTruthLabel(jet) == LargeRJetTruthLabel::UNKNOWN)
        {
            ATH_MSG_ERROR("Unable to retrieve the LargeRJetTruthLabel: " << m_truthLabelName << " from the jet.  Please use JetTruthLabelingTool before calling this function.");
            return JESUNC_ERROR_CODE;
        }
        const LargeRJetTruthLabel::TypeEnum jetTruthLabel = LargeRJetTruthLabel::intToEnum(accTruthLabel(jet));
    
        // We now have the truth jet label, check if it matches one of the label(s) assigned to this component
        // The uncertainty is only applied to same-labelled jets (zero otherwise)
        bool labelApplies = false;
        for (const LargeRJetTruthLabel::TypeEnum aLabel : m_truthLabels)
        {
            if (aLabel == jetTruthLabel)
            {
                labelApplies = true;
                break;
            }
        }
        // If the jet is not of the relevant label, then there is no uncertainty for this jet
        if (!labelApplies)
            return 0;
    }

    return m_uncHist->getValue(jet.pt()*m_energyScale,getAbsMass(jet,m_massDef)*m_energyScale);
}

} // end jet namespace


