/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IsolationCorrections/IsolationCorrectionTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMetaData/FileMetaData.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PathResolver/PathResolver.h"
#include <boost/algorithm/string.hpp>
#include "xAODPrimitives/IsolationHelpers.h"

#ifndef ROOTCORE
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#endif //ROOTCORE

#include "TFile.h"
#include "xAODEventShape/EventShape.h"

namespace CP {

  IsolationCorrectionTool::IsolationCorrectionTool( const std::string &name )
    : asg::AsgMetadataTool(name), m_systDDonoff("PH_Iso_DDonoff"){
    declareProperty("CorrFile",                    m_corr_file            = "IsolationCorrections/v1/isolation_ptcorrections_rel20_2.root");
    declareProperty("CorrFile_ddshift",            m_corr_ddshift_file    = "IsolationCorrections/v3/isolation_ddcorrection_shift.root");
    declareProperty("CorrFile_ddshift_2015_2016",  m_corr_ddshift_2015_2016_file = ""); //Obsolete, kept for compatibility
    declareProperty("CorrFile_ddshift_2017",       m_corr_ddshift_2017_file      = ""); //Obsolete, kept for compatibility
    declareProperty("CorrFile_ddsmearing",         m_corr_ddsmearing_file = "IsolationCorrections/v1/isolation_ddcorrection_smearing.root", "a run I smearing for MC calo iso"); 
    declareProperty("ToolVer",                     m_tool_ver_str         = "REL21");
    declareProperty("DataDrivenVer",               m_ddVersion            = "2017");
    declareProperty("UseMetadata",                 m_usemetadata          = false);
    declareProperty("AFII_corr",                   m_AFII_corr            = false);
    declareProperty("IsMC",                        m_is_mc                = true);
    declareProperty("Correct_etcone",              m_correct_etcone       = false);
    declareProperty("Trouble_categories",          m_trouble_categories   = true);
    declareProperty("Apply_ddshifts",              m_apply_ddDefault      = true);
    declareProperty("Apply_SC_leakcorr",           m_apply_SC_leak_corr   = false);
    declareProperty("Apply_etaEDParPU_correction",    m_apply_etaEDParPU_corr= false);
    declareProperty("CorrFile_etaEDParPU_correction", m_corr_etaEDParPU_file = "IsolationCorrections/zetas_pu.root");

    m_isol_corr = new IsolationCorrection(name);
  }

