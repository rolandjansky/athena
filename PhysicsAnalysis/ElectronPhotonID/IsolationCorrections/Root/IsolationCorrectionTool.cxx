/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IsolationCorrections/IsolationCorrectionTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/Photon.h"
#include "xAODMetaData/FileMetaData.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PathResolver/PathResolver.h"

#ifndef ROOTCORE
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include <boost/algorithm/string.hpp>
#endif //ROOTCORE

namespace CP {

  IsolationCorrectionTool::IsolationCorrectionTool( const std::string &name )
    : asg::AsgMetadataTool(name), m_systDDonoff("PH_Iso_DDonoff"){
    declareProperty("CorrFile",                    m_corr_file                    = "IsolationCorrections/v1/isolation_ptcorrections_rel20_2.root");
    declareProperty("CorrFile_ddshift_2015",       m_corr_ddshift_2015_file       = "IsolationCorrections/v1/isolation_ddcorrection_shift_2015_v3.root");
    declareProperty("CorrFile_ddshift",            m_corr_ddshift_file            = "IsolationCorrections/v1/isolation_ddcorrection_shift.root");
    declareProperty("CorrFile_ddsmearing",         m_corr_ddsmearing_file         = "IsolationCorrections/v1/isolation_ddcorrection_smearing.root");
    declareProperty("ToolVer",                     m_tool_ver_str                 = "REL20_2");
    declareProperty("DataDrivenVer",               m_ddVersion                    = "2015");
    declareProperty("UseMetadata",                 m_usemetadata                  = false);
    declareProperty("AFII_corr",                   m_AFII_corr                    = false);
    declareProperty("IsMC",                        m_is_mc                        = true);
    declareProperty("Correct_etcone",              m_correct_etcone               = false);
    declareProperty("Trouble_categories",          m_trouble_categories           = true);
    declareProperty("Apply_ddshifts",              m_apply_ddDefault              = true);
    m_isol_corr = new IsolationCorrection(name);
  }

  StatusCode IsolationCorrectionTool::initialize() {
    ATH_MSG_INFO( "in initialize of " << name() << "..." );

    m_isol_corr->msg().setLevel(this->msg().level());

    //
    // Resolve the paths to the input files
    std::vector < std::string > corrFileNameList;
    corrFileNameList.push_back(m_corr_file);
    corrFileNameList.push_back(m_corr_ddshift_2015_file);
    corrFileNameList.push_back(m_corr_ddshift_file);
    corrFileNameList.push_back(m_corr_ddsmearing_file);

    for ( unsigned int i=0; i<corrFileNameList.size(); ++i ){
      
      //First try the PathResolver
      std::string filename = PathResolverFindCalibFile( corrFileNameList.at(i) );
      if (filename.empty()){
	ATH_MSG_ERROR ( "Could NOT resolve file name " << corrFileNameList.at(i) );
	return StatusCode::FAILURE ;
      } else{
	ATH_MSG_INFO(" Path found = "<<filename);
      }
      corrFileNameList.at(i) = filename;	
    }
    //

    CP::IsolationCorrection::Version tool_ver;
    
    m_metadata_retrieved = false;
    	   
    if      (m_tool_ver_str == "REL20_2") tool_ver = CP::IsolationCorrection::REL20_2;
    else if (m_tool_ver_str == "REL20")   tool_ver = CP::IsolationCorrection::REL20;
    else if (m_tool_ver_str == "REL17_2") tool_ver = CP::IsolationCorrection::REL17_2;
    else {
      ATH_MSG_WARNING("Tool version not recognized: "<<m_tool_ver_str<<"\nAllowed versions: REL20_2, REL20, REL17_2");
      return StatusCode::FAILURE;
    }

    if(TString(corrFileNameList[0]).Contains("isolation_ptcorrections_rel17_2.root") && m_tool_ver_str != "REL17_2"){
      ATH_MSG_WARNING("The specified correction file is not for "<<m_tool_ver_str<<" please use proper correction file");
      return StatusCode::FAILURE;
    }

    if(TString(corrFileNameList[0]).Contains("isolation_ptcorrections_rel20_2.root") && m_tool_ver_str != "REL20_2"){
      ATH_MSG_WARNING("The specified correction file is not for "<<m_tool_ver_str<<" please use proper correction file");
      return StatusCode::FAILURE;
    }

    m_isol_corr->SetCorrectionFile(m_corr_file, m_corr_ddshift_file, m_corr_ddsmearing_file, m_corr_ddshift_2015_file);
    m_isol_corr->SetToolVer(tool_ver);
    m_isol_corr->SetTroubleCategories(m_trouble_categories);

    // If Default is false, there is no correction, and no topoEtconeXX systematic uncertainty !
    if (m_apply_ddDefault) {
      if (m_ddVersion == "2012" or m_ddVersion == "2015") {
	//register ourselves with the systematic registry! 
	CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
	if( registry.registerSystematics( *this ) != CP::SystematicCode::Ok ) return StatusCode::FAILURE;
      } else
	ATH_MSG_WARNING("Unknown data driven correction");
    } else{
      m_apply_dd = false;
    }

    //If we do not want to use metadata
    if(!m_usemetadata) {
    m_isol_corr->SetAFII(m_AFII_corr);
    m_isol_corr->SetDataMC(m_is_mc);    
    }

    return m_isol_corr->initialize();
  }

