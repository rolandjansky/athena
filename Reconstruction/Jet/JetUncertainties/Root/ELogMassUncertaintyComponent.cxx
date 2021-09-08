/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/ELogMassUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

ELogMassUncertaintyComponent::ELogMassUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_massDef(CompMassDef::UNKNOWN)
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

ELogMassUncertaintyComponent::ELogMassUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_massDef(component.massDef)
{
    ATH_MSG_DEBUG(Form("Creating ELogMassUncertaintyComponent named %s",m_uncHistName.Data()));
}

ELogMassUncertaintyComponent::ELogMassUncertaintyComponent(const ELogMassUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_massDef(toCopy.m_massDef)
{
    ATH_MSG_DEBUG(Form("Creating copy of ELogMassUncertaintyComponent named %s",m_uncHistName.Data()));
}

ELogMassUncertaintyComponent* ELogMassUncertaintyComponent::clone() const
{
    return new ELogMassUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool ELogMassUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.e()*m_energyScale,log(getMassOverE(jet,m_massDef))));
}

double ELogMassUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return m_uncHist->getValue(jet.e()*m_energyScale,log(getMassOverE(jet,m_massDef)));
}

} // end jet namespace

