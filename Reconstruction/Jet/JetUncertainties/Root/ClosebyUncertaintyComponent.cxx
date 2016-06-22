/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/ClosebyUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

ClosebyUncertaintyComponent::ClosebyUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

ClosebyUncertaintyComponent::ClosebyUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
{
    ATH_MSG_DEBUG(Form("Creating ClosebyUncertaintyComponent named %s",m_uncHistName.Data()));
}

ClosebyUncertaintyComponent::ClosebyUncertaintyComponent(const ClosebyUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
{
    ATH_MSG_DEBUG(Form("Creating copy of ClosebyUncertaintyComponent named %s",m_uncHistName.Data()));
}

ClosebyUncertaintyComponent* ClosebyUncertaintyComponent::clone() const
{
    return new ClosebyUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool ClosebyUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,getMinDR(jet)));
}

double ClosebyUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return m_uncHist->getValue(jet.pt()*m_energyScale,getMinDR(jet));
}

float ClosebyUncertaintyComponent::getMinDR(const xAOD::Jet& jet) const
{
    static SG::AuxElement::Accessor<float> accMinDR("MinDR");

    if (!accMinDR.isAvailable(jet))
    {
        ATH_MSG_ERROR("Failed to retrieve MinDR attribute from the jet");
        return JESUNC_ERROR_CODE;
    }
    return accMinDR(jet);
}

} // end jet namespace

