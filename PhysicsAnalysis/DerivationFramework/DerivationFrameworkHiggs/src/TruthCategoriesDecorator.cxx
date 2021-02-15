/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "DerivationFrameworkHiggs/TruthCategoriesDecorator.h"
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
#include "StoreGate/WriteDecorHandle.h"

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
    ATH_CHECK(m_xAODtoHepMCTool.retrieve());
 
    // Higgs truth category tool 
    ATH_CHECK(m_higgsTruthCatTool.retrieve());

    // Open the TEnv configuration file
    m_config = new TEnv();
    int status = m_config->ReadFile(PathResolverFindCalibFile(m_configPath).c_str(),EEnvLevel(0));
    if ( status != 0 ) {
      ATH_MSG_FATAL("Failed to open TEnv file "<<m_configPath);
      return StatusCode::FAILURE;
    }

    //All the decorations we want to add go here
    m_eventInfoIntDecorNames.push_back("HTXS_prodMode");
    m_eventInfoIntDecorNames.push_back("HTXS_errorCode");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage0_Category");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage1_Category_pTjet25");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage1_Category_pTjet30");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage1_FineIndex_pTjet30");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage1_FineIndex_pTjet25");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage1_2_Category_pTjet25");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage1_2_Category_pTjet30");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage1_2_FineIndex_pTjet30");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage1_2_FineIndex_pTjet25");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage1_2_Fine_Category_pTjet25");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage1_2_Fine_Category_pTjet30");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage1_2_Fine_FineIndex_pTjet25");
    m_eventInfoIntDecorNames.push_back("HTXS_Stage1_2_Fine_FineIndex_pTjet30");
    m_eventInfoIntDecorNames.push_back("HTXS_Njets_pTjet25");
    m_eventInfoIntDecorNames.push_back("HTXS_Njets_pTjet30");
    m_eventInfoIntDecorNames.push_back("HTXS_isZ2vvDecay");

    m_eventInfoFloatDecorNames.push_back("HTXS_Higgs_pt");
    std::string fourvecconts[4]={"_pt","_eta","_phi","_m"};
    for(int i=0;i<4;i++){
      m_eventInfoFloatDecorNames.push_back("HTXS_Higgs"+fourvecconts[i]);
      m_eventInfoFloatDecorNames.push_back("HTXS_V"+fourvecconts[i]);
      m_eventInfoFloatDecorNames.push_back("HTXS_Higgs_Decay"+fourvecconts[i]);
      m_eventInfoFloatDecorNames.push_back("HTXS_V_Decay"+fourvecconts[i]);
      m_eventInfoVectorFloatDecorNames.push_back("HTXS_V_jets25"+fourvecconts[i]);
      m_eventInfoVectorFloatDecorNames.push_back("HTXS_V_jets30"+fourvecconts[i]);
    }

    for(auto name : m_eventInfoIntDecorNames) m_eventInfoIntDecors.emplace_back("EventInfo."+name);
    for(auto name : m_eventInfoFloatDecorNames) m_eventInfoFloatDecors.emplace_back("EventInfo."+name);
    for(auto name : m_eventInfoVectorFloatDecorNames) m_eventInfoVectorFloatDecors.emplace_back("EventInfo."+name);

    ATH_CHECK(m_eventInfoIntDecors.initialize());
    ATH_CHECK(m_eventInfoFloatDecors.initialize());
    ATH_CHECK(m_eventInfoVectorFloatDecors.initialize());
    ATH_CHECK(m_truthEventCont.initialize());
    return StatusCode::SUCCESS;
  }
  
  StatusCode TruthCategoriesDecorator::finalize(){
    if(m_config) delete m_config;
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
      for ( int mcID : vectorize(m_config->GetValue("HTXS.MCsamples."+prodMode,"")) ){
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
    }
    // This is perfectly fine if we aren't running on a Higgs sample
    ATH_MSG_INFO("Did not manage to extract Higgs production mode for MC channel number " << 
                    mc_channel_number << ". HTXS categorization will hence not be derived.");
    return HTXS::HiggsProdMode::UNKNOWN;
  }

  float TruthCategoriesDecorator::getFloatDecor(const std::string key, const TLorentzVector p4) const {
    if(key.find("_pt")!=std::string::npos) return p4.Pt()*CLHEP::GeV;
    else if(key.find("_eta")!=std::string::npos) return p4.Eta();
    else if(key.find("_phi")!=std::string::npos) return p4.Phi();
    else return p4.M()*CLHEP::GeV;
  }

  std::vector<float> TruthCategoriesDecorator::getVectorFloatDecor(const std::string key, const std::vector<TLorentzVector> p4s) const {
    std::vector<float> vec;
    if(key.find("_pt")!=std::string::npos){
      for (auto p4:p4s) vec.push_back(p4.Pt()*CLHEP::GeV);
    }
    else if(key.find("_eta")!=std::string::npos){
      for (auto p4:p4s) vec.push_back(p4.Eta());
    }
    else if(key.find("_phi")!=std::string::npos){
      for (auto p4:p4s) vec.push_back(p4.Phi());
    }
    else if(key.find("_m")!=std::string::npos){
      for (auto p4:p4s) vec.push_back(p4.M()*CLHEP::GeV);
    }
    return vec;
  }


  StatusCode TruthCategoriesDecorator::addBranches() const{

    // Get a handle so we can retrieve the eventInfo information
    SG::WriteDecorHandle<xAOD::EventInfo, int> prodModeHandle(m_eventInfoIntDecors.at(0));

    // Extract the prodocution mode the first time 
    static bool first = true;
    static HTXS::HiggsProdMode prodMode = HTXS::HiggsProdMode::UNKNOWN;
    static HTXS::tH_type th_type = HTXS::tH_type::noTH;
    if (first) {
      uint32_t mcChannelNumber = prodModeHandle->mcChannelNumber();
      if(mcChannelNumber==0) mcChannelNumber = prodModeHandle->runNumber(); // EVNT input
      prodMode = getHiggsProductionMode(mcChannelNumber,th_type);
      first = false;
    }

    // If the production mode is unkown, the categorization will return -99
    // Set HTXS_prodMode decoration to indicate that HTXS categorization was indeed run
    if ( prodMode == HTXS::HiggsProdMode::UNKNOWN) {
      //cheating a bit here, prodMode is the first decorator
      prodModeHandle(0) = (int)prodMode;
      return StatusCode::SUCCESS;
    }

    // Retrieve the xAOD truth
    SG::ReadHandle<xAOD::TruthEventContainer> truthEventCont(m_truthEventCont);

    // convert xAOD -> HepMC
    std::vector<HepMC::GenEvent> hepmc_evts = m_xAODtoHepMCTool->getHepMCEvents( truthEventCont.cptr(), prodModeHandle.cptr() );

    if (hepmc_evts.size()==0) {
      // ANGRY MESSAGE HERE
      return StatusCode::FAILURE;
    }

    // classify event according to simplified template cross section
    HTXS::HiggsClassification *htxs =  m_higgsTruthCatTool->getHiggsTruthCategoryObject(hepmc_evts[0],prodMode);

    // Decorate
    prodModeHandle(0)=(int)htxs->prodMode;
    for( auto& key : m_eventInfoIntDecors){
      //already did this one
      if(key.key().find("HTXS_prodMode")!=std::string::npos) continue;
      SG::WriteDecorHandle<xAOD::EventInfo, int> handle(key);
      if(key.key().find("HTXS_errorCode")!=std::string::npos) handle(0) = (int)htxs->errorCode;
      else if(key.key().find("HTXS_Stage0_Category")!=std::string::npos) handle(0) = (int)htxs->stage0_cat;
      // Stage-1 binning
      else if(key.key().find("HTXS_Stage1_Category_pTjet25")!=std::string::npos) handle(0) = (int)htxs->stage1_cat_pTjet25GeV;
      else if(key.key().find("HTXS_Stage1_Category_pTjet30")!=std::string::npos) handle(0) = (int)htxs->stage1_cat_pTjet30GeV;
      else if(key.key().find("HTXS_Stage1_FineIndex_pTjet25")!=std::string::npos) handle(0) = HTXSstage1_to_HTXSstage1FineIndex(*htxs,th_type);
      else if(key.key().find("HTXS_Stage1_FineIndex_pTjet30")!=std::string::npos) handle(0) = HTXSstage1_to_HTXSstage1FineIndex(*htxs,th_type,true);
      // Stage-1.2 binning
      else if(key.key().find("HTXS_Stage1_2_Category_pTjet25")!=std::string::npos) handle(0) = (int)htxs->stage1_2_cat_pTjet25GeV;
      else if(key.key().find("HTXS_Stage1_2_Category_pTjet30")!=std::string::npos) handle(0) = (int)htxs->stage1_2_cat_pTjet30GeV;
      else if(key.key().find("HTXS_Stage1_2_FineIndex_pTjet25")!=std::string::npos) handle(0) = HTXSstage1_2_to_HTXSstage1_2_FineIndex(*htxs,th_type);
      else if(key.key().find("HTXS_Stage1_2_FineIndex_pTjet30")!=std::string::npos) handle(0) = HTXSstage1_2_to_HTXSstage1_2_FineIndex(*htxs,th_type,true);
      // Stage-1.2 finer binning
      else if(key.key().find("HTXS_Stage1_2_Fine_Category_pTjet25")!=std::string::npos) handle(0) = (int)htxs->stage1_2_fine_cat_pTjet25GeV;
      else if(key.key().find("HTXS_Stage1_2_Fine_Category_pTjet30")!=std::string::npos) handle(0) = (int)htxs->stage1_2_fine_cat_pTjet30GeV;
      else if(key.key().find("HTXS_Stage1_2_Fine_FineIndex_pTjet25")!=std::string::npos) handle(0) = HTXSstage1_2_Fine_to_HTXSstage1_2_Fine_FineIndex(*htxs,th_type);
      else if(key.key().find("HTXS_Stage1_2_Fine_FineIndex_pTjet30")!=std::string::npos) handle(0) = HTXSstage1_2_Fine_to_HTXSstage1_2_Fine_FineIndex(*htxs,th_type,true);
      //Njets
      else if(key.key().find("HTXS_Njets_pTjet25")!=std::string::npos) handle(0) = (int)htxs->jets25.size();
      else if(key.key().find("HTXS_Njets_pTjet30")!=std::string::npos) handle(0) = (int)htxs->jets30.size();
      else if(key.key().find("HTXS_isZ2vvDecay")!=std::string::npos) handle(0) = (bool)htxs->isZ2vvDecay;
    }
    for( auto& key : m_eventInfoFloatDecors){
      SG::WriteDecorHandle<xAOD::EventInfo, float> handle(key);
      // At the very least, save the Higgs boson pT
      if(key.key().find("HTXS_Higgs_pt")!=std::string::npos){
	if (m_detailLevel==0) handle(0) = htxs->higgs.Pt()*CLHEP::GeV;
      }
      // The Higgs and the associated V (last instances prior to decay)
      else if(key.key().find("HTXS_Higgs")!=std::string::npos && key.key().find("decay")==std::string::npos){
	if (m_detailLevel>0) handle(0)=getFloatDecor(key.key(),htxs->higgs);
      }
      else if(key.key().find("HTXS_V")!=std::string::npos && key.key().find("decay")==std::string::npos && key.key().find("jets")==std::string::npos){
	if (m_detailLevel>0) handle(0)=getFloatDecor(key.key(),htxs->V);
      }
      // Everybody might not want this ... but good for validation
      else if(key.key().find("HTXS_Higgs_decay")!=std::string::npos){
	if (m_detailLevel>2) handle(0)=getFloatDecor(key.key(),htxs->p4decay_higgs);
      }
      else if(key.key().find("HTXS_V_decay")!=std::string::npos){
	if (m_detailLevel>2) handle(0)=getFloatDecor(key.key(),htxs->p4decay_V);
      }
    }
    for( auto& key : m_eventInfoVectorFloatDecors){
      SG::WriteDecorHandle<xAOD::EventInfo, std::vector<float> > handle(key);
      // Jets built excluding Higgs decay products
      if(key.key().find("HTXS_V_jets25")!=std::string::npos){
	if (m_detailLevel>1) handle(0)=getVectorFloatDecor(key.key(),htxs->jets25);
      }
      else if(key.key().find("HTXS_V_jets30")!=std::string::npos){
        if (m_detailLevel>1) handle(0)=getVectorFloatDecor(key.key(),htxs->jets30);
      }
    }

    delete htxs;

    return StatusCode::SUCCESS;

  } // addBranches
  
} // namespace
