/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/LogPtMassUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

LogPtMassUncertaintyComponent::LogPtMassUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_massDef(CompMassDef::UNKNOWN)
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

LogPtMassUncertaintyComponent::LogPtMassUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_massDef(component.massDef)
{
    ATH_MSG_DEBUG(Form("Creating LogPtMassUncertaintyComponent named %s",m_uncHistName.Data()));
}

LogPtMassUncertaintyComponent::LogPtMassUncertaintyComponent(const LogPtMassUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_massDef(toCopy.m_massDef)
{
    ATH_MSG_DEBUG(Form("Creating copy of LogPtMassUncertaintyComponent named %s",m_uncHistName.Data()));
}

LogPtMassUncertaintyComponent* LogPtMassUncertaintyComponent::clone() const
{
    return new LogPtMassUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool LogPtMassUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,log10(getMassOverPt(jet,m_massDef))));
}

double LogPtMassUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return m_uncHist->getValue(jet.pt()*m_energyScale,log10(getMassOverPt(jet,m_massDef)));
}

} // end jet namespace

