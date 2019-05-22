/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/ELogPtMassForTagSFUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"
#include "BoostedJetTaggers/FatjetTruthLabel.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

ELogPtMassForTagSFUncertaintyComponent::ELogPtMassForTagSFUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_massDef(CompMassDef::UNKNOWN)
    , m_label()
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

ELogPtMassForTagSFUncertaintyComponent::ELogPtMassForTagSFUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_massDef(component.massDef)
    , m_label(component.FatjetTruthLabelForSF)
{
    ATH_MSG_DEBUG(Form("Creating ELogPtMassForTagSFUncertaintyComponent named %s",m_uncHistName.Data()));
}

ELogPtMassForTagSFUncertaintyComponent::ELogPtMassForTagSFUncertaintyComponent(const ELogPtMassForTagSFUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_massDef(toCopy.m_massDef)
    , m_label(toCopy.m_label)
{
    ATH_MSG_DEBUG(Form("Creating copy of ELogPtMassForTagSFUncertaintyComponent named %s",m_uncHistName.Data()));
}

ELogPtMassForTagSFUncertaintyComponent* ELogPtMassForTagSFUncertaintyComponent::clone() const
{
    return new ELogPtMassForTagSFUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool ELogPtMassForTagSFUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,log(getMassOverPt(jet,m_massDef))));
}

double ELogPtMassForTagSFUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    static SG::AuxElement::Accessor<FatjetTruthLabel> accLabel("FatjetTruthLabel");
    if ( !accLabel.isAvailable(jet) ){
      ATH_MSG_ERROR("FatjetTruthLabel is not decorrated to the jet. Please call BoostedJetTaggers tag() function before calling this function.");
    }
    FatjetTruthLabel jetFlavorLabel=accLabel(jet);

    if ( (m_label=="t_qqb" && jetFlavorLabel!=FatjetTruthLabel::tqqb) ||
	 (m_label=="t" && jetFlavorLabel!=FatjetTruthLabel::tqqb && jetFlavorLabel!=FatjetTruthLabel::Wqq_From_t && jetFlavorLabel!=FatjetTruthLabel::other_From_t) ||
	 (m_label=="V_qq" && jetFlavorLabel!=FatjetTruthLabel::Wqq && jetFlavorLabel!=FatjetTruthLabel::Zqq) ||
	 (m_label=="W_qq" && jetFlavorLabel!=FatjetTruthLabel::Wqq) ||
	 (m_label=="Z_qq" && jetFlavorLabel!=FatjetTruthLabel::Zqq) ||
	 (m_label=="q" && jetFlavorLabel!=FatjetTruthLabel::notruth && jetFlavorLabel!=FatjetTruthLabel::unknown) ) {
      // if the type of uncertainty is not match to the jet truth label, return 0% uncertainty
      return 0.0;
    }
    
    return m_uncHist->getValue(jet.pt()*m_energyScale,log(getMassOverPt(jet,m_massDef)));
}

} // end jet namespace

