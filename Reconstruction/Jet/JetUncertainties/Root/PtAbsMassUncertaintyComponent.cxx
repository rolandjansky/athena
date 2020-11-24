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
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PtAbsMassUncertaintyComponent::PtAbsMassUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_massDef(component.massDef)
{
    ATH_MSG_DEBUG(Form("Creating PtAbsMassUncertaintyComponent named %s",m_uncHistName.Data()));
}

PtAbsMassUncertaintyComponent::PtAbsMassUncertaintyComponent(const PtAbsMassUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_massDef(toCopy.m_massDef)
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
    return m_uncHist->getValue(jet.pt()*m_energyScale,getAbsMass(jet,m_massDef)*m_energyScale);
}

} // end jet namespace


