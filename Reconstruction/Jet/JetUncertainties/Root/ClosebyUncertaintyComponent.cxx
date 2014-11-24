/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/ClosebyUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

ClosebyUncertaintyComponent::ClosebyUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_minDRaccessor("MinDR")
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

ClosebyUncertaintyComponent::ClosebyUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_minDRaccessor("MinDR")
{
    ATH_MSG_DEBUG(Form("Creating ClosebyUncertaintyComponent named %s",m_name.Data()));
}

ClosebyUncertaintyComponent::ClosebyUncertaintyComponent(const ClosebyUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_minDRaccessor(toCopy.m_minDRaccessor)
{
    ATH_MSG_DEBUG(Form("Creating copy of ClosebyUncertaintyComponent named %s",m_name.Data()));
}

ClosebyUncertaintyComponent* ClosebyUncertaintyComponent::clone() const
{
    return new ClosebyUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool ClosebyUncertaintyComponent::getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    //float minDR = 0;
    //if (!jet.getAttribute<float>("MinDR",minDR))
    //{
    //    ATH_MSG_ERROR("Failed to retrieve MinDR from jet");
    //    return false;
    //}

    return histo->getValidity(jet.pt()/1.e3,m_minDRaccessor(jet));
}

double ClosebyUncertaintyComponent::getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getUncertainty(jet.pt()/1.e3,m_minDRaccessor(jet));
}

bool ClosebyUncertaintyComponent::getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getValidUncertainty(unc,jet.pt()/1.e3,m_minDRaccessor(jet));
}

} // end jet namespace

