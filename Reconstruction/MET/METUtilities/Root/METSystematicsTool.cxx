/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "METUtilities/METSystematicsTool.h"
#include "TFile.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TH3.h"
#include "TH2.h"

#include <iostream>
// #include <boost/filesystem.hpp>
#include "PathResolver/PathResolver.h"

// xAOD includes
#include "xAODCore/ShallowCopy.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODJet/JetContainer.h"


namespace met {
  
  using namespace xAOD;
  
  METSystematicsTool::METSystematicsTool(const std::string& name) : asg::AsgTool::AsgTool(name),
  m_appliedSystEnum(NONE),
  shiftpara_pthard_njet_mu(nullptr),
  resopara_pthard_njet_mu(nullptr),
  resoperp_pthard_njet_mu(nullptr),
  jet_systRpt_pt_eta(nullptr),
  h_calosyst_scale(nullptr),
  h_calosyst_reso (nullptr),
  m_rand(0)
  {
    ATH_MSG_DEBUG (__PRETTY_FUNCTION__ );
    
    declareProperty( "JetColl",           m_jetColl           = "AntiKt4LCTopoJets"                );
    declareProperty( "ConfigPrefix",      m_configPrefix      = "METUtilities/data12_8TeV/final2/" );
    declareProperty( "ConfigSoftTrkFile", m_configSoftTrkFile = "TrackSoftTerms.config"            );
    declareProperty( "ConfigJetTrkFile",  m_configJetTrkFile  = ""                                 ); 
    declareProperty( "ConfigSoftCaloFile",m_configSoftCaloFile= "METRefFinal.config"               );
    declareProperty( "TruthContainer",    m_truthCont         = "MET_Truth"                        );
    declareProperty( "TruthObj",          m_truthObj          = "NonInt"                           );
    declareProperty( "VertexContainer",   m_vertexCont        = "PrimaryVertices"                  );
    declareProperty( "EventInfo",         m_eventInfo         = "EventInfo"                        );
    
    applySystematicVariation(CP::SystematicSet()).ignore();
  }
  
  StatusCode METSystematicsTool::addMETAffectingSystematics(){
    ATH_MSG_VERBOSE (__PRETTY_FUNCTION__ );
    
    if(!m_configSoftTrkFile.empty()){
      if( !(addAffectingSystematic( softTrkAffSyst::MET_SoftTrk_ScaleUp  , true /*recommended */ ) &&
            addAffectingSystematic( softTrkAffSyst::MET_SoftTrk_ScaleDown, true /*recommended */ ) &&
            addAffectingSystematic( softTrkAffSyst::MET_SoftTrk_ResoPara , true /*recommended */ ) &&
            addAffectingSystematic( softTrkAffSyst::MET_SoftTrk_ResoPerp , true /*recommended */ ) &&
            addAffectingSystematic( softTrkAffSyst::MET_SoftTrk_ResoCorr , false /*not recommended */) ) ) {
        ATH_MSG_ERROR("failed to properly add softTrk affecting systematics " );
        return StatusCode::FAILURE;
      }
    }
    if(!m_configSoftCaloFile.empty()){
      if( !(addAffectingSystematic( softCaloAffSyst::MET_SoftCalo_ScaleUp  , true /*recommended */ ) &&
            addAffectingSystematic( softCaloAffSyst::MET_SoftCalo_ScaleDown, true /*recommended */ ) &&
            addAffectingSystematic( softCaloAffSyst::MET_SoftCalo_Reso     , true /*recommended */ ) ) ) {
        ATH_MSG_ERROR("failed to properly add softCalo affecting systematics " );
        return StatusCode::FAILURE;
      }
    }
    if(!m_configJetTrkFile.empty()){
      if( !(addAffectingSystematic( jetTrkAffSyst::MET_JetTrk_ScaleUp  , true /*recommended */ ) &&
            addAffectingSystematic( jetTrkAffSyst::MET_JetTrk_ScaleDown, true /*recommended */ ) ) ){
        ATH_MSG_ERROR("failed to properly add jetTrk affecting systematics " );
        return StatusCode::FAILURE;
      }
    }

    ATH_MSG_INFO( "AffectingSystematics are:" );
    for(const auto& syst : m_affectingSystematics) {
      ATH_MSG_INFO( syst.name() );
    }

    ATH_MSG_DEBUG("These systematics are set based on your config files: " );
    ATH_MSG_DEBUG("ConfigSoftTrkFile: " << m_configSoftTrkFile );
    ATH_MSG_DEBUG("ConfigSoftCaloFile: "<< m_configSoftCaloFile);
    ATH_MSG_DEBUG("ConfigSoftJetFile: " << m_configJetTrkFile  );

    return StatusCode::SUCCESS;
  }
  