  StatusCode IsolationCorrectionTool::finalize() {
    ATH_MSG_INFO( "in finalize" );    
    return m_isol_corr->finalize();
  }

  StatusCode IsolationCorrectionTool::get_simflavour_from_metadata(PATCore::ParticleDataType::DataType& result){
    //When we can not retrieve metadata Failure will be returned from this method

    //default result
    result = PATCore::ParticleDataType::Data;
    
#ifndef ROOTCORE
    //Athena environent
    std::string dataType("");
    if( AthAnalysisHelper::retrieveMetadata( "/TagInfo", "project_name" , dataType, inputMetaStore() ).isFailure() ) {
      //In case we can not retrieve the metatada
      return StatusCode::FAILURE;    
    }
    //
    //We got the dataType is it data
    if(dataType != "IS_SIMULATION") {
      ATH_MSG_DEBUG("NOT IS_SIMULATION aka Data");
      return StatusCode::SUCCESS;    
    }
    //
    //if not data  determine Fast/FullSim
    ATH_MSG_DEBUG("IS_SIMULATION");
    std::string simType("");
    if( AthAnalysisHelper::retrieveMetadata("/Simulation/Parameters", "SimulationFlavour", simType, inputMetaStore()).isFailure() ) {
      return StatusCode::FAILURE;    
    }
    else{
      boost::to_upper(simType);
      result = (simType.find("ATLFASTII")==std::string::npos) ?  PATCore::ParticleDataType::Full : PATCore::ParticleDataType::Fast;
      return StatusCode::SUCCESS;
    }
#endif    
    //Here is the RootCore or to be dual use , assumes we have not returned before for Athena
    std::string simulationType("");
    if (!inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData")) {
      return StatusCode::FAILURE;    
    }
    const xAOD::FileMetaData* fmd = 0;
    ATH_CHECK(inputMetaStore()->retrieve(fmd, "FileMetaData"));      
    //
    const bool s = fmd->value(xAOD::FileMetaData::simFlavour, simulationType);
    if (!s) { 
      //no simFlavour metadata failure
      return StatusCode::FAILURE;    
    }
    else {
      result = (simulationType == "FullSim" ? PATCore::ParticleDataType::Full : PATCore::ParticleDataType::Fast);
      return StatusCode::SUCCESS;    
    }
    //
    return StatusCode::SUCCESS;    
  }
  
