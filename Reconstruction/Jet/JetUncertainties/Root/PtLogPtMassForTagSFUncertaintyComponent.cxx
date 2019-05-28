/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/PtLogPtMassForTagSFUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"
#include "BoostedJetTaggers/FatjetTruthLabel.h"
#include "PATCore/TAccept.h"

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
    , m_label()
    , m_region()
    , m_result_name()
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PtLogPtMassForTagSFUncertaintyComponent::PtLogPtMassForTagSFUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_massDef(component.massDef)
    , m_label(component.FatjetTruthLabelForSF)
    , m_region(component.RegionForSF)
    , m_result_name(component.ResultName)
{
    ATH_MSG_DEBUG(Form("Creating PtLogPtMassForTagSFUncertaintyComponent named %s",m_uncHistName.Data()));
}

PtLogPtMassForTagSFUncertaintyComponent::PtLogPtMassForTagSFUncertaintyComponent(const PtLogPtMassForTagSFUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_massDef(toCopy.m_massDef)
    , m_label(toCopy.m_label)
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
    static SG::AuxElement::Accessor<FatjetTruthLabel> accLabel("FatjetTruthLabel");
    if ( !accLabel.isAvailable(jet) ){
      ATH_MSG_ERROR("FatjetTruthLabel is not decorrated to the jet. Please call BoostedJetTaggers tag() function before calling this function.");
    }
    FatjetTruthLabel jetFlavorLabel=accLabel(jet);
    
    static SG::AuxElement::ConstAccessor<Root::TAccept> accResult(m_result_name.Data());
    Root::TAccept m_accept;
    if ( m_result_name!="" ) {
      // currently only TCC 2var tagger uses JESComponent.X.RegionForSF method, which correspont to m_region!="".
      
      if ( !accResult.isAvailable(jet) ){
	ATH_MSG_ERROR("TAccept is not decorated to the jet.");
      } else {
	m_accept=accResult(jet);
	if ( m_region.Contains("2Var_") ) {
	  // TCC 2Var tagger
	  bool passMass=(m_accept.getCutResult("PassMassLow") && m_accept.getCutResult("PassMassHigh"));
	  bool passD2  =(m_accept.getCutResult("PassD2"));
	  if ( ! ((passMass && passD2 && m_region.Contains("passMpassD2")) ||
		  (passMass && !passD2 && m_region.Contains("passMfailD2")) ||
		  (!passMass && passD2 && m_region.Contains("failMpassD2")) ||
		  (!passMass && !passD2 && m_region.Contains("failMfailD2"))) ){
	    return 0.0;
	  }
	}
      }
    }
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

