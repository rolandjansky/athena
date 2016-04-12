/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IsolationCorrections/IsolationCorrectionTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "PATInterfaces/SystematicRegistry.h"

namespace CP {

  IsolationCorrectionTool::IsolationCorrectionTool( const std::string &name )
    : asg::AsgTool(name), m_apply_dd(false), m_systDDonoff("PH_Iso_DDonoff")
  {
    declareProperty("CorrFile",                    m_corr_file                    = "IsolationCorrections/isolation_ptcorrections_rel20_2.root");
    declareProperty("CorrFile_ddshift_2015",       m_corr_ddshift_2015_file       = "IsolationCorrections/isolation_ddcorrection_shift_2015_v2.root");
    declareProperty("CorrFile_ddshift",            m_corr_ddshift_file            = "IsolationCorrections/isolation_ddcorrection_shift.root");
    declareProperty("CorrFile_ddsmearing",         m_corr_ddsmearing_file         = "IsolationCorrections/isolation_ddcorrection_smearing.root");
    declareProperty("ToolVer",                     m_tool_ver_str                 = "REL20_2");
    declareProperty("AFII_corr",                   m_AFII_corr                    = false);
    declareProperty("IsMC",                        m_is_mc                        = true);
    declareProperty("DataDrivenVer",               m_ddVersion                    = "2015");
    declareProperty("Correct_etcone",              m_correct_etcone               = false);
    declareProperty("Trouble_categories",          m_trouble_categories           = true);
    m_isol_corr = new IsolationCorrection(name);
    m_isol_corr->msg().setLevel(this->msg().level());
  }

  StatusCode IsolationCorrectionTool::initialize() {
    ATH_MSG_INFO( "in initialize of " << name() << "..." );

    CP::IsolationCorrection::Version tool_ver;

    if      (m_tool_ver_str == "REL20_2") tool_ver = CP::IsolationCorrection::REL20_2;
    else if (m_tool_ver_str == "REL20")   tool_ver = CP::IsolationCorrection::REL20;
    else if (m_tool_ver_str == "REL17_2") tool_ver = CP::IsolationCorrection::REL17_2;
    else {
      ATH_MSG_WARNING("Tool version not recognized: "<<m_tool_ver_str<<"\nAllowed versions: REL20_2, REL20, REL17_2");
      return StatusCode::FAILURE;
    }

    if(m_corr_file == "IsolationCorrections/isolation_ptcorrections_rel17_2.root" && m_tool_ver_str != "REL17_2"){
      ATH_MSG_WARNING("The specified correction file is not for "<<m_tool_ver_str<<" please use proper correction file");
      return StatusCode::FAILURE;
    }

    if(m_corr_file == "IsolationCorrections/isolation_ptcorrections_rel20_2.root" && m_tool_ver_str != "REL20_2"){
      ATH_MSG_WARNING("The specified correction file is not for "<<m_tool_ver_str<<" please use proper correction file");
      return StatusCode::FAILURE;
    }

    m_isol_corr->SetDataMC(m_is_mc);
    m_isol_corr->SetCorrectionFile(m_corr_file,m_corr_ddshift_file,m_corr_ddsmearing_file, m_corr_ddshift_2015_file);
    m_isol_corr->SetToolVer(tool_ver);
    m_isol_corr->SetAFII(m_AFII_corr);
    m_isol_corr->SetTroubleCategories(m_trouble_categories);

    m_isol_corr->SetDD(m_apply_dd);

    if (m_ddVersion == "2012" or m_ddVersion == "2015")
      m_isol_corr->SetDD(m_apply_dd);
    else
      ATH_MSG_WARNING("Unknown data driven correction");

    //register ourselves with the systematic registry! 
    CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
    if( registry.registerSystematics( *this ) != CP::SystematicCode::Ok ) return StatusCode::FAILURE;
    
    return m_isol_corr->initialize();
  }

  StatusCode IsolationCorrectionTool::finalize() {
    ATH_MSG_INFO( "in finalize" );

    return m_isol_corr->finalize();
  }

  CP::CorrectionCode IsolationCorrectionTool::CorrectLeakage(xAOD::Egamma & eg) {

    std::vector<xAOD::Iso::IsolationType> topoisolation_types = {xAOD::Iso::topoetcone20,
								 xAOD::Iso::topoetcone30,
								 xAOD::Iso::topoetcone40};

    for (auto type : topoisolation_types) {
      // this will correct a corrected topoetcone : replace a (old) leakage by another (new) one
      float oldleak = eg.isolationCaloCorrection(type, xAOD::Iso::ptCorrection);
      float newleak = this->GetPtCorrection(eg,type);
      float iso     = 0;
      bool gotIso   = eg.isolationValue(iso,type);
      if (!gotIso) continue;
      iso += (oldleak-newleak);
      bool setIso = eg.setIsolationValue(iso,type);
      setIso = (setIso && eg.setIsolationCaloCorrection(newleak,type,xAOD::Iso::ptCorrection));
      if (!setIso) {
	        ATH_MSG_WARNING("Can't correct leakage for " << xAOD::Iso::toString(type));
	        return CP::CorrectionCode::Error;
      }
    }
    
    if (m_correct_etcone){
      // this is supposed to correct an uncorrected etcone
      std::vector<xAOD::Iso::IsolationType> isolation_types = {xAOD::Iso::etcone20,
							       xAOD::Iso::etcone30,
							       xAOD::Iso::etcone40};

      ATH_MSG_VERBOSE( "Correcting etconeXX isolation..." );
      for(auto type : topoisolation_types){
        float Etcone_value_corr = m_isol_corr->GetPtCorrectedIsolation(eg,type);
        eg.setIsolationValue(Etcone_value_corr,type);
      }
    }

    return CP::CorrectionCode::Ok;
  }