  StatusCode IsolationCorrectionTool::beginInputFile() {
    // If we do not want to use metadata
    if(!m_usemetadata) {
      return StatusCode::SUCCESS;    
    }
    //
    PATCore::ParticleDataType::DataType result;
    const StatusCode status = get_simflavour_from_metadata(result);
    if (status == StatusCode::SUCCESS) {
      ATH_MSG_DEBUG("We have metadata");
    
      if (result == PATCore::ParticleDataType::Fast) {
	m_is_mc = true;
	m_AFII_corr = true;
	ATH_MSG_DEBUG("Fast sim");
      }
      else if (result == PATCore::ParticleDataType::Full) {
	m_is_mc = true;
	m_AFII_corr = false;
	ATH_MSG_DEBUG("Full sim");
      }else {
	m_is_mc = false;
	m_AFII_corr = false;
	ATH_MSG_DEBUG("Data ");
      }
      ATH_MSG_INFO("is MC = " << m_is_mc);
      ATH_MSG_INFO("use AFII = " << m_AFII_corr);
      ATH_MSG_DEBUG("metadata from new file: " << (result == PATCore::ParticleDataType::Data ? "data" : 
						   (result == PATCore::ParticleDataType::Full ? "full simulation" : "fast simulation")));
    }
    else {
      ATH_MSG_WARNING("Not possible to retrieve metadata in the begin Input File");
      m_metadata_retrieved = false;
      m_is_mc = false;
      m_AFII_corr = false;
    }
    m_isol_corr->SetAFII(m_AFII_corr);
    m_isol_corr->SetDataMC(m_is_mc);    
    //
    return StatusCode::SUCCESS;    
  }

  StatusCode IsolationCorrectionTool::endInputFile() {
    // If we do not want to use metadata
    if(!m_usemetadata) {
      return StatusCode::SUCCESS;    
    }
    m_metadata_retrieved = false;
    return StatusCode::SUCCESS;
  }

  StatusCode IsolationCorrectionTool::beginEvent() {
    // If we do not want to use metadata, or we retrieved them already
    if((!m_usemetadata) || m_metadata_retrieved) {
      return StatusCode::SUCCESS;    
    }
    //
    //If not metadata have been available and want to use them go via event info 
    const xAOD::EventInfo* evtInfo(0);
    if( (evtStore()->retrieve(evtInfo, "")).isFailure()){
      ATH_MSG_WARNING(" No default Event Info collection found") ;
      return StatusCode::SUCCESS;
    }
    m_is_mc = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);   
    m_metadata_retrieved = true;
    m_isol_corr->SetDataMC(m_is_mc);
    return StatusCode::SUCCESS;
  }

  CP::CorrectionCode IsolationCorrectionTool::CorrectLeakage(xAOD::Egamma & eg) {

    static std::vector<xAOD::Iso::IsolationType> topoisolation_types = {xAOD::Iso::topoetcone20,
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
      static std::vector<xAOD::Iso::IsolationType> isolation_types = {xAOD::Iso::etcone20,
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

    static SG::AuxElement::Decorator<float> decDDcor20("topoetcone20_DDcorr");
    static SG::AuxElement::Decorator<float> decDDcor40("topoetcone40_DDcorr");
	
    static const std::vector<xAOD::Iso::IsolationType> topoisolation_types = {xAOD::Iso::topoetcone20,
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
      if (m_is_mc && m_apply_dd && type != xAOD::Iso::topoetcone30) {
	ddcorr = this->GetDDCorrection(eg,type);
	if (type == xAOD::Iso::topoetcone20)
	  decDDcor20(eg) = ddcorr;
	else if (type == xAOD::Iso::topoetcone40)
	  decDDcor40(eg) = ddcorr;
	iso += ddcorr;
      }
      if (eg.pt() > 25e3) 
	ATH_MSG_DEBUG("ddcor = " << ddcorr << " new Iso = " << iso << "\n");
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
  float IsolationCorrectionTool::GetPtCorrection(const xAOD::Egamma& input, xAOD::Iso::IsolationType isol) const {
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

    if (m_apply_ddDefault)
      result.insert( m_systDDonoff );

    return result;
  }

  CP::SystematicSet IsolationCorrectionTool::recommendedSystematics() const {
    return affectingSystematics();
  }

  CP::SystematicCode IsolationCorrectionTool::applySystematicVariation( const CP::SystematicSet& systConfig ) {
    if (systConfig.find(m_systDDonoff) != systConfig.end())
      m_apply_dd = false;
    else
      m_apply_dd = m_apply_ddDefault ? true  : false;
    return CP::SystematicCode::Ok;
  }

  IsolationCorrectionTool::~IsolationCorrectionTool() {
    delete m_isol_corr;
  }

}