  StatusCode METSystematicsTool::initialize()
  {
    ATH_MSG_VERBOSE (__PRETTY_FUNCTION__ );
    
    const char lastchar = m_configPrefix.back();
    if(std::strncmp(&lastchar,"/",1)!=0) {
      m_configPrefix.append("/");
    }

    if(m_configSoftTrkFile.empty()  &&
       m_configSoftCaloFile.empty() &&
       m_configJetTrkFile.empty() )  ATH_MSG_WARNING("you have initialized the METSystematicsTool with no configuration file.  The tool will do nothing.  Please set the configuration file properties.");
    
    if(!m_configSoftTrkFile.empty())  ATH_CHECK( softTrkSystInitialize() ) ;
    if(!m_configSoftCaloFile.empty()) ATH_CHECK( softCaloSystInitialize());
    if(!m_configJetTrkFile.empty())   ATH_CHECK( jetTrkSystInitialize()  ); 

    return addMETAffectingSystematics();
  }
  
  StatusCode METSystematicsTool::finalize()
  {
    ATH_MSG_DEBUG (__PRETTY_FUNCTION__);
    
    delete shiftpara_pthard_njet_mu;
    delete resopara_pthard_njet_mu;
    delete resoperp_pthard_njet_mu;
    delete jet_systRpt_pt_eta;
    delete h_calosyst_scale;
    delete h_calosyst_reso;
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode METSystematicsTool::softCaloSystInitialize()
  {
    ATH_MSG_VERBOSE (__PRETTY_FUNCTION__);
    
    std::string histfile  = "";
    std::string gsystpath = "";
    std::string histpath = "";
    std::string blank     = "";
    
    ATH_CHECK( extractHistoPath(histfile,gsystpath,histpath,blank ,SOFTCALO) );//properly sets the paths
    
    TFile infile(histpath.c_str());

    ATH_MSG_INFO( "METSystematics: Read calo uncertainties" );
    h_calosyst_scale = dynamic_cast<TH1D*>( infile.Get((gsystpath+"/globsyst_scale").c_str()));
    h_calosyst_reso  = dynamic_cast<TH1D*>( infile.Get((gsystpath+"/globsyst_reso").c_str()));
    
    if( !(h_calosyst_scale &&
          h_calosyst_reso
          )
	)
      {
	ATH_MSG_ERROR("Could not get all calo histos from the config file:" << histfile);
	return StatusCode::FAILURE;
      }
    
    h_calosyst_scale->SetDirectory(0);
    h_calosyst_reso ->SetDirectory(0);
    
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ << "  DONE!!" );
    return StatusCode::SUCCESS;
  }
  
  
  StatusCode METSystematicsTool::jetTrkSystInitialize()
  {
    ATH_MSG_VERBOSE (__PRETTY_FUNCTION__);
  
    std::string histfile  = "";
    std::string gsystpath = "";
    std::string configdir = "";
    std::string blank     = "";
  
    ATH_CHECK( extractHistoPath(histfile,gsystpath, configdir,blank  ,  JETTRK) );//properly sets the paths
  
    TFile infile((configdir).c_str());

    jet_systRpt_pt_eta = dynamic_cast<TH2D*>( infile.Get("jet_systRpt_pt_eta") ) ;  

    if( !jet_systRpt_pt_eta)
      {
	ATH_MSG_ERROR("Could not get jet track histo from the config file:" << histfile);
	return StatusCode::FAILURE;
      }
    
    jet_systRpt_pt_eta->SetDirectory(0);
  
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ << "  DONE!!");
    return StatusCode::SUCCESS;
  }
  
  
  StatusCode METSystematicsTool::softTrkSystInitialize()
  {
    ATH_MSG_VERBOSE (__PRETTY_FUNCTION__ );
    
    std::string histfile  = "";
    std::string psystpath = "";
    std::string histpath = "";
    std::string suffix    = "";
    
    ATH_CHECK( extractHistoPath(histfile,psystpath,histpath, suffix, SOFTTRK) );//properly sets the paths
    
    TFile infile(histpath.c_str());
    
    resoperp_pthard_njet_mu  = dynamic_cast<TH3D*>( infile.Get((psystpath+"/resoperp_"+suffix).c_str()) );
    resopara_pthard_njet_mu  = dynamic_cast<TH3D*>( infile.Get((psystpath+"/resopara_"+suffix).c_str()) );
    shiftpara_pthard_njet_mu = dynamic_cast<TH3D*>( infile.Get((psystpath+"/shiftpara_"+suffix).c_str()));
    
    if( !(resoperp_pthard_njet_mu  &&
          resopara_pthard_njet_mu  &&
          shiftpara_pthard_njet_mu
          )
	)
      {
	ATH_MSG_ERROR("Could not get all track histos from the config file:" << histfile );
	return StatusCode::FAILURE;
      }
    
    resoperp_pthard_njet_mu ->SetDirectory(0);
    resopara_pthard_njet_mu ->SetDirectory(0);
    shiftpara_pthard_njet_mu->SetDirectory(0);

    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ <<"  DONE!!");
    return StatusCode::SUCCESS;
  }
  
  CP::SystematicCode METSystematicsTool::sysApplySystematicVariation(const CP::SystematicSet& systSet){//this should already be filtered for MET systematics
    ATH_MSG_VERBOSE (__PRETTY_FUNCTION__ );
    // Only a single systematic can be applied at a time:
    // If at some point we can deal with multiple systematics, we will check here that the combination we are given will work
    
    if( systSet.size()==0 ) {
      ATH_MSG_DEBUG("No affecting systematics received.");
      return CP::SystematicCode::Ok;
    } else if( systSet.size() > 1 ) {
      ATH_MSG_WARNING("Tool does not support multiple systematics, returning unsupported" );
      return CP::SystematicCode::Unsupported;
    }
    CP::SystematicVariation systVar = *systSet.begin();
    if     ( systVar == CP::SystematicVariation("") )           m_appliedSystEnum = NONE                  ;
    else if( systVar == softTrkAffSyst::MET_SoftTrk_ScaleUp)    m_appliedSystEnum = MET_SOFTTRK_SCALEUP   ;
    else if( systVar == softTrkAffSyst::MET_SoftTrk_ScaleDown)  m_appliedSystEnum = MET_SOFTTRK_SCALEDOWN ;
    else if( systVar == softTrkAffSyst::MET_SoftTrk_ResoPara)   m_appliedSystEnum = MET_SOFTTRK_RESOPARA  ;
    else if( systVar == softTrkAffSyst::MET_SoftTrk_ResoPerp)   m_appliedSystEnum = MET_SOFTTRK_RESOPERP  ;
    else if( systVar == softTrkAffSyst::MET_SoftTrk_ResoCorr)   m_appliedSystEnum = MET_SOFTTRK_RESOCORR  ;
    else if( systVar == softCaloAffSyst::MET_SoftCalo_ScaleUp)   m_appliedSystEnum = MET_SOFTCALO_SCALEUP  ;
    else if( systVar == softCaloAffSyst::MET_SoftCalo_ScaleDown) m_appliedSystEnum = MET_SOFTCALO_SCALEDOWN;
    else if( systVar == softCaloAffSyst::MET_SoftCalo_Reso)      m_appliedSystEnum = MET_SOFTCALO_RESO     ;
    else if( systVar == jetTrkAffSyst::MET_JetTrk_ScaleUp)     m_appliedSystEnum = MET_JETTRK_SCALEUP    ;
    else if( systVar == jetTrkAffSyst::MET_JetTrk_ScaleDown)   m_appliedSystEnum = MET_JETTRK_SCALEDOWN  ;
    else{
      ATH_MSG_WARNING("unsupported systematic applied " );
      return CP::SystematicCode::Unsupported;
    }
    
    ATH_MSG_DEBUG("applied systematic is " << m_appliedSystEnum );
    
    return CP::SystematicCode::Ok;
  }
  
  CP::CorrectionCode METSystematicsTool::applyCorrection(xAOD::MissingET& inputMet, 
							 const xAOD::MissingETAssociationMap * map,
							 const xAOD::JetContainer * jetCont) const{

    //if asking for jet track systematics, the user needs to give a met association map as well
    //if using a different jetContainer, you can give it as an option to applyCorrection
    ATH_MSG_VERBOSE (__PRETTY_FUNCTION__ );

    if( MissingETBase::Source::isSoftTerm(inputMet.source())){
      if( getDefaultEventInfo() == nullptr) {
	ATH_MSG_WARNING("event info is empty, returning without applying correction");
	return CP::CorrectionCode::Error;
      }
      xAOD::MissingETContainer const * METcont = dynamic_cast<xAOD::MissingETContainer const*>(inputMet.container());
      if(METcont == nullptr){
	ATH_MSG_WARNING("MissingET object not owned by a container. Returning without applying correction" );
	return CP::CorrectionCode::Error;
      }
      if(jetCont == nullptr  &&
	 evtStore()->retrieve(jetCont, m_jetColl).isFailure()
	 ){
        ATH_MSG_WARNING( "retrieval of jet container " << m_jetColl <<  " failed" );
        return CP::CorrectionCode::Error;
      }

      return internalSoftTermApplyCorrection(inputMet, METcont, *getDefaultEventInfo(), jetCont);
    }

    if( MissingETBase::Source::isTrackTerm(inputMet.source()) &&
	MissingETBase::Source::isJetTerm  (inputMet.source())
					    ){
      if( map == nullptr) {
	ATH_MSG_WARNING("To calculate jet track systematics, you must give applyCorrection a MissingETAssociationMap, as applyCorrection(inputMet, map).  Returning without applying correction ");
	return CP::CorrectionCode::Error;
      }

      if(jetCont == nullptr  && //if we get a JetContainer, we will use that one 
	 evtStore()->retrieve(jetCont, m_jetColl).isFailure()
	 ){
        ATH_MSG_WARNING( "retrieval of jet container " << m_jetColl <<  " failed" );
        return CP::CorrectionCode::Error;
      }

      return getCorrectedJetTrackMET(inputMet, map, jetCont);
    }

    ATH_MSG_ERROR("METSystematicsTool received a MissingET object it can't correct.  You should only pass soft MET terms or jet track MET terms.");
    return CP::CorrectionCode::Error;
  }
  
  
  CP::CorrectionCode METSystematicsTool::correctedCopy(const xAOD::MissingET& met, xAOD::MissingET*& outputmet, 
						       const xAOD::MissingETAssociationMap * map,
						       const xAOD::JetContainer * jetCont) const  
  { ATH_MSG_VERBOSE (__PRETTY_FUNCTION__ );
    xAOD::MissingET * copy = nullptr;

    if( MissingETBase::Source::isSoftTerm(met.source())){
      if( getDefaultEventInfo() == nullptr) {
	ATH_MSG_WARNING("event info is empty, returning empty with unknown source");
	outputmet = nullptr; 
	return CP::CorrectionCode::Error;
      }
      xAOD::MissingETContainer const * METcont = dynamic_cast<xAOD::MissingETContainer const*>(met.container());
      if(METcont == nullptr){
	ATH_MSG_WARNING("MissingET object not owned by a container. Unable to apply correction, returning output MET object as null" );
	outputmet = nullptr; 
	return CP::CorrectionCode::Error;
      }
      if(jetCont == nullptr &&
	 evtStore()->retrieve(jetCont, m_jetColl).isFailure()){
        ATH_MSG_WARNING( "retrieval of jet container " << m_jetColl <<  " failed" );
	outputmet = nullptr; 
        return CP::CorrectionCode::Error;
      }
      copy = new xAOD::MissingET(met);
      if(internalSoftTermApplyCorrection(*copy, METcont,  *getDefaultEventInfo(), jetCont) != CP::CorrectionCode::Ok ){
	outputmet = nullptr; delete copy;
	return CP::CorrectionCode::Error;
      }
    }//soft term source
    if( MissingETBase::Source::isTrackTerm(met.source()) &&
	MissingETBase::Source::isJetTerm  (met.source())
	){
      if( map == nullptr) {
	ATH_MSG_WARNING("To calculate jet track systematics, you must give correctedCopy a MissingETAssociationMap, as correctedCopy(inputMet, map).  "); 
	outputmet = nullptr; 
	return CP::CorrectionCode::Error;
      }

      if(jetCont == nullptr &&
	 evtStore()->retrieve(jetCont, m_jetColl).isFailure()){
        ATH_MSG_WARNING( "retrieval of jet container " << m_jetColl <<  " failed" );
	outputmet = nullptr; 
        return CP::CorrectionCode::Error;
      }
      copy = new xAOD::MissingET(met);
      if(getCorrectedJetTrackMET(*copy, map, jetCont) != CP::CorrectionCode::Ok){
	outputmet = nullptr; delete copy;
	return CP::CorrectionCode::Error;
      }
    }//jet track term source
   
    outputmet = copy;
    return CP::CorrectionCode::Ok;
  }

  CP::CorrectionCode METSystematicsTool::internalSoftTermApplyCorrection(xAOD::MissingET                & softMet,
									 xAOD::MissingETContainer const * METcont,
									 xAOD::EventInfo          const & eInfo  ,
									 xAOD::JetContainer       const * jetCont 
									 ) const{ //this is equivalent of "getSoftTerms"
    
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ );


    if( ! MissingETBase::Source::isSoftTerm(softMet.source()) ){
      ATH_MSG_ERROR("not soft met, cannot apply soft term correction to this MET"); 
      return CP::CorrectionCode::Error;
    }
    
    if(jetCont == nullptr) {
      ATH_MSG_WARNING("No jet container, cannot apply soft term correction");
      return CP::CorrectionCode::Error;
    }

    bool doSyst = false;
    if( MissingETBase::Source::isTrackTerm(softMet.source()) ) {
      doSyst = m_appliedSystEnum>=MET_SOFTTRK_SCALEUP && m_appliedSystEnum<=MET_SOFTTRK_RESOCORR;
    } else {
      doSyst = m_appliedSystEnum>=MET_SOFTCALO_SCALEUP && m_appliedSystEnum<=MET_SOFTCALO_RESO;
    }
    
    if(doSyst) {

      if(METcont == nullptr){
        ATH_MSG_WARNING("failed to retrieve MET container from passed object");
        return CP::CorrectionCode::Error;
      }
      
      xAOD::MissingET const ptHard = calcPtHard(METcont);//GeV?
      
      int          phbin                                     = shiftpara_pthard_njet_mu->GetXaxis()->FindBin(ptHard.met());
      if(phbin>shiftpara_pthard_njet_mu->GetNbinsX())  phbin = shiftpara_pthard_njet_mu->GetNbinsX();
      int    const jetbin                                    = shiftpara_pthard_njet_mu->GetYaxis()->FindBin(jetCont->size());
      int    const mubin                                     = shiftpara_pthard_njet_mu->GetZaxis()->FindBin(eInfo.actualInteractionsPerCrossing() );
      double const ptHardShift                               = shiftpara_pthard_njet_mu->GetBinContent(phbin,jetbin,mubin);
      
      double const randGaus = m_rand.Gaus(0.,1.);
      
      ATH_MSG_DEBUG("About to apply systematic " << appliedSystematicsString() );
      
      //now we need to know what soft term systematics we are doing
      //m_appliedSystEnum was cached by the applySystematicVariation method
      switch( m_appliedSystEnum ){
        case MET_SOFTTRK_SCALEUP : {
          softMet                = softTrkSyst_scale(softMet, ptHard,  ptHardShift);
          break;
        }
        case MET_SOFTTRK_SCALEDOWN: {
          softMet                = softTrkSyst_scale(softMet, ptHard,-1*ptHardShift);
          break;
        }
        case MET_SOFTTRK_RESOPARA : {
          double const smearpara = resopara_pthard_njet_mu->GetBinContent(phbin,jetbin,mubin)*randGaus;
          softMet                = softTrkSyst_reso(softMet, ptHard, ptHardShift, smearpara, 0.);
          break;
        }
        case MET_SOFTTRK_RESOPERP : {
          double const smearperp = resoperp_pthard_njet_mu->GetBinContent(phbin,jetbin,mubin)*randGaus;
          softMet                = softTrkSyst_reso(softMet, ptHard, ptHardShift, 0., smearperp );
          break;
        }
        case MET_SOFTTRK_RESOCORR : {
          double const smearpara = resopara_pthard_njet_mu->GetBinContent(phbin,jetbin,mubin)*randGaus;
          double const smearperp = resoperp_pthard_njet_mu->GetBinContent(phbin,jetbin,mubin)*randGaus;
          softMet                = softTrkSyst_reso(softMet, ptHard, ptHardShift, smearpara   , smearperp);
          break;
        }
        case MET_SOFTCALO_SCALEUP : {
          double const caloscale = 1. +  h_calosyst_scale->GetBinContent(1);
          softMet                = caloSyst_scale(softMet,caloscale);
          break;
        }
        case MET_SOFTCALO_SCALEDOWN : {
          double const caloscale = 1. - h_calosyst_scale->GetBinContent(1);
          softMet                = caloSyst_scale(softMet,caloscale);
          break;
        }
        case MET_SOFTCALO_RESO  : {
          softMet                = caloSyst_reso(softMet) ;
          break;
        }
        default:{
          ATH_MSG_DEBUG("No systematic applied, returning nominal MET term");
        }
      }
    } else {
      ATH_MSG_DEBUG("Ignore irrelevant systematic.");
    }

    return CP::CorrectionCode::Ok;
  }

  CP::CorrectionCode METSystematicsTool::calcJetTrackMETWithSyst(xAOD::MissingET& jettrkmet, 
  								 const xAOD::MissingETAssociationMap* map,
 								 const xAOD::Jet* jet) const

  {
    ATH_MSG_VERBOSE(__PRETTY_FUNCTION__);
    
    if( jet_systRpt_pt_eta == nullptr ) {
      ATH_MSG_ERROR("jet track systematics histogram not initialized properly.") ;
      return CP::CorrectionCode::Error;
    }
    xAOD::MissingETAssociation const * const assoc = MissingETComposition::getAssociation(map,jet);
    MissingETBase::Types::constvec_t trkvec = assoc->overlapTrkVec();
    //    MissingETBase::Types::constvec_t trkvec = assoc->jetTrkVec();

    int         phbin  = jet_systRpt_pt_eta->GetXaxis()->FindBin(jet->pt()/1e3);
    if(phbin>jet_systRpt_pt_eta->GetNbinsX())  phbin  = jet_systRpt_pt_eta->GetNbinsX();
    
    int         etabin  = jet_systRpt_pt_eta->GetYaxis()->FindBin(fabs( jet->eta()  ));
    if(etabin>jet_systRpt_pt_eta->GetNbinsY()) etabin = jet_systRpt_pt_eta->GetNbinsY();

    double uncert = 0.;
    switch( m_appliedSystEnum ){
    
    case MET_JETTRK_SCALEUP : {
      uncert  = jet_systRpt_pt_eta->GetBinContent(phbin,etabin);
      break;
    }
    case MET_JETTRK_SCALEDOWN : {
      uncert  = -1.*jet_systRpt_pt_eta->GetBinContent(phbin,etabin);
      break;
    }
    default:{
      ATH_MSG_DEBUG("No jet track systematic applied");
      break;
    }
    }

    jettrkmet.setMpx  ( jettrkmet.mpx() - trkvec.cpx() * uncert);
    jettrkmet.setMpy  ( jettrkmet.mpy() - trkvec.cpy() * uncert);
    
    return CP::CorrectionCode::Ok;
  }
 
  CP::CorrectionCode METSystematicsTool::getCorrectedJetTrackMET(xAOD::MissingET& jettrkmet,
  								 const xAOD::MissingETAssociationMap* map,
  								 const xAOD::JetContainer * jetCont
  								 ) const 
   { 
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ );
    if(!map) {
      ATH_MSG_ERROR("MissingETAssociationMap null, error calculating jet track systematics.");
      return CP::CorrectionCode::Error;
    }
    if(!jetCont) {
      ATH_MSG_ERROR("JetContainer null, error calculating jet track systematics.");
      return CP::CorrectionCode::Error;
    }

    for(xAOD::JetContainer::const_iterator iJet=jetCont->begin(); iJet!=jetCont->end(); ++iJet ){
      if(calcJetTrackMETWithSyst(jettrkmet, map , *iJet) != CP::CorrectionCode::Ok){
	ATH_MSG_ERROR("Failed to calculate jet track systematics.  There was a problem with the " << iJet - jetCont->begin() << "th jet in your container."); 
	return CP::CorrectionCode::Error;
      }
    }
    return CP::CorrectionCode::Ok;
   }

  xAOD::MissingET METSystematicsTool::caloSyst_scale(xAOD::MissingET const &softTerms, double const scale) const{
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ );
    return xAOD::MissingET(softTerms.mpx()*scale, softTerms.mpy()*scale, softTerms.sumet(),
                           softTerms.name(),softTerms.source());
  }
  
  xAOD::MissingET METSystematicsTool::caloSyst_reso(xAOD::MissingET const &softTerms) const {
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ );
    ATH_MSG_VERBOSE("caloSyst_reso: input MET: " << softTerms.met());
    
    double const metSigma     = .7 * sqrt(softTerms.sumet());
    double const resUnc       =  h_calosyst_reso->GetBinContent(1);
    double const smearedSigma = sqrt( (metSigma* (1. + resUnc))*(metSigma* (1. + resUnc)) -
				      metSigma * metSigma );

    ATH_MSG_VERBOSE("caloSyst_reso: metSigma: " << metSigma << ", resUnc: " << resUnc << ", smearedSigma = " << smearedSigma);
    
    double const rand  = gRandom->Gaus(0.,1.);
    double const shift = softTerms.met()<1e-9 ? 0. : rand*smearedSigma / softTerms.met();
    
    ATH_MSG_VERBOSE("caloSyst_reso: shift = " << shift);

    return xAOD::MissingET(softTerms.mpx()*(1.+shift),softTerms.mpy()*(1.+shift),softTerms.sumet(),
                           softTerms.name(),softTerms.source());
  }
  
  xAOD::MissingET METSystematicsTool::softTrkSyst_scale(xAOD::MissingET const &softTerms, xAOD::MissingET const &ptHard, double const shift) const
  {  ATH_MSG_VERBOSE(__PRETTY_FUNCTION__);
    return softTrkSyst_reso(softTerms, ptHard, shift, 0. , 0.);
  }
  
  xAOD::MissingET METSystematicsTool::softTrkSyst_reso(xAOD::MissingET const &softTerms,
                                                       xAOD::MissingET const &ptHard,
                                                       double const shift,
                                                       double const smearpara,
                                                       double const smearperp) const{
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ );
    
    xAOD::MissingET projection = projectST(softTerms,ptHard);
    projection.setMpx(projection.mpx() + shift + smearpara );
    projection.setMpy(projection.mpy() +       + smearperp );
    
    return projectST(projection, ptHard);
  }
  
  xAOD::MissingET METSystematicsTool::projectST(xAOD::MissingET const &softTerms, xAOD::MissingET const &ptHard) const
  {
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ );
    
    double const ux    = ptHard.mpx()/ptHard.met();
    double const uy    = ptHard.mpy()/ptHard.met();
    double const projL = ux*softTerms.mpx() + uy*softTerms.mpy();
    double const projT = uy*softTerms.mpx() - ux*softTerms.mpy();
    xAOD::MissingET proj(projL,projT,softTerms.sumet(),
                         softTerms.name(),softTerms.source());
    
    return proj;
  }
  
  xAOD::MissingET METSystematicsTool::calcPtHard(xAOD::MissingETContainer const * const cont) const
  {
    ATH_MSG_VERBOSE(__PRETTY_FUNCTION__ );
    
    //get truth container
    xAOD::MissingETContainer const * truthCont = nullptr;
    if(evtStore()->retrieve(truthCont, m_truthCont).isFailure()){
      ATH_MSG_ERROR( m_truthCont << " container empty or does not exist, calcPtHard returning zero.");
      return xAOD::MissingET(0.,0.,0.);
    }
    
    xAOD::MissingET const * truthptr = (*truthCont)[MissingETBase::Source::truthNonInt()];
    if(truthptr == nullptr){
      ATH_MSG_ERROR( m_truthObj << " is not in " << m_truthCont << ". calcPtHard returing zero." );
      return xAOD::MissingET(0.,0.,0.);
    }
    
    xAOD::MissingET ptHard = (*truthptr);
    
    //loop over all non soft or total terms
    for(xAOD::MissingETContainer::const_iterator iMET=cont->begin();iMET!=cont->end(); ++iMET  ) {
      if( !( MissingETBase::Source::isSoftTerm( (*iMET)->source() ) ||
            MissingETBase::Source::isTotalTerm( (*iMET)->source() ) ) ) {
        ptHard -= **iMET;
      }
    }
    
    ptHard *= 1./1000. ;
    
    ATH_MSG_DEBUG("PtHard value is " << std::to_string(ptHard.met()) );
    
    return ptHard;
  }
  
  StatusCode METSystematicsTool::extractHistoPath(std::string & histfile ,
                                                  std::string & systpath ,
                                                  std::string & histpath,
                                                  std::string & suffix   ,
                                                  SystType const & type
                                                  )
  {
    ATH_MSG_VERBOSE (__PRETTY_FUNCTION__);
    TEnv reader;
    
    std::string configpath = "";
    std::string configfile = "";
    
    switch(type){
    case SOFTCALO   :  configfile = m_configSoftCaloFile;
      configpath  = PathResolver::find_file(m_configPrefix+m_configSoftCaloFile, "CALIBPATH", PathResolver::RecursiveSearch) ;
        break;
    case SOFTTRK    :  configfile = m_configSoftTrkFile;
        configpath  = PathResolver::find_file(m_configPrefix+m_configSoftTrkFile, "CALIBPATH", PathResolver::RecursiveSearch) ;
        break;
    case JETTRK :   configfile = m_configJetTrkFile;
      configpath = PathResolver::find_file(m_configPrefix+m_configJetTrkFile, "CALIBPATH", PathResolver::RecursiveSearch) ;
      break;
    default     :  configpath = "";
    }
    
    ATH_MSG_INFO( "Searching for configFile: " << configfile);
    ATH_MSG_INFO( "PWD: "        << gSystem->Getenv("PWD")  )  ;
    ATH_MSG_INFO( "CALIBPATH: "  << gSystem->Getenv("CALIBPATH") );
     
    if(configpath.empty() || configfile.empty() ){
      ATH_MSG_ERROR( "Path Resolver couldn't find config file");
      return StatusCode::FAILURE;
    }
    
    //    boost::filesystem::path configpathBf(configpath);
    //    configdir = configpathBf.parent_path().string();
   
    if( reader.ReadFile(configpath.c_str(),EEnvLevel(0)) < 0) {
      ATH_MSG_ERROR( "Couldn't read the track config file!" );
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_INFO( "Configuring from file " << configpath	 );


    switch(type){
      case SOFTCALO   :   
	histfile =  reader.GetValue( "Conf.InputFile" , "");
	systpath =  reader.GetValue( "GlobalSyst.sourcedir" , "" );
	break;
      case SOFTTRK    :   
	histfile = reader.GetValue( "Conf.InputFile" , "");
        systpath = reader.GetValue( "PtHardSyst.sourcedir" , "" );
        suffix   = reader.GetValue( "PtHardSyst.suffix"    , "" );
        break;
      case JETTRK :
	histfile = reader.GetValue( "JetTrkSyst.InputFile" , "");
	systpath  = "/";
	break;
      default     :  break;
    }

    ATH_MSG_INFO( "Will read histograms from " << histfile );
    
    histpath = PathResolver::find_file(histfile, "CALIBPATH", PathResolver::RecursiveSearch) ;
    ATH_MSG_INFO("Extracted histogram path " << histpath);
    
    if(histfile.empty() || systpath.empty() || histpath.empty() ){
      ATH_MSG_ERROR("Failed to correctly set histfile path, or path to histograms inside of the histfile" );
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  
  //stolen from JetUncertainties
  xAOD::EventInfo const * METSystematicsTool::getDefaultEventInfo() const
  {   ATH_MSG_VERBOSE (__PRETTY_FUNCTION__ );
    xAOD::EventInfo const * eInfoConst = nullptr;
 
    if (evtStore()->retrieve(eInfoConst ,m_eventInfo).isFailure()){
      ATH_MSG_ERROR("Failed to retrieve default EventInfo object");
    }

    return eInfoConst;
  }
  
  int METSystematicsTool::getNPV() const{
    ATH_MSG_VERBOSE (__PRETTY_FUNCTION__ );
    const xAOD::VertexContainer* vertices = nullptr;
    
    if (evtStore()->retrieve(vertices,m_vertexCont).isFailure()){
      ATH_MSG_ERROR("Failed to retrieve default NPV value from PrimaryVertices");
      return 0;
    }
    
    int NPV = 0;
    xAOD::VertexContainer::const_iterator itr;
    for (itr = vertices->begin(); itr != vertices->end(); ++itr)
      if ( (*itr)->vertexType()  != 0) NPV++;
    
    return NPV;
  }
  
  void METSystematicsTool::setRandomSeed(int seed){
    ATH_MSG_VERBOSE(__PRETTY_FUNCTION__);
    m_rand.SetSeed(seed);
  }
}

//  LocalWords:  SOFTTRK
