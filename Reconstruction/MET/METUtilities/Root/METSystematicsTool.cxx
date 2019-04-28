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
#include "xAODJet/Jet.h"

namespace met {

  using namespace xAOD;

  typedef ElementLink<xAOD::IParticleContainer> iplink_t;
  static const SG::AuxElement::ConstAccessor< std::vector<iplink_t > > acc_constitObjLinks("ConstitObjectLinks");
  static const SG::AuxElement::ConstAccessor< iplink_t  > acc_originalObject("originalObjectLink");

  //  const static MissingETBase::Types::bitmask_t invisSource = 0x100000; // doesn't overlap with any other

  METSystematicsTool::METSystematicsTool(const std::string& name) : asg::AsgTool::AsgTool(name),
  m_appliedSystEnum(NONE),
  m_shiftpara_pthard_njet_mu(nullptr),
  m_resopara_pthard_njet_mu(nullptr),
  m_resoperp_pthard_njet_mu(nullptr),
  m_jet_systRpt_pt_eta(nullptr),
  m_h_calosyst_scale(nullptr),
  m_h_calosyst_reso (nullptr),
  m_rand(0),
  m_units(-1)
  {
    ATH_MSG_DEBUG (__PRETTY_FUNCTION__ );

    declareProperty( "ConfigPrefix",      m_configPrefix      = "METUtilities/data17_13TeV/prerec_Jan16");
    declareProperty( "ConfigSoftTrkFile", m_configSoftTrkFile = "TrackSoftTerms.config"                     );
    //    declareProperty( "ConfigSoftTrkFile", m_configSoftTrkFile = "TrackSoftTerms_afii.config"            );//for ATLFAST
    declareProperty( "ConfigJetTrkFile",  m_configJetTrkFile  = ""                                          );
    declareProperty( "ConfigSoftCaloFile",m_configSoftCaloFile= ""                                          );
    // declareProperty( "ConfigSoftCaloFile",m_configSoftCaloFile= "METRefFinal_Obsolete2012_V2.config"        );
    declareProperty( "TruthContainer",    m_truthCont         = "MET_Truth"                                 );
    declareProperty( "TruthObj",          m_truthObj          = "NonInt"                                    );
    declareProperty( "VertexContainer",   m_vertexCont        = "PrimaryVertices"                           );
    declareProperty( "EventInfo",         m_eventInfo         = "EventInfo"                                 );
    declareProperty( "UseDevArea",        m_useDevArea        = false                                       );

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

    delete m_shiftpara_pthard_njet_mu;
    delete m_resopara_pthard_njet_mu;
    delete m_resoperp_pthard_njet_mu;
    delete m_jet_systRpt_pt_eta;
    delete m_h_calosyst_scale;
    delete m_h_calosyst_reso;

    return StatusCode::SUCCESS;
  }

