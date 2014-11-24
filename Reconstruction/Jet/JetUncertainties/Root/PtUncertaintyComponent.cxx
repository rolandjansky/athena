/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/PtUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

PtUncertaintyComponent::PtUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PtUncertaintyComponent::PtUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
{
    ATH_MSG_DEBUG(Form("Creating PtUncertaintyComponent named %s",m_name.Data()));
}

PtUncertaintyComponent::PtUncertaintyComponent(const PtUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
{
    ATH_MSG_DEBUG(Form("Creating copy of PtUncertaintyComponent named %s",m_name.Data()));
}

PtUncertaintyComponent* PtUncertaintyComponent::clone() const
{
    return new PtUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool PtUncertaintyComponent::getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getValidity(jet.pt()/1.e3);
}

double PtUncertaintyComponent::getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getUncertainty(jet.pt()/1.e3);
}

bool PtUncertaintyComponent::getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getValidUncertainty(unc,jet.pt()/1.e3);
}

} // end jet namespace