  CP::CorrectionCode IsolationCorrectionTool::applyCorrection(xAOD::Egamma &eg) {
    std::vector<xAOD::Iso::IsolationType> topoisolation_types = {xAOD::Iso::topoetcone20,
								 xAOD::Iso::topoetcone30,
								 xAOD::Iso::topoetcone40};
    for (auto type : topoisolation_types) {
      float oldleak = eg.isolationCaloCorrection(type, xAOD::Iso::ptCorrection);
      float newleak = this->GetPtCorrection(eg,type);
      float oldiso  = 0;
      bool gotIso   = eg.isolationValue(oldiso,type);
      if (!gotIso) continue;
      if (eg.pt() > 25e3) 
	ATH_MSG_DEBUG("pt = " << eg.pt() << " eta = " << eg.eta() << ", def Iso " << xAOD::Iso::toString(type) << " = " << oldiso
		      << " old leak = " << oldleak << " new leak = " << newleak);
      float iso     = oldiso + (oldleak-newleak);
      float ddcorr  = 0;
      if (m_is_mc) {
	if (m_apply_dd)
	  ddcorr = this->GetDDCorrection(eg,type);
	iso += ddcorr;
      }
      if (eg.pt() > 25e3) 
	ATH_MSG_DEBUG("ddcor = " << ddcorr << " new Iso = " << iso);
      bool setIso = eg.setIsolationValue(iso,type);
      setIso = (setIso && eg.setIsolationCaloCorrection(newleak-ddcorr,type,xAOD::Iso::ptCorrection));
      if (!setIso) {
      	ATH_MSG_WARNING("Can't correct leakage for " << xAOD::Iso::toString(type));
      	return CP::CorrectionCode::Error;
      }
    }
    return CP::CorrectionCode::Ok;
  }

  float IsolationCorrectionTool::GetPtCorrectedIsolation(const xAOD::Egamma& input, xAOD::Iso::IsolationType isol){
    return m_isol_corr->GetPtCorrectedIsolation(input, isol);
  }
  float IsolationCorrectionTool::GetPtCorrection(const xAOD::Egamma& input, xAOD::Iso::IsolationType isol){
    return m_isol_corr->GetPtCorrection(input, isol);
  }
  
  float IsolationCorrectionTool::GetDDCorrection(const xAOD::Egamma& input, xAOD::Iso::IsolationType isol){
    if (m_ddVersion == "2012") {
      if (isol == xAOD::Iso::topoetcone40) return m_isol_corr->GetDDCorrection(input);
    } else if (m_ddVersion == "2015")
      return m_isol_corr->GetDDCorrection_2015(input, isol);

    return 0;
  }

  CP::CorrectionCode IsolationCorrectionTool::correctedCopy( const xAOD::Egamma& input, xAOD::Egamma*& output) {
    // A sanity check:
    if( output ) ATH_MSG_WARNING( "Non-null pointer received. " "There's a possible memory leak!" );

    if ( input.type() == xAOD::Type::Electron ) {
      output = new xAOD::Electron();
      output->makePrivateStore( static_cast<const xAOD::Electron&>(input) );
      return applyCorrection( *output );
    }
    else if ( input.type() == xAOD::Type::Photon ) {
      output = new xAOD::Photon();
      output->makePrivateStore( static_cast<const xAOD::Photon&>(input) );
      return applyCorrection( *output );
    }
    else {
      ATH_MSG_WARNING("Didn't get an electron or photon");
      return CP::CorrectionCode::Error;
    }

  }

  //Systematics
  bool IsolationCorrectionTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const {
    CP::SystematicSet sys = affectingSystematics();
    return sys.find( systematic ) != sys.end();
  }

  CP::SystematicSet IsolationCorrectionTool::affectingSystematics() const {
    CP::SystematicSet result;

    result.insert( m_systDDonoff );

    return result;
  }

  CP::SystematicSet IsolationCorrectionTool::recommendedSystematics() const {
    return affectingSystematics();
  }

  CP::SystematicCode IsolationCorrectionTool::applySystematicVariation( const CP::SystematicSet& systConfig ) {

    if (systConfig.find(m_systDDonoff) != systConfig.end())
      m_apply_dd = true;
    else
      m_apply_dd = false;

    return CP::SystematicCode::Ok;
  }

  IsolationCorrectionTool::~IsolationCorrectionTool() {
    delete m_isol_corr;
  }

}
