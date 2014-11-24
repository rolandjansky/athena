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
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PtMassEtaUncertaintyComponent::PtMassEtaUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_absEta(CompParametrization::IsAbsEta(component.parametrization))
{
    ATH_MSG_DEBUG(Form("Creating PtMassEtaUncertaintyComponent named %s",m_name.Data()));
}

PtMassEtaUncertaintyComponent::PtMassEtaUncertaintyComponent(const PtMassEtaUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_absEta(toCopy.m_absEta)
{
    ATH_MSG_DEBUG(Form("Creating copy of PtMassEtaUncertaintyComponent named %s",m_name.Data()));
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

bool PtMassEtaUncertaintyComponent::getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getValidity(jet.pt()/1.e3,jet.m()/jet.pt(),m_absEta ? fabs(jet.eta()) : jet.eta());
}

double PtMassEtaUncertaintyComponent::getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getUncertainty(jet.pt()/1.e3,jet.m()/jet.pt(),m_absEta ? fabs(jet.eta()) : jet.eta());
}

bool PtMassEtaUncertaintyComponent::getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getValidUncertainty(unc,jet.pt()/1.e3,jet.m()/jet.pt(),m_absEta ? fabs(jet.eta()) : jet.eta());
}

} // end jet namespace