  StatusCode IsolationCorrectionTool::initialize() {
    ATH_MSG_INFO( "in initialize of " << name() << "..." );

    m_isol_corr->msg().setLevel(this->msg().level());

    // Warning about obsolete properties
    if (m_corr_ddshift_2015_2016_file != "" || m_corr_ddshift_2017_file != "")
      ATH_MSG_WARNING("The properties CorrFile_ddshift_2015_2016 and CorrFile_ddshift_2017 are obsolete. Use CorrFile_ddshift instead");

    //
    // Resolve the paths to the input files
    std::vector < std::string > corrFileNameList;
    corrFileNameList.push_back(m_corr_file);
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
    
    if      (m_tool_ver_str == "REL21")   tool_ver = CP::IsolationCorrection::REL21;	   
    else if (m_tool_ver_str == "REL20_2") tool_ver = CP::IsolationCorrection::REL20_2;
    else if (m_tool_ver_str == "REL17_2") tool_ver = CP::IsolationCorrection::REL17_2;
    else {
      ATH_MSG_WARNING("Tool version not recognized: "<<m_tool_ver_str<<"\nAllowed versions: REL21, REL20_2, REL17_2");
      return StatusCode::FAILURE;
    }

    if (TString(corrFileNameList[0]).Contains("isolation_ptcorrections_rel17_2.root") && m_tool_ver_str != "REL17_2") {
      ATH_MSG_WARNING("The specified correction file is not for "<<m_tool_ver_str<<" please use proper correction file");
      return StatusCode::FAILURE;
    }
    
    if (TString(corrFileNameList[0]).Contains("isolation_ptcorrections_rel20_2.root") && !( m_tool_ver_str == "REL20_2" || m_tool_ver_str == "REL21" ) ){
      ATH_MSG_WARNING("The specified correction file is not for "<<m_tool_ver_str<<" please use proper correction file");
      return StatusCode::FAILURE;
    }

    m_isol_corr->SetCorrectionFile(m_corr_file, m_corr_ddshift_file, m_corr_ddsmearing_file);
    m_isol_corr->SetToolVer(tool_ver);
    m_isol_corr->SetTroubleCategories(m_trouble_categories);
    
    // Note that systematics in Rel 21 are NOT done with the DD-Corr ON/OFF method! 
    if (m_apply_ddDefault) {
      if (m_ddVersion == "2015" || m_ddVersion == "2015_2016" || m_ddVersion == "2017") {
	//if not REL21, register ourselves with the systematic registry! 
	if (m_tool_ver_str!="REL21") {
	  CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
	  if( registry.registerSystematics( *this ) != CP::SystematicCode::Ok ) return StatusCode::FAILURE;
	}
	m_apply_dd = true;
      } else {
	ATH_MSG_WARNING("Unknown data driven correction");
	m_apply_dd = false;
      }
    } else
      m_apply_dd = false;
    
    // Don't use DD Corrections for AFII if not rel21 (I do not know what was done in rel20.7 !)
    if (m_tool_ver_str != "REL21" && m_AFII_corr) m_apply_dd = false;

    //If we do not want to use metadata
    if (!m_usemetadata) {
      m_isol_corr->SetAFII(m_AFII_corr);
      m_isol_corr->SetDataMC(m_is_mc);    
    }

    if(m_apply_etaEDParPU_corr){
      std::string filename = PathResolverFindCalibFile( m_corr_etaEDParPU_file );
      if (filename.empty()){
        ATH_MSG_ERROR ( "Could NOT resolve file name " << m_corr_etaEDParPU_file );
        return StatusCode::FAILURE ;
      }
      ATH_MSG_INFO(" Path found = "<<filename);
      std::unique_ptr<TFile> f(new TFile(filename.c_str(), "READ"));

      m_map_isotype_zetaPU[xAOD::Iso::topoetcone20] = std::unique_ptr<TGraph>((TGraph*)f->Get("topoetcone20")->Clone());
      m_map_isotype_zetaPU[xAOD::Iso::topoetcone30] = std::unique_ptr<TGraph>((TGraph*)f->Get("topoetcone30")->Clone());
      m_map_isotype_zetaPU[xAOD::Iso::topoetcone40] = std::unique_ptr<TGraph>((TGraph*)f->Get("topoetcone40")->Clone());
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
    //
    std::string simType("");
    
#ifndef XAOD_STANDALONE
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
      ATH_MSG_DEBUG("no sim flavour from metadata: must be data");
      return StatusCode::FAILURE;    
    }
    else {
      boost::to_upper(simType);
      result = (simType.find("ATLFASTII")==std::string::npos) ?  PATCore::ParticleDataType::Full : PATCore::ParticleDataType::Fast;
      return StatusCode::SUCCESS;    
    }
    //
    return StatusCode::SUCCESS;    
  }
  
  StatusCode IsolationCorrectionTool::beginInputFile() {
    // If we do not want to use metadata
    if (!m_usemetadata) {
      ATH_MSG_INFO("is MC = " << m_is_mc << ", use AFII = " << m_AFII_corr);
      return StatusCode::SUCCESS;    
    }
    //
    PATCore::ParticleDataType::DataType result;
    const StatusCode status = get_simflavour_from_metadata(result);
    if (status == StatusCode::SUCCESS) {
      ATH_MSG_DEBUG("We have metadata");
      if (result == PATCore::ParticleDataType::Fast) {
	m_is_mc     = true;
	m_AFII_corr = true;
      } else if (result == PATCore::ParticleDataType::Full) {
	m_is_mc     = true;
	m_AFII_corr = false;
      } else {
	m_is_mc     = false;
	m_AFII_corr = false;
      }
      ATH_MSG_DEBUG("metadata from new file: " << (result == PATCore::ParticleDataType::Data ? "data" : 
						   (result == PATCore::ParticleDataType::Full ? "full simulation" : "fast simulation")));    
    }
    else {
      ATH_MSG_WARNING("Not possible to retrieve metadata in the begin Input File");
      m_metadata_retrieved = false;
      m_is_mc     = false;
      m_AFII_corr = false;
    }
    ATH_MSG_INFO("is MC = " << m_is_mc << ", use AFII = " << m_AFII_corr);
    m_isol_corr->SetAFII(m_AFII_corr);
    m_isol_corr->SetDataMC(m_is_mc);    
    //
    return StatusCode::SUCCESS;    
  }

  StatusCode IsolationCorrectionTool::endInputFile() {
    if (m_usemetadata)
      m_metadata_retrieved = false;
    return StatusCode::SUCCESS;
  }

