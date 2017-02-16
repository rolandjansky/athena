/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AthPhotonEfficiencyCorrectionTool
   @brief Calculate the photon scale factors in Athena

   @author Rob Roy Fletcher <rob.fletcher@cern.ch>, Karsten Koeneke, Michael Pitt
   @date   August 2014
*/

// Include this class's header
#include "PhotonEfficiencyCorrection/AsgPhotonEfficiencyCorrectionTool.h"

// STL includes
#include <string>
#include <cfloat>
//#include <climits>
#include <iostream>
#include <limits.h>

// Include the return object
#include "PATCore/TResult.h"
#include "PATCore/PATCoreEnums.h"

// xAOD includes
#include "xAODEgamma/Egamma.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "PathResolver/PathResolver.h"


// ROOT includes
#include "TSystem.h"

#define MAXETA 2.47
#define MIN_ET 10000.0
#define MIN_ET_OF_SF 10000.0
#define MIN_ET_Iso_SF 10000.0
#define MAX_ET_OF_SF 2999999.9

// =============================================================================
// Standard constructor
// =============================================================================
AsgPhotonEfficiencyCorrectionTool::AsgPhotonEfficiencyCorrectionTool( std::string myname ) : 
  AsgTool(myname),
  m_rootTool_unc(0),
  m_rootTool_con(0),
  m_rootTool_uncRadZ(0),
  m_rootTool_conRadZ(0),  
  m_appliedSystematics(0),
  m_sysSubstring(""),
  m_sysSubstringRadZ("")
{

  // Create an instances of the underlying ROOT tools
  m_rootTool_unc = new Root::TPhotonEfficiencyCorrectionTool();
  m_rootTool_con = new Root::TPhotonEfficiencyCorrectionTool();
  m_rootTool_uncRadZ = new Root::TPhotonEfficiencyCorrectionTool();
  m_rootTool_conRadZ = new Root::TPhotonEfficiencyCorrectionTool();

  // Declare the needed properties
  declareProperty( "CorrectionFileNameConv", m_corrFileNameConv="",
                   "File that stores the correction factors for simulation for converted photons");

  declareProperty( "CorrectionFileNameUnconv", m_corrFileNameUnconv="",
                   "File that stores the correction factors for simulation for unconverted photons");
				   
  declareProperty("MapFilePath", m_mapFile = "" ,
                  "Full path to the map file");  
				  
  declareProperty( "ForceDataType", m_dataTypeOverwrite=-1,
                   "Force the DataType of the Photon to specified value");

  declareProperty( "ResultPrefix",       m_resultPrefix="", "The prefix string for the result");
  declareProperty( "ResultName",         m_resultName="",   "The string for the result");
  
  // Properties needed for isolation corrections
  declareProperty( "IsoWP",         m_isoWP="",   "Set isolation WP, if this string is empty the tool will return ID SF");
  declareProperty( "Threshold_lowPT",         m_Threshold_lowPT=25.0,    "low pT threshold for ISO SF(default=25)");
  declareProperty( "Threshold_highPT",        m_Threshold_highPT=100.0,   "high pT threshold for ISO SF (default=100)");
  declareProperty( "UseRadiativeZSF_mediumPT",m_UseRadZ_mediumPT=false,   "use RadZ derived SF (default=true)");

}

// =============================================================================
// Standard destructor
// =============================================================================
AsgPhotonEfficiencyCorrectionTool::~AsgPhotonEfficiencyCorrectionTool()
{
  if(finalize().isFailure()){
    ATH_MSG_ERROR ( "Failure in AsgPhotonEfficiencyCorrectionTool finalize()");
  }

  if ( m_rootTool_unc ) delete m_rootTool_unc;
  if ( m_rootTool_con ) delete m_rootTool_con;
  if ( m_rootTool_uncRadZ ) delete m_rootTool_uncRadZ;
  if ( m_rootTool_conRadZ ) delete m_rootTool_conRadZ;  
}

