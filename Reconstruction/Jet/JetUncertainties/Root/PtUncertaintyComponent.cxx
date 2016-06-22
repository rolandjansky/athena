/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/PtUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

PtUncertaintyComponent::PtUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PtUncertaintyComponent::PtUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
{
    ATH_MSG_DEBUG(Form("Creating PtUncertaintyComponent named %s",m_uncHistName.Data()));
}

PtUncertaintyComponent::PtUncertaintyComponent(const PtUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
{
    ATH_MSG_DEBUG(Form("Creating copy of PtUncertaintyComponent named %s",m_uncHistName.Data()));
}

PtUncertaintyComponent* PtUncertaintyComponent::clone() const
{
    return new PtUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Uncertainty/validity retrieval methods      //
//                                              //
//////////////////////////////////////////////////

bool PtUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale));
}

double PtUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return m_uncHist->getValue(jet.pt()*m_energyScale);
}

} // end jet namespace