  StatusCode IsolationCorrectionTool::beginEvent() {
    // If we do not want to use metadata, or we retrieved them already
    if ((!m_usemetadata) || m_metadata_retrieved)
      return StatusCode::SUCCESS;    
  
    //
    //If not metadata have been available and want to use them go via event info 
    const xAOD::EventInfo* evtInfo(0);
    if( (evtStore()->retrieve(evtInfo, "EventInfo")).isFailure()){
      ATH_MSG_WARNING("EventInfo not found ??") ;
      return StatusCode::FAILURE;
    }
    m_is_mc = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);   
    m_metadata_retrieved = true;
    m_isol_corr->SetDataMC(m_is_mc);
    return StatusCode::SUCCESS;
  }

  // this will correct a corrected topoetcone : replace a (old) leakage by another (new) one
  // This is not for photon, as it does not consider DD
  CP::CorrectionCode IsolationCorrectionTool::CorrectLeakage(xAOD::Egamma & eg) {

    static std::vector<xAOD::Iso::IsolationType> topoisolation_types = {xAOD::Iso::topoetcone20,
									/* xAOD::Iso::topoetcone30, */
									xAOD::Iso::topoetcone40};
    
    for (auto type : topoisolation_types) {
      float oldleak = 0.;
      if (eg.isolationCaloCorrection(oldleak, type, xAOD::Iso::ptCorrection)) {
	ATH_MSG_DEBUG("leakage correction not stored for isolation type " << xAOD::Iso::toString(type) << ". Nothing done.");
	continue;
      }
      float newleak = this->GetPtCorrection(eg,type);
      float iso     = 0;
      if (!eg.isolationValue(iso,type)) {
	ATH_MSG_WARNING("Isolation variable " << xAOD::Iso::toString(type) << " not stored. Nothing done.");
	continue;
      }
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
									      /* xAOD::Iso::topoetcone30, */
									      xAOD::Iso::topoetcone40};
    for (auto type : topoisolation_types) {
      float oldleak = 0.;
      if (!eg.isolationCaloCorrection(oldleak, type, xAOD::Iso::ptCorrection)) {
	ATH_MSG_DEBUG("leakage correction not stored for isolation type " << xAOD::Iso::toString(type) << ". Nothing done");
	continue;
      }
      float newleak = this->GetPtCorrection(eg,type);
      float oldiso  = 0;
      bool gotIso   = eg.isolationValue(oldiso,type);
      if (!gotIso) continue;
      //      if (eg.pt() > 25e3) 
      //	ATH_MSG_DEBUG("pt = " << eg.pt() << " eta = " << eg.eta() << ", def Iso " << xAOD::Iso::toString(type) << " = " << oldiso
      //		      << " old leak = " << oldleak << " new leak = " << newleak);
      
      // Use the Random Run Number from the Event Info to check which year's DD-Corrections to use 
      // If the RandomRunNo can't be obtained, then default to what is set by either the default choice or by the AuxData check 
      unsigned int theRunNumber = 0 ; 
      const xAOD::EventInfo *eventInfo = evtStore()->retrieve< const xAOD::EventInfo>("EventInfo");  
      if (eventInfo) { 
	static const SG::AuxElement::Accessor<unsigned int> randomrunnumber("RandomRunNumber"); 
	if (randomrunnumber.isAvailable(*eventInfo))
	  theRunNumber = randomrunnumber(*(eventInfo)) ; 
      } else
	ATH_MSG_WARNING("Could not retrieve EventInfo object"); 
      if (theRunNumber >= 320000)
	m_ddVersion = "2017" ;      // RunNo found, and is in 2017 range 
      else if (theRunNumber > 0)
	m_ddVersion = "2015_2016" ; // RunNo found, but less than 2017 range
      // otherwise, stick with default (m_ddVersion is already assigned)
      
      // Don't use DD Corrections for AFII if not rel21 ? (I do not know what was done for rel20.7 !!!) 
      if (m_tool_ver_str != "REL21" && m_AFII_corr) m_apply_dd = false;
      
      float iso     = oldiso + (oldleak-newleak);
      float ddcorr  = 0;


      if(m_apply_SC_leak_corr){
          float SC_correction = 0;
          if(!eg.isolationCaloCorrection(SC_correction, xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::coreEnergy)){
              ATH_MSG_WARNING("Could not find SC based correction for " << eg.type());
          }
          ATH_MSG_VERBOSE("Old leakage correction: " << newleak);
          ATH_MSG_VERBOSE("SC based leakage correction value: " << SC_correction);
          ATH_MSG_VERBOSE("Iso before SC correction: " << iso);
          iso = iso + newleak - SC_correction;
          ATH_MSG_VERBOSE("Iso after SC correction: " << iso);
      }

      if(m_apply_etaEDParPU_corr && !m_AFII_corr){
        float abseta = fabs(eg.caloCluster()->etaBE(2));
        const xAOD::EventShape* evtShapeCentral;

        if(evtStore()->retrieve(evtShapeCentral, "TopoClusterIsoCentralEventShape").isFailure()){
          ATH_MSG_WARNING("Cannot retrieve density container " << "TopoClusterIsoCentralEventShape" << " for isolation correction.");
          return CP::CorrectionCode::Error;
        }
        float centralDensity = evtShapeCentral->getDensity(xAOD::EventShape::Density);
        float densityOldCorrection = 0.;
        if(abseta <= 1.5){
          densityOldCorrection = centralDensity;
        }
        else{
          const xAOD::EventShape* evtShapeForward;
          if(evtStore()->retrieve(evtShapeForward, "TopoClusterIsoForwardEventShape").isFailure()){
            ATH_MSG_WARNING("Cannot retrieve density container " << "TopoClusterIsoForwardEventShape" << " for isolation correction.");
            return CP::CorrectionCode::Error;
          }
          densityOldCorrection = evtShapeForward->getDensity(xAOD::EventShape::Density);
        }
        float dR = xAOD::Iso::coneSize(type);
        float a_core = 5*7*0.025*TMath::Pi()/128;
        float area = TMath::Pi()*dR*dR-a_core;
        float oldpu_corr = densityOldCorrection*area;
        float newpu_corr = m_map_isotype_zetaPU[type]->Eval(abseta)*centralDensity*area;
        iso = iso + oldpu_corr - newpu_corr;
        ATH_MSG_VERBOSE("Applying parametrized pileup correction to " << eg.type() << " with |eta|="<< abseta);
        ATH_MSG_VERBOSE("Old parametrized pileup correction for "<<xAOD::Iso::toString(type)<< ": "<<oldpu_corr);
        ATH_MSG_VERBOSE("New parametrized pileup correction for "<<xAOD::Iso::toString(type)<< ": "<<newpu_corr);
        ATH_MSG_VERBOSE("Isolation after new correction for "<<xAOD::Iso::toString(type)<< ": "<<iso);
        ATH_MSG_VERBOSE("Isolation after old correction for "<<xAOD::Iso::toString(type)<< ": "<<iso+newpu_corr-oldpu_corr); 
      }

      if (m_is_mc && m_apply_dd && type != xAOD::Iso::topoetcone30 && eg.type() == xAOD::Type::Photon) {
	ddcorr = this->GetDDCorrection(eg,type);
	if (type == xAOD::Iso::topoetcone20)
	  decDDcor20(eg) = ddcorr;
	else if (type == xAOD::Iso::topoetcone40)
	  decDDcor40(eg) = ddcorr;
    iso += ddcorr;
      }
      //if (eg.pt() > 25e3) ATH_MSG_DEBUG("ddcor = " << ddcorr << " new Iso = " << iso << "\n");
      bool setIso = eg.setIsolationValue(iso,type);
      setIso = (setIso && eg.setIsolationCaloCorrection(newleak-ddcorr,type,xAOD::Iso::ptCorrection));
      ATH_MSG_VERBOSE("oldeak = " << oldleak << " ddcor = " << ddcorr << " leak param = " << newleak
		      << " stored correction " << eg.isolationCaloCorrection(type,xAOD::Iso::ptCorrection));
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
    return m_isol_corr->GetDDCorrection(input, isol, m_ddVersion);
  }

  CP::CorrectionCode IsolationCorrectionTool::correctedCopy( const xAOD::Egamma& input, xAOD::Egamma*& output) {
    // A sanity check:
    if( output ) ATH_MSG_WARNING( "Non-null pointer received. There's a possible memory leak!" );

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
//     if (m_apply_ddDefault && m_tool_ver_str!="REL21")
//       result.insert( m_systDDonoff );
    return result;
  }

  CP::SystematicSet IsolationCorrectionTool::recommendedSystematics() const {
    return affectingSystematics();
  }

  CP::SystematicCode IsolationCorrectionTool::applySystematicVariation( const CP::SystematicSet& /* systConfig */ ) {
    return CP::SystematicCode::Ok;
  }

  IsolationCorrectionTool::~IsolationCorrectionTool() {
    delete m_isol_corr;
  }

}