  StatusCode METSystematicsTool::softCaloSystInitialize()
  {
    ATH_MSG_VERBOSE (__PRETTY_FUNCTION__);
    ATH_MSG_INFO("Doing SoftCalo systematics initialization.  THIS IS FOR SOFTWARE DEVELOPMENT ONLY.");
    ATH_MSG_INFO("CST IS NOT YET RECOMMENDED OR SUPPORTED BY THE MET GROUP.  YOU ARE USING A 2012 config file.");

    std::string histfile  = "";
    std::string gsystpath = "";
    std::string histpath = "";
    std::string blank     = "";

    ATH_CHECK( extractHistoPath(histfile,gsystpath,histpath,blank ,SOFTCALO) );//properly sets the paths

    TFile infile(histpath.c_str());

    ATH_MSG_INFO( "METSystematics: Read calo uncertainties" );
    m_h_calosyst_scale = dynamic_cast<TH1D*>( infile.Get((gsystpath+"/globsyst_scale").c_str()));
    m_h_calosyst_reso  = dynamic_cast<TH1D*>( infile.Get((gsystpath+"/globsyst_reso").c_str()));

    if( !(m_h_calosyst_scale &&
          m_h_calosyst_reso
          )
	)
      {
	ATH_MSG_ERROR("Could not get all calo histos from the config file:" << histfile);
	return StatusCode::FAILURE;
      }

    m_h_calosyst_scale->SetDirectory(0);
    m_h_calosyst_reso ->SetDirectory(0);

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

    m_jet_systRpt_pt_eta = dynamic_cast<TH2D*>( infile.Get("jet_systRpt_pt_eta") ) ;
//    m_jet_systRpt_pt_eta = dynamic_cast<TH2D*>( infile.Get("uncertaintyMap") ) ;

    if( !m_jet_systRpt_pt_eta)
      {
	ATH_MSG_ERROR("Could not get jet track histo from the config file:" << histfile);
	return StatusCode::FAILURE;
      }

    m_jet_systRpt_pt_eta->SetDirectory(0);

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

    m_resoperp_pthard_njet_mu  = dynamic_cast<TH3D*>( infile.Get((psystpath+"/resoperp_"+suffix).c_str()) );
    m_resopara_pthard_njet_mu  = dynamic_cast<TH3D*>( infile.Get((psystpath+"/resopara_"+suffix).c_str()) );
    m_shiftpara_pthard_njet_mu = dynamic_cast<TH3D*>( infile.Get((psystpath+"/shiftpara_"+suffix).c_str()));

    if( !(m_resoperp_pthard_njet_mu  &&
          m_resopara_pthard_njet_mu  &&
          m_shiftpara_pthard_njet_mu
          )
	)
      {
	ATH_MSG_ERROR("Could not get all track histos from the config file:" << histfile );
	return StatusCode::FAILURE;
      }

    m_resoperp_pthard_njet_mu ->SetDirectory(0);
    m_resopara_pthard_njet_mu ->SetDirectory(0);
    m_shiftpara_pthard_njet_mu->SetDirectory(0);

    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ <<"  DONE!!");
    return StatusCode::SUCCESS;
  }

  CP::SystematicCode METSystematicsTool::sysApplySystematicVariation(const CP::SystematicSet& systSet){//this should already be filtered for MET systematics
    ATH_MSG_VERBOSE (__PRETTY_FUNCTION__ );
    // Only a single systematic can be applied at a time:
    // If at some point we can deal with multiple systematics, we will check here that the combination we are given will work

    m_appliedSystEnum = NONE;
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
							 const xAOD::MissingETAssociationMap * map) const{

    //if asking for jet track systematics, the user needs to give a met association map as well
    //if using a different jetContainer, you can give it as an option to applyCorrection
    ATH_MSG_VERBOSE (__PRETTY_FUNCTION__ );
    if( getDefaultEventInfo() == nullptr) {
	ATH_MSG_WARNING("event info is empty, returning without applying correction");
	return CP::CorrectionCode::Error;
    }

    if(! getDefaultEventInfo()->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
      ATH_MSG_WARNING("This tool should be applied to MC only.  Return without changing the input MET object");
      return CP::CorrectionCode::Error;
    }

    if( MissingETBase::Source::isSoftTerm(inputMet.source())){

      xAOD::MissingETContainer const * METcont = dynamic_cast<xAOD::MissingETContainer const*>(inputMet.container());
      if(METcont == nullptr){
	ATH_MSG_WARNING("MissingET object not owned by a container. Returning without applying correction" );
	return CP::CorrectionCode::Error;
      }

      return internalSoftTermApplyCorrection(inputMet, METcont, *getDefaultEventInfo());
    }

  //  if( MissingETBase::Source::isTrackTerm(inputMet.source()) &&
    if(MissingETBase::Source::isJetTerm  (inputMet.source())
					    ){
      if( map == nullptr) {
	ATH_MSG_WARNING("To calculate jet track systematics, you must give applyCorrection a MissingETAssociationMap, as applyCorrection(inputMet, map).  Returning without applying correction ");
	return CP::CorrectionCode::Error;
      }

      return getCorrectedJetTrackMET(inputMet, map);
    }

    ATH_MSG_WARNING("METSystematicsTool received a MissingET object it can't correct.  You should only pass soft MET terms or jet track MET terms.");
    return CP::CorrectionCode::Error;
  }


  CP::CorrectionCode METSystematicsTool::correctedCopy(const xAOD::MissingET& met, xAOD::MissingET*& outputmet,
						       const xAOD::MissingETAssociationMap * map) const
  { ATH_MSG_VERBOSE (__PRETTY_FUNCTION__ );
    xAOD::MissingET * copy = nullptr;

    if(outputmet != nullptr ){
      ATH_MSG_WARNING("Please pass a nullptr to the 2nd argument of correctedCopy to fill the output pointer");
      ATH_MSG_WARNING("Returning without making a correctedCopy");
      return CP::CorrectionCode::Error;
    }

    if( getDefaultEventInfo() == nullptr) {
      ATH_MSG_WARNING("event info is empty, return a nullptr");
      outputmet = nullptr;
      return CP::CorrectionCode::Error;
    }

    if(! getDefaultEventInfo()->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
      ATH_MSG_WARNING("This tool should be applied to MC only.  Return without changing the input MET object");
      outputmet = nullptr;
      return CP::CorrectionCode::Error;
    }

    if( MissingETBase::Source::isSoftTerm(met.source())){
      xAOD::MissingETContainer const * METcont = dynamic_cast<xAOD::MissingETContainer const*>(met.container());
      if(METcont == nullptr){
	ATH_MSG_WARNING("MissingET object not owned by a container. Unable to apply correction, returning output MET object as null" );
	outputmet = nullptr;
	return CP::CorrectionCode::Error;
      }
      copy = new xAOD::MissingET(met);

      if(internalSoftTermApplyCorrection(*copy, METcont,  *getDefaultEventInfo()) != CP::CorrectionCode::Ok ){
	outputmet = nullptr; delete copy;
	return CP::CorrectionCode::Error;
      }
    }//soft term source
    if( //MissingETBase::Source::isTrackTerm(met.source()) &&
	MissingETBase::Source::isJetTerm  (met.source())
	){
      if( map == nullptr) {
	ATH_MSG_WARNING("To calculate jet track systematics, you must give correctedCopy a MissingETAssociationMap, as correctedCopy(inputMet, map).  ");
	outputmet = nullptr;
	return CP::CorrectionCode::Error;
      }

      copy = new xAOD::MissingET(met);
      if(getCorrectedJetTrackMET(*copy, map) != CP::CorrectionCode::Ok){
	outputmet = nullptr; delete copy;
	return CP::CorrectionCode::Error;
      }
    }//jet track term source

    outputmet = copy;
    return CP::CorrectionCode::Ok;
  }

  CP::CorrectionCode METSystematicsTool::internalSoftTermApplyCorrection(xAOD::MissingET                & softMet,
									 xAOD::MissingETContainer const * METcont,
									 xAOD::EventInfo          const & eInfo
									 ) const{ //this is equivalent of "getSoftTerms"

    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ );


    if( ! MissingETBase::Source::isSoftTerm(softMet.source()) ){
      ATH_MSG_ERROR("not soft met, cannot apply soft term correction to this MET");
      return CP::CorrectionCode::Error;
    }

    bool doSyst = false;
    if( MissingETBase::Source::isTrackTerm(softMet.source()) ) {
      doSyst = m_appliedSystEnum>=MET_SOFTTRK_SCALEUP && m_appliedSystEnum<=MET_SOFTTRK_RESOCORR;
      ATH_MSG_VERBOSE("Track soft term " << softMet.name()
		   << ", met = " << softMet.met()
		   << ", applied systematic is " << m_appliedSystEnum
		   << ", do syst? " << doSyst);
    } else {
      doSyst = m_appliedSystEnum>=MET_SOFTCALO_SCALEUP && m_appliedSystEnum<=MET_SOFTCALO_RESO;
      ATH_MSG_VERBOSE("Calo soft term " << softMet.name()
		   << ", met = " << softMet.met()
		   << ", applied systematic is " << m_appliedSystEnum
		   << ", do syst? " << doSyst);
    }

    if(doSyst) {

      //this is for speed
      //we avoid creating met private stores here
      missingEt softMetStruct;
      softMetStruct.mpx = softMet.mpx();
      softMetStruct.mpy = softMet.mpy();
      softMetStruct.sumet = softMet.sumet();
      softMetStruct.name = softMet.name();
      softMetStruct.source = softMet.source();

      if(METcont == nullptr){
        ATH_MSG_WARNING("failed to retrieve MET container from passed object");
        return CP::CorrectionCode::Error;
      }

      missingEt const ptHard = calcPtHard(METcont);
      double const ptHardMet = TMath::Sqrt( ptHard.mpx * ptHard.mpx +
					    ptHard.mpy * ptHard.mpy )  ;

      xAOD::MissingET* jetterm = *METcont->find( MissingETBase::Source::jet() );
      size_t njet = (jetterm==nullptr) ? 0 : acc_constitObjLinks(*jetterm ).size();

      int          phbin                                     = m_shiftpara_pthard_njet_mu->GetXaxis()->FindBin( ptHardMet  ) ;
      if(phbin>m_shiftpara_pthard_njet_mu->GetNbinsX())  phbin = m_shiftpara_pthard_njet_mu->GetNbinsX();
      int    const jetbin                                    = m_shiftpara_pthard_njet_mu->GetYaxis()->FindBin(njet);
      int    const mubin                                     = m_shiftpara_pthard_njet_mu->GetZaxis()->FindBin(eInfo.actualInteractionsPerCrossing() );
      double const ptHardShift                               = m_shiftpara_pthard_njet_mu->GetBinContent(phbin,jetbin,mubin);

      double const randGaus = m_rand.Gaus(0.,1.);

      ATH_MSG_DEBUG("About to apply systematic " << appliedSystematicsString() );

      //now we need to know what soft term systematics we are doing
      //m_appliedSystEnum was cached by the applySystematicVariation method
      switch( m_appliedSystEnum ){
        case MET_SOFTTRK_SCALEUP : {
          softMetStruct                = softTrkSyst_scale(softMetStruct, ptHard,  ptHardShift);
          break;
        }
        case MET_SOFTTRK_SCALEDOWN: {
          softMetStruct                = softTrkSyst_scale(softMetStruct, ptHard,-1*ptHardShift);
          break;
        }
        case MET_SOFTTRK_RESOPARA : {
          double const smearpara = m_resopara_pthard_njet_mu->GetBinContent(phbin,jetbin,mubin)*randGaus;
          softMetStruct                = softTrkSyst_reso(softMetStruct, ptHard, ptHardShift, smearpara, 0.);
          break;
        }
        case MET_SOFTTRK_RESOPERP : {
          double const smearperp = m_resoperp_pthard_njet_mu->GetBinContent(phbin,jetbin,mubin)*randGaus;
          softMetStruct                = softTrkSyst_reso(softMetStruct, ptHard, ptHardShift, 0., smearperp );
          break;
        }
        case MET_SOFTTRK_RESOCORR : {
          double const smearpara = m_resopara_pthard_njet_mu->GetBinContent(phbin,jetbin,mubin)*randGaus;
          double const smearperp = m_resoperp_pthard_njet_mu->GetBinContent(phbin,jetbin,mubin)*randGaus;
          softMetStruct                = softTrkSyst_reso(softMetStruct, ptHard, ptHardShift, smearpara   , smearperp);
          break;
        }
        case MET_SOFTCALO_SCALEUP : {
          double const caloscale = 1. +  m_h_calosyst_scale->GetBinContent(1);
          softMetStruct                = caloSyst_scale(softMetStruct,caloscale);
          break;
        }
        case MET_SOFTCALO_SCALEDOWN : {
          double const caloscale = 1. - m_h_calosyst_scale->GetBinContent(1);
          softMetStruct                = caloSyst_scale(softMetStruct,caloscale);
          break;
        }
        case MET_SOFTCALO_RESO  : {
          softMetStruct                = caloSyst_reso(softMetStruct) ;
          break;
        }
        default:{
          ATH_MSG_DEBUG("No systematic applied, returning nominal MET term");
        }
      }

      //fill the softMet back with the struct values to return
      softMet.setMpx(    softMetStruct.mpx);
      softMet.setMpy(    softMetStruct.mpy);
      softMet.setSumet(  softMetStruct.sumet);
      softMet.setName(   softMetStruct.name);
      softMet.setSource( softMetStruct.source);
    } else {
      ATH_MSG_DEBUG("Ignore irrelevant systematic.");
    }

    ATH_MSG_VERBOSE("Output soft term " << softMet.name()
		 << ", met = " << softMet.met() );

    return CP::CorrectionCode::Ok;
  }

  CP::CorrectionCode METSystematicsTool::calcJetTrackMETWithSyst(xAOD::MissingET& jettrkmet,
  								 const xAOD::MissingETAssociationMap* map,
 								 const xAOD::Jet* jet) const

  {
    ATH_MSG_VERBOSE(__PRETTY_FUNCTION__);

    if( m_jet_systRpt_pt_eta == nullptr ) {
      ATH_MSG_ERROR("jet track systematics histogram not initialized properly.") ;
      return CP::CorrectionCode::Error;
    }

    if(m_appliedSystEnum==MET_JETTRK_SCALEUP || m_appliedSystEnum==MET_JETTRK_SCALEDOWN) {
      xAOD::MissingETAssociation const * const assoc = MissingETComposition::getAssociation(map,jet);
      MissingETBase::Types::constvec_t trkvec = assoc->overlapTrkVec();

      int phbin  = m_jet_systRpt_pt_eta->GetXaxis()->FindBin(jet->pt()/1e3);
      if(phbin>m_jet_systRpt_pt_eta->GetNbinsX())  phbin  = m_jet_systRpt_pt_eta->GetNbinsX();

      int etabin  = m_jet_systRpt_pt_eta->GetYaxis()->FindBin(fabs( jet->eta()  ));
      if(etabin>m_jet_systRpt_pt_eta->GetNbinsY()) etabin = m_jet_systRpt_pt_eta->GetNbinsY();

      double uncert = 0.;
      switch( m_appliedSystEnum ){
      case MET_JETTRK_SCALEUP : {
	uncert  = m_jet_systRpt_pt_eta->GetBinContent(phbin,etabin);
	break;
      }
      case MET_JETTRK_SCALEDOWN : {
	uncert  = -1.*m_jet_systRpt_pt_eta->GetBinContent(phbin,etabin);
	break;
      }
      default:
	break;
      }

      ATH_MSG_VERBOSE("Uncertainty on this jet is " << uncert);

      jettrkmet.setMpx  ( jettrkmet.mpx() - trkvec.cpx() * uncert);
      jettrkmet.setMpy  ( jettrkmet.mpy() - trkvec.cpy() * uncert);
    } else { ATH_MSG_VERBOSE("No jet track systematics applied"); }

    return CP::CorrectionCode::Ok;
  }

  CP::CorrectionCode METSystematicsTool::calcJetTrackMETWithSyst(xAOD::MissingET& jettrkmet,
  								 const xAOD::MissingETAssociationMap* map) const

  {
    ATH_MSG_VERBOSE(__PRETTY_FUNCTION__);

    if( m_jet_systRpt_pt_eta == nullptr ) {
      ATH_MSG_ERROR("jet track systematics histogram not initialized properly.") ;
      return CP::CorrectionCode::Error;
    }

    if(m_appliedSystEnum==MET_JETTRK_SCALEUP || m_appliedSystEnum==MET_JETTRK_SCALEDOWN) {
      double uncert = 0.;
      int jetCount=0;

      std::vector<const xAOD::Jet*> jets;
      for(iplink_t jetlink : acc_constitObjLinks(jettrkmet)) {
	if((*jetlink)->type()!=xAOD::Type::Jet) {
	  ATH_MSG_ERROR("Invalid object of type " << (*jetlink)->type() << " in jet term");
	  return CP::CorrectionCode::Error;
	}
	jets.push_back(static_cast<const xAOD::Jet*>(*jetlink));
      }
      bool originalInputs = jets.empty() ? false : !acc_originalObject.isAvailable(*jets.front());
      for(const xAOD::Jet *jet : jets) {
	const MissingETAssociation* assoc = 0;
	if(originalInputs) {
	  assoc = MissingETComposition::getAssociation(map,jet);
	} else {
	  const IParticle* orig = *acc_originalObject(*jet);
	  assoc = MissingETComposition::getAssociation(map,static_cast<const xAOD::Jet*>(orig));
	}

	MissingETBase::Types::constvec_t trkvec = assoc->jetTrkVec();
	if(fabs(jet->eta())<=2.5)
	  {
	    jetCount++;
	    int         phbin  = m_jet_systRpt_pt_eta->GetXaxis()->FindBin(jet->pt()/1e3);
	    if(phbin>m_jet_systRpt_pt_eta->GetNbinsX())  phbin  = m_jet_systRpt_pt_eta->GetNbinsX();

	    int         etabin  = m_jet_systRpt_pt_eta->GetYaxis()->FindBin(fabs( jet->eta()  ));
	    if(etabin>m_jet_systRpt_pt_eta->GetNbinsY()) etabin = m_jet_systRpt_pt_eta->GetNbinsY();
	    float uncert_frac=(trkvec.sumpt())*(m_jet_systRpt_pt_eta->GetBinContent(phbin, etabin));

	    ATH_MSG_VERBOSE("Sumpt: "<< trkvec.sumpt());
	    ATH_MSG_VERBOSE("jet uncert: "<< m_jet_systRpt_pt_eta->GetBinContent(phbin, etabin));
	    uncert=sqrt(uncert*uncert+uncert_frac*uncert_frac);
	  }
      }

      ATH_MSG_VERBOSE("Uncertainty: "<< uncert);
      ATH_MSG_VERBOSE("Jet Counting: "<< jetCount);

      switch( m_appliedSystEnum ) {
      case MET_JETTRK_SCALEUP :
	{
	  break;
	}
      case MET_JETTRK_SCALEDOWN :
	{
	  uncert  = -1.*uncert;
	  break;
	}
      default:
	break;
	}

      if(fabs(jettrkmet.mpx())>0.0)
	jettrkmet.setMpx  ( jettrkmet.mpx()*(1 + uncert/(fabs(jettrkmet.mpx())*sqrt(2))));
      else jettrkmet.setMpx (0.0);
      if(fabs(jettrkmet.mpy())>0.0)
	jettrkmet.setMpy  ( jettrkmet.mpy()*(1 + uncert/(fabs(jettrkmet.mpy())*sqrt(2))));
      else jettrkmet.setMpy (0.0);
      jettrkmet.setSumet( jettrkmet.sumet() + uncert);
    }

    return CP::CorrectionCode::Ok;
  }


  CP::CorrectionCode METSystematicsTool::getCorrectedJetTrackMET(xAOD::MissingET& jettrkmet,
  								 const xAOD::MissingETAssociationMap* map
  								 ) const
   {
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ );
    if(!map) {
      ATH_MSG_ERROR("MissingETAssociationMap null, error calculating jet track systematics.");
      return CP::CorrectionCode::Error;
    }

    if(calcJetTrackMETWithSyst(jettrkmet, map) != CP::CorrectionCode::Ok){
      ATH_MSG_ERROR("Failed to calculate jet track systematics.");
      return CP::CorrectionCode::Error;
    }
    return CP::CorrectionCode::Ok;
   }

  missingEt METSystematicsTool::caloSyst_scale(missingEt const &softTerms, double const scale) const{
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ );

    return missingEt(softTerms.mpx*scale, softTerms.mpy*scale, softTerms.sumet,
                           softTerms.name,softTerms.source);
  }

  missingEt METSystematicsTool::caloSyst_reso(missingEt const &softTerms) const {
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ );
    //    ATH_MSG_VERBOSE("caloSyst_reso: input MET: " << softTerms.met);

    double const metSigma     = .7 * sqrt(softTerms.sumet);
    double const resUnc       =  m_h_calosyst_reso->GetBinContent(1);
    double const smearedSigma = sqrt( (metSigma* (1. + resUnc))*(metSigma* (1. + resUnc)) -
				      metSigma * metSigma );

    ATH_MSG_VERBOSE("caloSyst_reso: metSigma: " << metSigma << ", resUnc: " << resUnc << ", smearedSigma = " << smearedSigma);

    double const softTermsMet = TMath::Sqrt( softTerms.mpx * softTerms.mpx +
					     softTerms.mpy * softTerms.mpy );


    double const rand  = gRandom->Gaus(0.,1.);
    double const shift = softTermsMet<1e-9 ? 0. : rand*smearedSigma / softTermsMet;

    ATH_MSG_VERBOSE("caloSyst_reso: shift = " << shift);

    return missingEt(softTerms.mpx*(1.+shift),softTerms.mpy*(1.+shift),softTerms.sumet,
                           softTerms.name,softTerms.source);
  }

  missingEt METSystematicsTool::softTrkSyst_scale(missingEt const &softTerms, missingEt const &ptHard, double const shift) const
  {  ATH_MSG_VERBOSE(__PRETTY_FUNCTION__);
    return softTrkSyst_reso(softTerms, ptHard, shift, 0. , 0.);
  }

  missingEt METSystematicsTool::softTrkSyst_reso(missingEt const &softTerms,
						 missingEt const &ptHard,
						 double const shift,
						 double const smearpara,
						 double const smearperp) const{
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ );

    missingEt projection = projectST(softTerms,ptHard);
    projection.mpx = (projection.mpx + shift + smearpara );
    projection.mpy = (projection.mpy +       + smearperp );

    return projectST(projection, ptHard);
  }

  missingEt METSystematicsTool::projectST(missingEt const &softTerms, missingEt const &ptHard) const
  {
    ATH_MSG_VERBOSE( __PRETTY_FUNCTION__ );
    double const ptHardMet =     TMath::Sqrt( ptHard.mpx * ptHard.mpx +
					      ptHard.mpy * ptHard.mpy );

    double const ux    = ptHard.mpx/ptHardMet;
    double const uy    = ptHard.mpy/ptHardMet;
    double const projL = ux*softTerms.mpx + uy*softTerms.mpy;
    double const projT = uy*softTerms.mpx - ux*softTerms.mpy;
    missingEt proj(projL,projT,softTerms.sumet,
                         softTerms.name,softTerms.source);

    return proj;
  }

  missingEt METSystematicsTool::calcPtHard(xAOD::MissingETContainer const * const cont) const
  {
    ATH_MSG_VERBOSE(__PRETTY_FUNCTION__ );

    //get truth container
    xAOD::MissingETContainer const * truthCont = nullptr;
    if(evtStore()->retrieve(truthCont, m_truthCont).isFailure()){
      ATH_MSG_ERROR( m_truthCont << " container empty or does not exist, calcPtHard returning zero.");
      return missingEt();
    }

    xAOD::MissingETContainer::const_iterator truthiter = truthCont->find(MissingETBase::Source::truthNonInt());
    if(truthiter == truthCont->end()){
      ATH_MSG_ERROR( m_truthObj << " is not in " << m_truthCont << ". calcPtHard returing zero." );
      return missingEt();
    }
    const xAOD::MissingET& truthmet = **truthiter;

    missingEt ptHard;
    ptHard.mpx     = truthmet.mpx();
    ptHard.mpy     = truthmet.mpy();
    ptHard.sumet   = truthmet.sumet();
    ptHard.name    = truthmet.name();
    ptHard.source  = truthmet.source();

    //loop over all non soft or total terms
    for(xAOD::MissingETContainer::const_iterator iMET=cont->begin();iMET!=cont->end(); ++iMET  ) {
      if( !( MissingETBase::Source::isSoftTerm( (*iMET)->source() ) ||
	     MissingETBase::Source::isTotalTerm( (*iMET)->source() ) ) ) {
        ptHard.mpx   -= (*iMET)->mpx();
        ptHard.mpy   -= (*iMET)->mpy();
        ptHard.sumet += (*iMET)->sumet();
      }
    }

    if( ptHard.sumet < 0 ) {
      ATH_MSG_ERROR("PtHard has a negative sumet, returning ptHart = 0");
      return missingEt();
    }

    ptHard.mpx  *= 1./(double(m_units)) ;
    ptHard.mpy  *= 1./(double(m_units)) ;
    ptHard.sumet*= 1./(double(m_units)) ;
    
    // protect from zero pthard. protects against an empty pthard for the soft term systematic
    if(ptHard.sumet<1.0e-6){
      ptHard.sumet=1.0e-6;
      ptHard.mpx=1.0e-6;
      ptHard.mpy=0.0;
    }

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
    if(m_useDevArea) { configpath += "dev/"; }

    switch(type){
    case SOFTCALO   :  configfile += m_configSoftCaloFile;
      configpath  = PathResolverFindCalibFile(m_configPrefix+m_configSoftCaloFile);//, "CALIBPATH", PathResolver::RecursiveSearch) ;
        break;
    case SOFTTRK    :  configfile += m_configSoftTrkFile;
      configpath  = PathResolverFindCalibFile(m_configPrefix+m_configSoftTrkFile);//, "CALIBPATH", PathResolver::RecursiveSearch) ;
        break;
    case JETTRK :   configfile += m_configJetTrkFile;
      configpath = PathResolverFindCalibFile(m_configPrefix+m_configJetTrkFile);//, "CALIBPATH", PathResolver::RecursiveSearch) ;
      break;
    default     :  configpath = "";
    }

    ATH_MSG_INFO( "Searching for configFile: " << configfile);
    ATH_MSG_DEBUG( "PWD: "        << gSystem->Getenv("PWD")  )  ;
    ATH_MSG_DEBUG( "CALIBPATH: "  << gSystem->Getenv("CALIBPATH") );

    if(configpath.empty() || configfile.empty() ){
      ATH_MSG_ERROR( "Path Resolver couldn't find config file");
      return StatusCode::FAILURE;
    }

    if( reader.ReadFile(configpath.c_str(),EEnvLevel(0)) < 0) {
      ATH_MSG_ERROR( "Couldn't read the track config file!" );
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( "Configuring from file : " << configpath	 );


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
	histfile = m_configPrefix + histfile;
	systpath  = "/";
	break;
      default     :  break;
    }
    if(m_useDevArea) histfile = "dev/"+histfile;

    std::string const units_string = reader.GetValue( "Units", "" );

    //check if we already set the units in another config file
    if(m_units == 1000){
      if(units_string != "GeV"){
	ATH_MSG_ERROR("initialized the different systematics using two config files that conflict on units");
	return StatusCode::FAILURE;
      }
    }
    if(m_units == 1){
      if( (units_string != "")  &&
	  (units_string != "MeV")
	  ){
	ATH_MSG_ERROR("initialized the different systematics using two config files that conflict on units");
	return StatusCode::FAILURE;
      }
    }

    //set the units again
    if( (units_string == "")  ||
	(units_string == "MeV")
	){
    m_units = 1;
    }else if(units_string == "GeV"){
      m_units = 1000;
    }else{
      ATH_MSG_ERROR("You specified an incorrect 'Units' value.  Please specify MeV , GeV , or leave it blank for MeV");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( "Will read histograms from : " << histfile );
    ATH_MSG_INFO( "Systpath :" <<  systpath ) ;
    histpath = PathResolverFindCalibFile(histfile);//, "CALIBPATH", PathResolver::RecursiveSearch) ;
    ATH_MSG_INFO("Extracted histogram path : " << histpath);

    if(histfile.empty() || systpath.empty() || histpath.empty() ){
      ATH_MSG_ERROR("Failed to correctly set histfile path, or path to histograms inside of the histfile" );
      return StatusCode::FAILURE;
    }


    if( (m_units != 1)    &&
	(m_units != 1000) &&
	(m_units != -1)
	){
      ATH_MSG_ERROR("Something is wrong with your units initialization.  Please contact the developers (you should never get here).");
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

  void METSystematicsTool::setRandomSeed(int seed) const {
    ATH_MSG_VERBOSE(__PRETTY_FUNCTION__);
    m_rand.SetSeed(seed);
  }
}

//  LocalWords:  SOFTTRK
