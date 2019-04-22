/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/LogPtMassUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"
#include "BoostedJetTaggers/WTopLabel.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

LogPtMassUncertaintyComponent::LogPtMassUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_massDef(CompMassDef::UNKNOWN)
    , m_label()
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

LogPtMassUncertaintyComponent::LogPtMassUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_massDef(component.massDef)
    , m_label(component.WTopTruthLabel)
{
    ATH_MSG_DEBUG(Form("Creating LogPtMassUncertaintyComponent named %s",m_uncHistName.Data()));
}

LogPtMassUncertaintyComponent::LogPtMassUncertaintyComponent(const LogPtMassUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_massDef(toCopy.m_massDef)
    , m_label(toCopy.m_label)
{
    ATH_MSG_DEBUG(Form("Creating copy of LogPtMassUncertaintyComponent named %s",m_uncHistName.Data()));
}

LogPtMassUncertaintyComponent* LogPtMassUncertaintyComponent::clone() const
{
    return new LogPtMassUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool LogPtMassUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,log10(getMassOverPt(jet,m_massDef))));
}

double LogPtMassUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    static SG::AuxElement::Accessor<WTopLabel> accLabel("WTopContainmentTruthLabel");
    if ( !accLabel.isAvailable(jet) ){
      ATH_MSG_ERROR("WTopContainmentTruthLabel is not decorrated to the jet. Please call BoostedJetTaggers tag() function before calling this function.");
    }
    WTopLabel jetFlavorLabel=accLabel(jet);

    if ( (m_label=="t_qqb" && jetFlavorLabel!=WTopLabel::t) ||
	 (m_label=="W_qq" && jetFlavorLabel!=WTopLabel::W && jetFlavorLabel!=WTopLabel::Z) ||
	 (m_label=="q" && (jetFlavorLabel==WTopLabel::t || jetFlavorLabel==WTopLabel::W || jetFlavorLabel==WTopLabel::Z)) ) {
      // if the type of uncertainty is not match to the jet truth label, return 0% uncertainty
      return 0.0;
    }

    return m_uncHist->getValue(jet.pt()*m_energyScale,log10(getMassOverPt(jet,m_massDef)));
}

} // end jet namespace

