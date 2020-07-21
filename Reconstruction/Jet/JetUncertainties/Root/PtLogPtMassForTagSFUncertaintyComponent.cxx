/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/PtLogPtMassForTagSFUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

PtLogPtMassForTagSFUncertaintyComponent::PtLogPtMassForTagSFUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_massDef(CompMassDef::UNKNOWN)
    , m_labels()
    , m_largeRJetTruthLabelName("")
    , m_region()
    , m_result_name()
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PtLogPtMassForTagSFUncertaintyComponent::PtLogPtMassForTagSFUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_massDef(component.massDef)
    , m_labels(component.LargeRJetTruthLabelsForSF)
    , m_largeRJetTruthLabelName(component.LargeRJetTruthLabelName)
    , m_region(component.RegionForSF)
    , m_result_name(component.ResultName)
{
    ATH_MSG_DEBUG(Form("Creating PtLogPtMassForTagSFUncertaintyComponent named %s",m_uncHistName.Data()));
}

PtLogPtMassForTagSFUncertaintyComponent::PtLogPtMassForTagSFUncertaintyComponent(const PtLogPtMassForTagSFUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_massDef(toCopy.m_massDef)
    , m_labels(toCopy.m_labels)
    , m_largeRJetTruthLabelName(toCopy.m_largeRJetTruthLabelName)
    , m_region(toCopy.m_region)
    , m_result_name(toCopy.m_result_name)
{
    ATH_MSG_DEBUG(Form("Creating copy of PtLogPtMassForTagSFUncertaintyComponent named %s",m_uncHistName.Data()));
}

PtLogPtMassForTagSFUncertaintyComponent* PtLogPtMassForTagSFUncertaintyComponent::clone() const
{
    return new PtLogPtMassForTagSFUncertaintyComponent(*this);
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool PtLogPtMassForTagSFUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,log(getMassOverPt(jet,m_massDef))));
}

double PtLogPtMassForTagSFUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    static const SG::AuxElement::Accessor<int> accLabel(m_largeRJetTruthLabelName);
    if ( !accLabel.isAvailable(jet) ){
      ATH_MSG_ERROR("LargeRJetTruthLabel: " << m_largeRJetTruthLabelName << " is not decorrated to the jet. Please use JetTruthLabelingTool before calling this function.");
      return JESUNC_ERROR_CODE;
    }
    int jetFlavorLabelInt=accLabel(jet);
    LargeRJetTruthLabel::TypeEnum jetFlavorLabel=LargeRJetTruthLabel::intToEnum(jetFlavorLabelInt);
    
    float mOverPt=jet.m()/jet.pt();
    if ( m_result_name!="" ) {
      // currently only TCC 2var tagger uses JESComponent.X.RegionForSF method, which correspont to m_region!="".
      
      SG::AuxElement::ConstAccessor<int> accResult(m_result_name.Data());
      if ( !accResult.isAvailable(jet) ){
	ATH_MSG_ERROR(m_result_name+" is not decorated to the jet.");
      } else {
	TagResult::TypeEnum myCutResult=TagResult::intToEnum(accResult(jet));
	if ( m_region==CompTaggerRegionVar::passMpassD2_2Var ||
	     m_region==CompTaggerRegionVar::passMfailD2_2Var ||
	     m_region==CompTaggerRegionVar::failMpassD2_2Var ||
	     m_region==CompTaggerRegionVar::failMfailD2_2Var) {
	  // TCC 2Var tagger
	  if ( m_result_name.Contains("SmoothZ") ){
	    // to apply W-tagging efficiency SF to Z-tagger, jet mass is shifted by 10GeV
	    const double WtoZmassShift = 10803;
	    mOverPt=(jet.m()-WtoZmassShift)/jet.pt();
	  }
	  if ( ! ((myCutResult==TagResult::passMpassD2_2Var && m_region==CompTaggerRegionVar::passMpassD2_2Var) ||
		  (myCutResult==TagResult::passMfailD2_2Var && m_region==CompTaggerRegionVar::passMfailD2_2Var) ||
		  (myCutResult==TagResult::failMpassD2_2Var && m_region==CompTaggerRegionVar::failMpassD2_2Var) ||
		  (myCutResult==TagResult::failMfailD2_2Var && m_region==CompTaggerRegionVar::failMfailD2_2Var)) ){
	    return 0.0;
	  }
	} else {
	  ATH_MSG_ERROR("RegionForSF is specified in the config file, but it is not available for this tagger.");
	  return 0.0;
	}
      }
    }

    bool isValidLabel=false;
    for ( CompFlavorLabelVar::TypeEnum m_label : m_labels ) {
      if ( (m_label==CompFlavorLabelVar::t_qqb && jetFlavorLabel==LargeRJetTruthLabel::tqqb) ||
	   (m_label==CompFlavorLabelVar::t && (jetFlavorLabel==LargeRJetTruthLabel::tqqb || jetFlavorLabel==LargeRJetTruthLabel::other_From_t)) ||
	   (m_label==CompFlavorLabelVar::t_other && (jetFlavorLabel==LargeRJetTruthLabel::Wqq_From_t || jetFlavorLabel==LargeRJetTruthLabel::other_From_t)) ||
	   (m_label==CompFlavorLabelVar::V_qq && (jetFlavorLabel==LargeRJetTruthLabel::Wqq || jetFlavorLabel==LargeRJetTruthLabel::Zqq || jetFlavorLabel==LargeRJetTruthLabel::Wqq_From_t)) ||
	   (m_label==CompFlavorLabelVar::W_qq_From_t && jetFlavorLabel==LargeRJetTruthLabel::Wqq_From_t) ||
	   (m_label==CompFlavorLabelVar::W_qq && jetFlavorLabel==LargeRJetTruthLabel::Wqq) ||
	   (m_label==CompFlavorLabelVar::Z_qq && jetFlavorLabel==LargeRJetTruthLabel::Zqq) ||
	   (m_label==CompFlavorLabelVar::q && (jetFlavorLabel==LargeRJetTruthLabel::notruth || jetFlavorLabel==LargeRJetTruthLabel::qcd || jetFlavorLabel==LargeRJetTruthLabel::other_From_V)) ) {
	isValidLabel=true;
      }
    }
    if (!isValidLabel) return 0.0;// if the type of uncertainty is not match to the jet truth label, return 0% uncertainty
    return m_uncHist->getValue(jet.pt()*m_energyScale,log(mOverPt));
}

} // end jet namespace

