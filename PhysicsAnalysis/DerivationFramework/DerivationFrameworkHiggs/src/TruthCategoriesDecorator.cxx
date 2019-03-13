/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DerivationFrameworkHiggs/TruthCategoriesDecorator.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODJet/JetContainer.h"
#include "TruthUtils/PIDHelpers.h"
#include "PathResolver/PathResolver.h"

#include "GenInterfaces/IHiggsTruthCategoryTool.h"
#include "GenInterfaces/IxAODtoHepMCTool.h"

#include "CLHEP/Units/SystemOfUnits.h"

// Note: must include TLorentzVector before the next one
#include "TLorentzVector.h"
#include "TruthRivetTools/HiggsTemplateCrossSectionsDefs.h"

#include <TObjString.h>
#include <TObjArray.h>

namespace DerivationFramework {
  
  TruthCategoriesDecorator::TruthCategoriesDecorator(const std::string& t, const std::string& n, const IInterface* p):
    AthAlgTool(t,n,p),
    m_xAODtoHepMCTool("xAODtoHepMCTool"), 
    m_higgsTruthCatTool("HiggsTruthCategoryTool"),
    m_config(nullptr),
    m_configPath("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ConfigPath",m_configPath="DerivationFrameworkHiggs/HiggsMCsamples.cfg");
    declareProperty("DetailLevel",m_detailLevel=3);
  }
  
  TruthCategoriesDecorator::~TruthCategoriesDecorator() {}
  

  StatusCode TruthCategoriesDecorator::initialize() {
    
    ATH_MSG_INFO("Initialize " );

    // FOR xAOD->HEPMC ::  xAODtoHepMC tool
    if (m_xAODtoHepMCTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool: " << m_xAODtoHepMCTool); 
      return StatusCode::FAILURE; 
    } 
    ATH_MSG_INFO("Retrieved tool: " << m_xAODtoHepMCTool);

 
    // Higgs truth category tool 
    if (m_higgsTruthCatTool.retrieve().isFailure()) { 
      ATH_MSG_FATAL("Failed to retrieve tool: " << m_higgsTruthCatTool); 
      return StatusCode::FAILURE; 
    } 
    ATH_MSG_INFO("Retrieved tool: " << m_higgsTruthCatTool); 

    // Open the TEnv configuration file
    m_config = new TEnv();
    int status = m_config->ReadFile(PathResolverFindCalibFile(m_configPath).c_str(),EEnvLevel(0));
    if ( status != 0 ) {
      ATH_MSG_FATAL("Failed to open TEnv file "<<m_configPath);
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
  
  StatusCode TruthCategoriesDecorator::finalize(){
    return StatusCode::SUCCESS;
  }

  // Converts a string of numbers - separated by "sep" to a vector of integers
  // sep is by default any number of space characters
  std::vector<int> TruthCategoriesDecorator::vectorize(TString str, TString sep) const {
    std::vector<int> result;
    TObjArray *strings = str.Tokenize(sep.Data());
    if (strings->GetEntries()==0) { delete strings; return result; }
    TIter istr(strings);
    while (TObjString* os=(TObjString*)istr()) {
      result.push_back(atol(os->GetString()));
    }
    delete strings;
    return result;
  }

  

  HTXS::HiggsProdMode TruthCategoriesDecorator::getHiggsProductionMode(uint32_t mc_channel_number, HTXS::tH_type &th_type) const {
    if (m_config==nullptr) {
      ATH_MSG_ERROR("TEnv file pointer is NULL? Bad configuration.");
      return HTXS::HiggsProdMode::UNKNOWN;
    }

    for (TString prodMode:{"GGF","VBF","WH","QQ2ZH","GG2ZH","TTH","BBH","TH","THQB","WHT"}) {

      // loop over each mcID belonging to the production mode
      for ( int mcID : vectorize(m_config->GetValue("HTXS.MCsamples."+prodMode,"")) )
	if (mcID==(int)mc_channel_number) {
	  ATH_MSG_INFO("Higgs production for MC channel number "<<mc_channel_number<<" mode is "<<prodMode);
	  // gah, need to convert
	  if (prodMode=="GGF"  ) return HTXS::HiggsProdMode::GGF;
	  if (prodMode=="VBF"  ) return HTXS::HiggsProdMode::VBF;
	  if (prodMode=="WH"   ) return HTXS::HiggsProdMode::WH;
	  if (prodMode=="QQ2ZH") return HTXS::HiggsProdMode::QQ2ZH;
	  if (prodMode=="GG2ZH") return HTXS::HiggsProdMode::GG2ZH;
	  if (prodMode=="TTH"  ) return HTXS::HiggsProdMode::TTH;
	  if (prodMode=="BBH"  ) return HTXS::HiggsProdMode::BBH;
	  if (prodMode=="TH"   ) return HTXS::HiggsProdMode::TH;
	  if (prodMode=="THQB" ) { th_type = HTXS::tH_type::THQB; return HTXS::HiggsProdMode::TH; }
	  if (prodMode=="WHT"  ) { th_type = HTXS::tH_type::TWH; return HTXS::HiggsProdMode::TH; }
	}
    }
    ATH_MSG_WARNING("Did not managed to extract Higgs production mode for MC channel number " << 
		    mc_channel_number << ". HTXS categorization will hence not be derived.");
    return HTXS::HiggsProdMode::UNKNOWN;
  }

  // Save a TLV as 4 floats
  void TruthCategoriesDecorator::decorateFourVec(const xAOD::EventInfo *eventInfo, TString prefix, const TLorentzVector p4) const {
    eventInfo->auxdecor<float>((prefix+"_pt").Data())  = p4.Pt()*CLHEP::GeV;
    eventInfo->auxdecor<float>((prefix+"_eta").Data()) = p4.Eta();
    eventInfo->auxdecor<float>((prefix+"_phi").Data()) = p4.Phi();
    eventInfo->auxdecor<float>((prefix+"_m").Data())   = p4.M()*CLHEP::GeV;
  }

  // Save a vector of TLVs as vectors of float
  void TruthCategoriesDecorator::decorateFourVecs(const xAOD::EventInfo *eventInfo, TString prefix, const std::vector<TLorentzVector> p4s) const {
    std::vector<float> pt, eta, phi, mass;
    for (auto p4:p4s) { pt.push_back(p4.Pt()*CLHEP::GeV); eta.push_back(p4.Eta()); phi.push_back(p4.Phi()); mass.push_back(p4.M()*CLHEP::GeV); }
    eventInfo->auxdecor<std::vector<float> >((prefix+"_pt").Data())  = pt;
    eventInfo->auxdecor<std::vector<float> >((prefix+"_eta").Data()) = eta;
    eventInfo->auxdecor<std::vector<float> >((prefix+"_phi").Data()) = phi;
    eventInfo->auxdecor<std::vector<float> >((prefix+"_m").Data())   = mass;
  }
  
  StatusCode TruthCategoriesDecorator::addBranches() const{    

    // Retrieve the xAOD event info
    const xAOD::EventInfo *eventInfo = nullptr;
    ATH_CHECK( evtStore()->retrieve(eventInfo,"EventInfo") );

    // Extract the prodocution mode the first time 
    static bool first = true;
    static HTXS::HiggsProdMode prodMode = HTXS::HiggsProdMode::UNKNOWN;
    static HTXS::tH_type th_type = HTXS::tH_type::noTH;
    if (first) {
      uint32_t mcChannelNumber = eventInfo->mcChannelNumber();
      if(mcChannelNumber==0) mcChannelNumber = eventInfo->runNumber(); // EVNT input
      prodMode = getHiggsProductionMode(mcChannelNumber,th_type);
      first = false;
    }

    // If the production mode is unkown, the categorization will return -99
    // Set HTXS_prodMode decoration to indicate that HTXS categorization was indeed run
    if ( prodMode == HTXS::HiggsProdMode::UNKNOWN) {
      eventInfo->auxdecor<int>("HTXS_prodMode") = (int)prodMode;
      return StatusCode::SUCCESS;
    }

    // Retrieve the xAOD truth
    const xAOD::TruthEventContainer* xTruthEventContainer = nullptr;
    ATH_CHECK( evtStore()->retrieve(xTruthEventContainer,"TruthEvents") );

    // convert xAOD -> HepMC
    std::vector<HepMC::GenEvent> hepmc_evts = m_xAODtoHepMCTool->getHepMCEvents( xTruthEventContainer, eventInfo );

    if (hepmc_evts.size()==0) {
      // ANGRY MESSAGE HERE
      return StatusCode::FAILURE;
    }

    // classify event according to simplified template cross section
    HTXS::HiggsClassification *htxs =  m_higgsTruthCatTool->getHiggsTruthCategoryObject(hepmc_evts[0],prodMode);
    
    // Decorate the enums
    eventInfo->auxdecor<int>("HTXS_prodMode")   = (int)htxs->prodMode;
    eventInfo->auxdecor<int>("HTXS_errorCode")  = (int)htxs->errorCode;
    eventInfo->auxdecor<int>("HTXS_Stage0_Category") = (int)htxs->stage0_cat;

    eventInfo->auxdecor<int>("HTXS_Stage1_Category_pTjet25") = (int)htxs->stage1_cat_pTjet25GeV;
    eventInfo->auxdecor<int>("HTXS_Stage1_Category_pTjet30") = (int)htxs->stage1_cat_pTjet30GeV;

    eventInfo->auxdecor<int>("HTXS_Stage1_FineIndex_pTjet30") = HTXSstage1_to_HTXSstage1FineIndex(*htxs,th_type);
    eventInfo->auxdecor<int>("HTXS_Stage1_FineIndex_pTjet25") = HTXSstage1_to_HTXSstage1FineIndex(*htxs,th_type,true);

    eventInfo->auxdecor<int>("HTXS_Njets_pTjet25")  = (int)htxs->jets25.size();
    eventInfo->auxdecor<int>("HTXS_Njets_pTjet30")  = (int)htxs->jets30.size();
    

    // At the very least, save the Higgs boson pT
    if (m_detailLevel==0) eventInfo->auxdecor<float>("HTXS_Higgs_pt") = htxs->higgs.Pt()*CLHEP::GeV;

    if (m_detailLevel>0) {
      // The Higgs and the associated V (last instances prior to decay)
      decorateFourVec(eventInfo,"HTXS_Higgs",htxs->higgs);
      decorateFourVec(eventInfo,"HTXS_V",htxs->V);
    }

    if (m_detailLevel>1) {
      // Jets built excluding Higgs decay products
      decorateFourVecs(eventInfo,"HTXS_V_jets25",htxs->jets25);
      decorateFourVecs(eventInfo,"HTXS_V_jets30",htxs->jets30);
    }

    if (m_detailLevel>2) {
      // Everybody might not want this ... but good for validation
      decorateFourVec(eventInfo,"HTXS_Higgs_decay",htxs->p4decay_higgs);
      decorateFourVec(eventInfo,"HTXS_V_decay",htxs->p4decay_V);
    }

    delete htxs;

    return StatusCode::SUCCESS;

  } // addBranches
  
} // namespace