// =============================================================================
// Athena initialize method
// =============================================================================
StatusCode AsgPhotonEfficiencyCorrectionTool::initialize()
{
  // Resolve the paths to the input files
  std::vector < std::string > m_corrFileNameList;

  // First check if the tool is initialized using the input files or map
  if(m_mapFile.size()){ // using map file
     m_corrFileNameList.push_back(getFileName(m_isoWP,true,""));	// converted photons input
	 m_corrFileNameList.push_back(getFileName(m_isoWP,false,""));  // unconverted photons input
	// if isolation SF are initialized, then read RadZ inputs:
	if(m_isoWP.size()){
     m_corrFileNameList.push_back(getFileName(m_isoWP,true,"_RadZ"));	// converted photons input
	 m_corrFileNameList.push_back(getFileName(m_isoWP,false,"_RadZ"));  // unconverted photons input	
	}
  }
  else if(m_corrFileNameConv.size() && m_corrFileNameUnconv.size()){ // initialize the tool using input files (old scheme)
  	m_corrFileNameList.push_back(m_corrFileNameConv);
	m_corrFileNameList.push_back(m_corrFileNameUnconv);
  }
  else{
      ATH_MSG_ERROR ( "Fail to resolve input file name, check if you set MapFilePath or CorrectionFileName properly" );
      return StatusCode::FAILURE ; 
  }

  // once the input files are retrieved, update the path using PathResolver or TOOL/data folder
  for ( unsigned int i=0; i<m_corrFileNameList.size(); ++i ){

    //First try the PathResolver
    std::string filename = PathResolverFindCalibFile( m_corrFileNameList.at(i) );

    // ROOTCore: Data folder
    char *rootCoreArea = getenv("ROOTCOREBIN");
    if(filename.empty() && rootCoreArea != NULL){
      filename = std::string(rootCoreArea) + "/data/"+ m_corrFileNameList.at(i);
    }  

    if (filename.empty()){
      ATH_MSG_ERROR ( "Could NOT resolve file name " << m_corrFileNameList.at(i) );
      return StatusCode::FAILURE ;
    } else{
      ATH_MSG_INFO(" Path found = "<<filename);
    }

    m_corrFileNameList.at(i) = filename;

  }
   
  // Set prefix for sustematics if this is ISO or ID SF
  if( m_corrFileNameList[0].find("offline.Tight") != std::string::npos) m_sysSubstring="ID_";
  if( m_corrFileNameList[0].find("Isolation.isolFixedCut") != std::string::npos) {m_sysSubstring="TRKISO_"; m_sysSubstringRadZ="LOWPTISO_";}
  if(m_sysSubstring == "") {ATH_MSG_ERROR ( "Invalid input file" ); return StatusCode::FAILURE;}

  // Configure the underlying Root tool
  m_rootTool_con->addFileName( m_corrFileNameList[0] );
  m_rootTool_unc->addFileName( m_corrFileNameList[1] );

  m_rootTool_con->setResultPrefix( m_resultPrefix );
  m_rootTool_con->setResultName( m_resultName );
  m_rootTool_unc->setResultPrefix( m_resultPrefix );
  m_rootTool_unc->setResultName( m_resultName );  
  
  // Forward the message level
  m_rootTool_con->msg().setLevel(this->msg().level());
  m_rootTool_unc->msg().setLevel(this->msg().level());

  
  // Check if ForceDataType is set up properly (should be 3 for AtlFast2)
  if(TString(m_corrFileNameList[0]).Contains("AFII") && m_dataTypeOverwrite!=3)
  {
      ATH_MSG_ERROR("Property ForceDataType is set to "<< m_dataTypeOverwrite << ", while it should be 3 for FastSim");
      return StatusCode::FAILURE;
  }
  if(!TString(m_corrFileNameList[0]).Contains("AFII") && m_dataTypeOverwrite!=1)
  {
      ATH_MSG_ERROR("Property ForceDataType is set to "<< m_dataTypeOverwrite << ", while it should be 1 for FullSim");
      return StatusCode::FAILURE;
  }  
  
  // We need to initialize the underlying ROOT TSelectorTool
  if ( (0 == m_rootTool_con->initialize()) || (0 == m_rootTool_unc->initialize()) )
    {
      ATH_MSG_ERROR("Could not initialize the TPhotonEfficiencyCorrectionTool!");
      return StatusCode::FAILURE;
    }
  
  // For isolation SF initialize additional instances for lowPT photons
  if(m_isoWP.size()){
    m_rootTool_conRadZ->addFileName( m_corrFileNameList[2] );
    m_rootTool_uncRadZ->addFileName( m_corrFileNameList[3] ); 
    m_rootTool_conRadZ->setResultPrefix( m_resultPrefix );
    m_rootTool_conRadZ->setResultName( m_resultName );
    m_rootTool_uncRadZ->setResultPrefix( m_resultPrefix );
    m_rootTool_uncRadZ->setResultName( m_resultName );  
    m_rootTool_conRadZ->msg().setLevel(this->msg().level());
    m_rootTool_uncRadZ->msg().setLevel(this->msg().level());
    if ( (0 == m_rootTool_conRadZ->initialize()) || (0 == m_rootTool_uncRadZ->initialize()) )
    {
      ATH_MSG_ERROR("Could not initialize the TPhotonEfficiencyCorrectionTool!");
      return StatusCode::FAILURE;
    }
  }

  // Copy the now filled TResult to the dummy
  m_resultDummy = m_rootTool_con->getTResult();  // do only for converted photons instance

  // Add the recommended systematics to the registry
  if ( registerSystematics() != CP::SystematicCode::Ok) {
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS ;
}


// =============================================================================
// Athena finalize method
// =============================================================================
StatusCode AsgPhotonEfficiencyCorrectionTool::finalize()
{
  if ( !(m_rootTool_con->finalize()) || !(m_rootTool_unc->finalize()) )
    {
      ATH_MSG_ERROR("Something went wrong at finalize!");
      return StatusCode::FAILURE;
    }
  // finilize instances in case of IsolationSF	
  if(m_isoWP.size()){
    if ( !(m_rootTool_conRadZ->finalize()) || !(m_rootTool_uncRadZ->finalize()) )
    {
      ATH_MSG_ERROR("Something went wrong at finalize!");
      return StatusCode::FAILURE;
    }
  }  

  return StatusCode::SUCCESS ;
}


// =============================================================================
// The main accept method: the actual cuts are applied here 
// =============================================================================
const Root::TResult& AsgPhotonEfficiencyCorrectionTool::calculate( const xAOD::Egamma* egam ) const
{
  if ( !egam )
    {
      ATH_MSG_ERROR ( "Did NOT get a valid egamma pointer!" );
      return m_resultDummy;
    }
  
  // Get the run number 
  const xAOD::EventInfo* eventInfo = evtStore()->retrieve< const xAOD::EventInfo> ("EventInfo");
  if(!eventInfo){
    ATH_MSG_ERROR ( "Could not retrieve EventInfo object!" );
    return m_resultDummy;
  }
  const unsigned int runnumber = eventInfo->runNumber();

  // Get the needed values (et, etas2, conv. flag) from the egamma object
  
  // check if converted
  bool isConv=xAOD::EgammaHelpers::isConvertedPhoton(egam);

  // retrieve transvrse energy from e/cosh(etaS2)
  const xAOD::CaloCluster* cluster  = egam->caloCluster(); 
  double eta2   = fabsf(cluster->etaBE(2));
  double et = egam->pt();
  	
  // Check if photon in the range to get the SF
  if(eta2>MAXETA) {
	ATH_MSG_WARNING( "No correction factor provided for eta "<<cluster->etaBE(2)<<" Returning SF = 1 + / - 1");
	return m_resultDummy;
  }
  if(et<MIN_ET_OF_SF && m_sysSubstring=="ID_") {
	ATH_MSG_WARNING( "No ID scale factor uncertainty provided for et "<<et/1e3<<"GeV Returning SF = 1 + / - 1");
	return m_resultDummy;
  } 
  if(m_sysSubstring=="TRKISO_" && et<MIN_ET_Iso_SF) {
	ATH_MSG_WARNING( "No isolation scale factor uncertainty provided for et "<<et/1e3<<"GeV Returning SF = 1 + / - 1");
	return m_resultDummy;
  }   
  if(et>MAX_ET_OF_SF) {
	ATH_MSG_WARNING( "No scale factor provided for et "<<et/1e3<<"GeV Returning SF for "<<MAX_ET_OF_SF/1e3<<"GeV");
	et=MAX_ET_OF_SF;
  }   
  
  // Get the DataType of the current egamma object
//!  PATCore::ParticleDataType::DataType dataType = (PATCore::ParticleDataType::DataType) (egam->dataType());
//!  ATH_MSG_VERBOSE( "The egamma object with author=" << egam->author()
//!                   << " has PATCore::ParticleDataType::DataType=" << dataType
//!                   << " and EventKernel::ParticleDataType::DataType=" << egam->dataType() );
  /* For now the dataType must be set by the user. May be added to the IParticle class later.  */
  // probably event info should be able to tell us if it's data, fullsim, AF,..
  PATCore::ParticleDataType::DataType dataType = PATCore::ParticleDataType::DataType::Data;
  if ( m_dataTypeOverwrite >= 0 ) dataType = (PATCore::ParticleDataType::DataType)m_dataTypeOverwrite;

  // Call the ROOT tool to get an answer, check if the SF is for isolation or ID
  if(m_isoWP.size()){
    double RadZ_ptcut = m_UseRadZ_mediumPT ? m_Threshold_highPT : m_Threshold_lowPT;
    if(et<RadZ_ptcut*1e3)
	  return isConv ? m_rootTool_conRadZ->calculate( dataType,runnumber,eta2,et /* in MeV */) : m_rootTool_uncRadZ->calculate( dataType,runnumber,eta2,et /* in MeV */);
	else
	  return isConv ? m_rootTool_con->calculate( dataType,runnumber,eta2,et /* in MeV */) : m_rootTool_unc->calculate( dataType,runnumber,eta2,et /* in MeV */);
  }
  return isConv ? m_rootTool_con->calculate( dataType,runnumber,eta2,et /* in MeV */) : m_rootTool_unc->calculate( dataType,runnumber,eta2,et /* in MeV */);
  
}

const Root::TResult& AsgPhotonEfficiencyCorrectionTool::calculate( const xAOD::IParticle *part ) const
{
  const xAOD::Egamma* egam = dynamic_cast<const xAOD::Egamma*>(part);
  if ( egam )
    {
      return calculate(egam);
    } 
  else
    {
      ATH_MSG_ERROR ( " Could not cast to const egamma pointer!" );
      return m_resultDummy;
    }
}

CP::CorrectionCode AsgPhotonEfficiencyCorrectionTool::getEfficiencyScaleFactor(const xAOD::Egamma& inputObject, double& efficiencyScaleFactor) const{
   
  // if not in the range: return OutOfVelidityRange with SF = 1 +/- 1
  if(fabs((inputObject.caloCluster())->etaBE(2))>MAXETA || inputObject.pt()<MIN_ET){
    efficiencyScaleFactor=1;
    if(m_appliedSystematics!=nullptr) efficiencyScaleFactor+=appliedSystematics().getParameterByBaseName("PH_EFF_"+m_sysSubstring+"Uncertainty");
	return CP::CorrectionCode::OutOfValidityRange;
  }
  
  if(m_appliedSystematics==nullptr){
    efficiencyScaleFactor=calculate(&inputObject).getScaleFactor();
    return  CP::CorrectionCode::Ok;
  }
  
  //Get the result + the uncertainty
  float m_sigma(0);
  // will check if it isolation SF and if it correspong to low or high PT
  if(m_isoWP.size() && (inputObject.pt()<((m_UseRadZ_mediumPT ? m_Threshold_highPT : m_Threshold_lowPT)*1e3)))
    m_sigma=appliedSystematics().getParameterByBaseName("PH_EFF_"+m_sysSubstringRadZ+"Uncertainty");
  else
    m_sigma=appliedSystematics().getParameterByBaseName("PH_EFF_"+m_sysSubstring+"Uncertainty");
  efficiencyScaleFactor=calculate(&inputObject).getScaleFactor()+m_sigma*calculate(&inputObject).getTotalUncertainty();
  return  CP::CorrectionCode::Ok;
}

CP::CorrectionCode AsgPhotonEfficiencyCorrectionTool::getEfficiencyScaleFactorError(const xAOD::Egamma& inputObject, double& efficiencyScaleFactorError) const{   

  // if not in the range: return OutOfVelidityRange with SF = 1 +/- 1
  if(fabs((inputObject.caloCluster())->etaBE(2))>MAXETA || inputObject.pt()<MIN_ET){
    efficiencyScaleFactorError=1;
	return CP::CorrectionCode::OutOfValidityRange;
  }
  
  efficiencyScaleFactorError=calculate(&inputObject).getTotalUncertainty();
  return  CP::CorrectionCode::Ok;
}

CP::CorrectionCode AsgPhotonEfficiencyCorrectionTool::applyEfficiencyScaleFactor(xAOD::Egamma& inputObject) const {
   
  // if not in the range: return OutOfVelidityRange with SF = 1 +/- 1
  if(fabs((inputObject.caloCluster())->etaBE(2))>MAXETA || inputObject.pt()<MIN_ET){
    ATH_MSG_INFO("decorate object");
	inputObject.auxdata< float >( m_resultPrefix+m_resultName+"SF" ) = 1.0;
	if(m_appliedSystematics != nullptr) inputObject.auxdata< float >( m_resultPrefix+m_resultName+"SF" ) =1.0 + appliedSystematics().getParameterByBaseName("PH_EFF_"+m_sysSubstring+"Uncertainty");
	return CP::CorrectionCode::OutOfValidityRange;
  }
  
  float eff;
  if(m_appliedSystematics==nullptr) eff  = calculate(&inputObject).getScaleFactor();
  else{ // if SF is up or down varies by m_sigma
    float m_sigma(0);
	m_sigma=appliedSystematics().getParameterByBaseName("PH_EFF_"+m_sysSubstring+"Uncertainty");
	eff=calculate(&inputObject).getScaleFactor()+m_sigma*calculate(&inputObject).getTotalUncertainty();
  }
  // decorate photon
  ATH_MSG_INFO("decorate object");
  inputObject.auxdata< float >( m_resultPrefix+m_resultName+"SF" ) = eff;
  return  CP::CorrectionCode::Ok;
}

//=======================================================================
//   Systematics Interface
//=======================================================================
bool AsgPhotonEfficiencyCorrectionTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const {
	if(!systematic.empty()){
		CP::SystematicSet sys = affectingSystematics();
		return sys.find(systematic) != sys.end();
	}
	return true;
}


/// returns: the list of all systematics this tool can be affected by (for now keep +-1 sigma variation, but ignore it later in applySystematicVariation() )
CP::SystematicSet AsgPhotonEfficiencyCorrectionTool::affectingSystematics() const {
  CP::SystematicSet mySysSet;
 
  mySysSet.insert(CP::SystematicVariation("PH_EFF_"+m_sysSubstring+"Uncertainty", CP::SystematicVariation::CONTINUOUS));
  mySysSet.insert(CP::SystematicVariation("PH_EFF_"+m_sysSubstring+"Uncertainty", 1));
  mySysSet.insert(CP::SystematicVariation("PH_EFF_"+m_sysSubstring+"Uncertainty", -1));
  
  // Add systematics for lowPT isolation
  if(m_isoWP.size()){
  mySysSet.insert(CP::SystematicVariation("PH_EFF_"+m_sysSubstringRadZ+"Uncertainty", CP::SystematicVariation::CONTINUOUS));
  mySysSet.insert(CP::SystematicVariation("PH_EFF_"+m_sysSubstringRadZ+"Uncertainty", 1));
  mySysSet.insert(CP::SystematicVariation("PH_EFF_"+m_sysSubstringRadZ+"Uncertainty", -1));
  }
  
  return mySysSet;
}

/// Register the systematics with the registry and add them to the recommended list
CP::SystematicCode AsgPhotonEfficiencyCorrectionTool::registerSystematics() {
  CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  if (registry.registerSystematics(*this) != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Failed to add systematic to list of recommended systematics.");
	return CP::SystematicCode::Unsupported;
  }
  return CP::SystematicCode::Ok;
}

/// returns: the list of all systematics this tool recommends to use
CP::SystematicSet AsgPhotonEfficiencyCorrectionTool::recommendedSystematics() const {
  CP::SystematicSet mySysSet;
  mySysSet.insert(CP::SystematicVariation("PH_EFF_"+m_sysSubstring+"Uncertainty", 1));
  mySysSet.insert(CP::SystematicVariation("PH_EFF_"+m_sysSubstring+"Uncertainty", -1));
  
  // Add systematics for lowPT isolation
  if(m_isoWP.size()){
  mySysSet.insert(CP::SystematicVariation("PH_EFF_"+m_sysSubstringRadZ+"Uncertainty", 1));
  mySysSet.insert(CP::SystematicVariation("PH_EFF_"+m_sysSubstringRadZ+"Uncertainty", -1));
  }
  
  return mySysSet;
}


CP::SystematicCode AsgPhotonEfficiencyCorrectionTool::
applySystematicVariation ( const CP::SystematicSet& systConfig )
{
  // First, check if we already know this systematic configuration
  auto itr = m_systFilter.find(systConfig);

  // If it's a new input set, we need to filter it
  if( itr == m_systFilter.end() ){

  // New systematic. We need to parse it. MP 26.11.15 remove the static from <<< static CP::SystematicSet affectingSys = affectingSystematics(); >>>
    CP::SystematicSet affectingSys = affectingSystematics();
    CP::SystematicSet filteredSys;   
	if (!CP::SystematicSet::filterForAffectingSystematics(systConfig, affectingSys, filteredSys)){
      ATH_MSG_ERROR("Unsupported combination of systematics passed to the tool!");
      return CP::SystematicCode::Unsupported;
    }
	
    // Insert filtered set into the map
    itr = m_systFilter.insert(std::make_pair(systConfig, filteredSys)).first;
  }

  CP::SystematicSet& mySysConf = itr->second;
  m_appliedSystematics = &mySysConf;
  return CP::SystematicCode::Ok;
}

//===============================================================================
// Map Key Feature
//===============================================================================
// Gets the correction filename from map
std::string AsgPhotonEfficiencyCorrectionTool::getFileName(std::string isoWP, bool isConv, std::string sufix) {
  
  // Check if isoWP is TightCaloOnly, then will read the inputs from RadZ files
  if(0==isoWP.compare("TightCaloOnly")) sufix="_RadZ";
  
  // First locate the map file:
  std::string mapFileName = PathResolverFindCalibFile( m_mapFile );
  if(mapFileName.empty()){
	ATH_MSG_ERROR ( "Somthing wrong with reading the map file, check you input: " << m_mapFile );
	return mapFileName;	// return an empty string
  }
  
  std::string correction_type = isoWP.empty() ? "ID_Tight" : "ISO_"+isoWP+sufix;
  correction_type += isConv ? "_Converted" : "_Unconverted";
  std::string value;
  
  // Read the map file to find the proper correction filename
  std::ifstream is(mapFileName);
  if (!is.is_open()){
    ATH_MSG_ERROR("Couldn't read Map File" + mapFileName);
	return  "";
  }
  while (!is.eof()) {
    std::string strLine;
	getline(is,strLine);
	
	int nPos = strLine.find('=');
	
	if ((signed int)std::string::npos == nPos) continue; // no '=', invalid line;
	std::string strKey = strLine.substr(0,nPos);
	std::string strVal = strLine.substr(nPos + 1, strLine.length() - nPos + 1);
	
	// check if this is the right key, if the right one stop the search
	if(0==correction_type.compare(strKey)) {value=strVal; break;}
  }
  
  return value;

}



