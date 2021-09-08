/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/ELogMassEtaUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

ELogMassEtaUncertaintyComponent::ELogMassEtaUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_absEta(false)
    , m_massDef(CompMassDef::UNKNOWN)
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

ELogMassEtaUncertaintyComponent::ELogMassEtaUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_absEta(CompParametrization::isAbsEta(component.parametrization))
    , m_massDef(component.massDef)
{
    ATH_MSG_DEBUG(Form("Creating ELogMassEtaUncertaintyComponent named %s",m_uncHistName.Data()));
}

ELogMassEtaUncertaintyComponent::ELogMassEtaUncertaintyComponent(const ELogMassEtaUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_absEta(toCopy.m_absEta)
    , m_massDef(toCopy.m_massDef)
{
    ATH_MSG_DEBUG(Form("Creating copy of ELogMassEtaUncertaintyComponent named %s",m_uncHistName.Data()));
}

ELogMassEtaUncertaintyComponent* ELogMassEtaUncertaintyComponent::clone() const
{
    return new ELogMassEtaUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool ELogMassEtaUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.e()*m_energyScale,log(getMassOverE(jet,m_massDef)),m_absEta ? fabs(jet.eta()) : jet.eta()));
}

double ELogMassEtaUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return m_uncHist->getValue(jet.e()*m_energyScale,log(getMassOverE(jet,m_massDef)),m_absEta ? fabs(jet.eta()) : jet.eta());
}

} // end jet namespace

