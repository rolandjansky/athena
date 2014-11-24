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
    , m_NSegmentAccessor("GhostMuonSegmentCount")
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PunchthroughUncertaintyComponent::PunchthroughUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_absEta(CompParametrization::IsAbsEta(component.parametrization))
    , m_NSegmentAccessor("GhostMuonSegmentCount")
{
    ATH_MSG_DEBUG(Form("Creating PunchthroughUncertaintyComponent named %s",m_name.Data()));
}

PunchthroughUncertaintyComponent::PunchthroughUncertaintyComponent(const PunchthroughUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_absEta(toCopy.m_absEta)
    , m_NSegmentAccessor(toCopy.m_NSegmentAccessor)
{
    ATH_MSG_DEBUG(Form("Creating copy of PunchthroughUncertaintyComponent named %s",m_name.Data()));
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

bool PunchthroughUncertaintyComponent::getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    // const int nSegments = jet.getAttribute< std::vector<const xAOD::MuonSegment*> >("GhostMuonSegment").size()
    //const int nSegments = jet.getAttribute<int>("GhostMuonSegmentCount");
    return histo->getValidity(jet.pt()/1.e3,m_NSegmentAccessor(jet),m_absEta ? fabs(jet.eta()) : jet.eta());
}

double PunchthroughUncertaintyComponent::getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getUncertainty(jet.pt()/1.e3,m_NSegmentAccessor(jet),m_absEta ? fabs(jet.eta()) : jet.eta());
}

bool PunchthroughUncertaintyComponent::getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getValidUncertainty(unc,jet.pt()/1.e3,m_NSegmentAccessor(jet),m_absEta ? fabs(jet.eta()) : jet.eta());
}

} // end jet namespace

