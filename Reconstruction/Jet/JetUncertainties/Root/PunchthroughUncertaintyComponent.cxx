/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/PunchthroughUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

PunchthroughUncertaintyComponent::PunchthroughUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_absEta(false)
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PunchthroughUncertaintyComponent::PunchthroughUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_absEta(CompParametrization::isAbsEta(component.parametrization))
{
    ATH_MSG_DEBUG(Form("Creating PunchthroughUncertaintyComponent named %s",m_uncHistName.Data()));
}

PunchthroughUncertaintyComponent::PunchthroughUncertaintyComponent(const PunchthroughUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_absEta(toCopy.m_absEta)
{
    ATH_MSG_DEBUG(Form("Creating copy of PunchthroughUncertaintyComponent named %s",m_uncHistName.Data()));
}

PunchthroughUncertaintyComponent* PunchthroughUncertaintyComponent::clone() const
{
    return new PunchthroughUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool PunchthroughUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,getNumSegments(jet),m_absEta ? fabs(jet.eta()) : jet.eta()));
}

double PunchthroughUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return m_uncHist->getValue(jet.pt()*m_energyScale,getNumSegments(jet),m_absEta ? fabs(jet.eta()) : jet.eta());
}

int PunchthroughUncertaintyComponent::getNumSegments(const xAOD::Jet& jet) const
{

//    std::cout << "Calling getNumSegments for component " << getName() << std::endl;

    static SG::AuxElement::Accessor<int> accNseg("GhostMuonSegmentCount");

    if (!accNseg.isAvailable(jet))
    {
        ATH_MSG_ERROR("Failed to retrieve number of segments from the jet");
        return -1;
    }
    return accNseg(jet);
}

} // end jet namespace

