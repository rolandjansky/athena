/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/PtMassEtaUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

PtMassEtaUncertaintyComponent::PtMassEtaUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_absEta(false)
    , m_massDef(CompMassDef::UNKNOWN)
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PtMassEtaUncertaintyComponent::PtMassEtaUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_absEta(CompParametrization::isAbsEta(component.parametrization))
    , m_massDef(component.massDef)
{
    ATH_MSG_DEBUG(Form("Creating PtMassEtaUncertaintyComponent named %s",m_uncHistName.Data()));
}

PtMassEtaUncertaintyComponent::PtMassEtaUncertaintyComponent(const PtMassEtaUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_absEta(toCopy.m_absEta)
    , m_massDef(toCopy.m_massDef)
{
    ATH_MSG_DEBUG(Form("Creating copy of PtMassEtaUncertaintyComponent named %s",m_uncHistName.Data()));
}

PtMassEtaUncertaintyComponent* PtMassEtaUncertaintyComponent::clone() const
{
    return new PtMassEtaUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool PtMassEtaUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,getMassOverPt(jet,m_massDef),m_absEta ? fabs(jet.eta()) : jet.eta()));
}

double PtMassEtaUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return m_uncHist->getValue(jet.pt()*m_energyScale,getMassOverPt(jet,m_massDef),m_absEta ? fabs(jet.eta()) : jet.eta());
}

} // end jet namespace

