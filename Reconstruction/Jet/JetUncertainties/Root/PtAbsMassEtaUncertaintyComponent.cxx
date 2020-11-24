/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/PtAbsMassEtaUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

PtAbsMassEtaUncertaintyComponent::PtAbsMassEtaUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_absEta(false)
    , m_massDef(CompMassDef::UNKNOWN)
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PtAbsMassEtaUncertaintyComponent::PtAbsMassEtaUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_absEta(CompParametrization::isAbsEta(component.parametrization))
    , m_massDef(component.massDef)
{
    ATH_MSG_DEBUG(Form("Creating PtAbsMassEtaUncertaintyComponent named %s",m_uncHistName.Data()));
}

PtAbsMassEtaUncertaintyComponent::PtAbsMassEtaUncertaintyComponent(const PtAbsMassEtaUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_absEta(toCopy.m_absEta)
    , m_massDef(toCopy.m_massDef)
{
    ATH_MSG_DEBUG(Form("Creating copy of PtAbsMassEtaUncertaintyComponent named %s",m_uncHistName.Data()));
}

PtAbsMassEtaUncertaintyComponent* PtAbsMassEtaUncertaintyComponent::clone() const
{
    return new PtAbsMassEtaUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool PtAbsMassEtaUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,getAbsMass(jet,m_massDef)*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta()));
}

double PtAbsMassEtaUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return m_uncHist->getValue(jet.pt()*m_energyScale,getAbsMass(jet,m_massDef)*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta());
}

} // end jet namespace


