/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/PtEtaUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

PtEtaUncertaintyComponent::PtEtaUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_absEta(false)
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PtEtaUncertaintyComponent::PtEtaUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_absEta(CompParametrization::IsAbsEta(component.parametrization))
{
    ATH_MSG_DEBUG(Form("Creating PtEtaUncertaintyComponent named %s",m_name.Data()));
}

PtEtaUncertaintyComponent::PtEtaUncertaintyComponent(const PtEtaUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_absEta(toCopy.m_absEta)
{
    ATH_MSG_DEBUG(Form("Creating copy of PtEtaUncertaintyComponent named %s",m_name.Data()));
}

PtEtaUncertaintyComponent* PtEtaUncertaintyComponent::clone() const
{
    return new PtEtaUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool PtEtaUncertaintyComponent::getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getValidity(jet.pt()/1.e3,m_absEta ? fabs(jet.eta()) : jet.eta());
}

double PtEtaUncertaintyComponent::getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getUncertainty(jet.pt()/1.e3,m_absEta ? fabs(jet.eta()) : jet.eta());
}

bool PtEtaUncertaintyComponent::getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getValidUncertainty(unc,jet.pt()/1.e3,m_absEta ? fabs(jet.eta()) : jet.eta());
}

} // end jet namespace

