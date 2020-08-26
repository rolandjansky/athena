/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AthPhotonEfficiencyCorrectionTool
   @brief Calculate the photon scale factors in Athena

   @author Michael Pitt <michael.pitt@cern.ch>, Giovanni Marchiori
   @date   February 2018
*/

// Include this class's header
#include "PhotonEfficiencyCorrection/AsgPhotonEfficiencyCorrectionTool.h"

// STL includes
#include <string>
#include <cfloat>
#include <iostream>
#include <limits.h>

// Include the return object
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
#define MIN_ET_Trig_SF 10000.0
#define MAX_ET_OF_SF 1499999.99
#define MAX_ET_Iso_SF 999999.99
#define MAX_ET_Trig_SF 99999.99

typedef Root::TPhotonEfficiencyCorrectionTool::Result Result;


// =============================================================================
// Standard constructor
// =============================================================================
AsgPhotonEfficiencyCorrectionTool::AsgPhotonEfficiencyCorrectionTool( std::string myname ) : 
  AsgTool(myname),
  m_rootTool_unc(0),
  m_rootTool_con(0),
  m_appliedSystematics(0),
  m_sysSubstring("")
{

  // Create an instances of the underlying ROOT tools
  m_rootTool_unc = new Root::TPhotonEfficiencyCorrectionTool();
  m_rootTool_con = new Root::TPhotonEfficiencyCorrectionTool();

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
  declareProperty( "IsoKey",         m_isoWP="",   "Set isolation WP, if this string is empty the tool will return ID SF");
  
  // Properties needed for trigger SF
  declareProperty( "TriggerKey",         m_trigger="",   "Set trigger, if this string is empty the tool will return ID SF");
  
  // Properties related to the receiving of event run number
  declareProperty("UseRandomRunNumber",  m_useRandomRunNumber = true,
                                        "Set if use RandomRunNumber from eventinfo");
  declareProperty("DefaultRandomRunNumber",  m_defaultRandomRunNumber = 999999,
                                        "Set default run number manually");
										

}

// =============================================================================
// Standard destructor
// =============================================================================
AsgPhotonEfficiencyCorrectionTool::~AsgPhotonEfficiencyCorrectionTool()
{
  if ( m_rootTool_unc ) delete m_rootTool_unc;
  if ( m_rootTool_con ) delete m_rootTool_con;
}

// =============================================================================
// Athena initialize method
// =============================================================================
StatusCode AsgPhotonEfficiencyCorrectionTool::initialize()
{
  // Resolve the paths to the input files
  std::vector < std::string > corrFileNameList;

  // First check if the tool is initialized using the input files or map
  if(m_mapFile.size()){ // using map file
     corrFileNameList.push_back(getFileName(m_isoWP,m_trigger,true));	// converted photons input
	 corrFileNameList.push_back(getFileName(m_isoWP,m_trigger,false));  // unconverted photons input
  }
  else if(m_corrFileNameConv.size() && m_corrFileNameUnconv.size()){ // initialize the tool using input files (old scheme)
  	corrFileNameList.push_back(m_corrFileNameConv);
	corrFileNameList.push_back(m_corrFileNameUnconv);
  }
  else{
      ATH_MSG_ERROR ( "Fail to resolve input file name, check if you set MapFilePath or CorrectionFileName properly" );
      return StatusCode::FAILURE ; 
  }

  // once the input files are retrieved, update the path using PathResolver or TOOL/data folder
  for ( unsigned int i=0; i<corrFileNameList.size(); ++i ){

    //Using the PathResolver to locate the file
    std::string filename = PathResolverFindCalibFile( corrFileNameList.at(i) );

    if (filename.empty()){
      ATH_MSG_ERROR ( "Could NOT resolve file name " << corrFileNameList.at(i) );
      return StatusCode::FAILURE ;
    } else{
      ATH_MSG_INFO(" Using path = "<<filename);
    }

    corrFileNameList.at(i) = filename;

  }
   
  // Set prefix for sustematics if this is ISO, Trigger or ID SF
  if( corrFileNameList[0].find(m_file_prefix_ID) != std::string::npos) m_sysSubstring="ID_";
  if( corrFileNameList[0].find(m_file_prefix_ISO) != std::string::npos) m_sysSubstring="ISO_";
  if( corrFileNameList[0].find(m_file_prefix_Trig) != std::string::npos) m_sysSubstring="TRIGGER_";
  if(m_sysSubstring == "") {ATH_MSG_ERROR ( "Invalid input file" ); return StatusCode::FAILURE;}

  // Configure the underlying Root tool
  m_rootTool_con->addFileName( corrFileNameList[0] );
  m_rootTool_unc->addFileName( corrFileNameList[1] );

    // Forward the message level
  m_rootTool_con->msg().setLevel(this->msg().level());
  m_rootTool_unc->msg().setLevel(this->msg().level());

  
  // Check if ForceDataType is set up properly (should be 3 for AtlFast2)
  if(TString(corrFileNameList[0]).Contains("AFII") && m_dataTypeOverwrite!=3)
  {
      ATH_MSG_ERROR("Property ForceDataType is set to "<< m_dataTypeOverwrite << ", while it should be 3 for FastSim");
      return StatusCode::FAILURE;
  }
  if(!TString(corrFileNameList[0]).Contains("AFII") && m_dataTypeOverwrite!=1)
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
  

  // Add the recommended systematics to the registry
  if ( registerSystematics() != CP::SystematicCode::Ok) {
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS ;
}


// =============================================================================
// The main accept method: the actual cuts are applied here 
// =============================================================================
const Result AsgPhotonEfficiencyCorrectionTool::calculate( const xAOD::Egamma* egam ) const
{
  Result result;
  if ( !egam )
    {
      ATH_MSG_ERROR ( "Did NOT get a valid egamma pointer!" );
      return result;
    }
  
   // Get the run number
  const xAOD::EventInfo* eventInfo = evtStore()->retrieve< const xAOD::EventInfo> ("EventInfo");
  if(!eventInfo){
    ATH_MSG_ERROR ( "Could not retrieve EventInfo object!" );
    return result;
  }

  //Retrieve the proper random Run Number
  unsigned int runnumber = m_defaultRandomRunNumber;
  if (m_useRandomRunNumber) {
    static const SG::AuxElement::Accessor<unsigned int> randomrunnumber("RandomRunNumber");
        if (!randomrunnumber.isAvailable(*eventInfo)) {
          ATH_MSG_WARNING("Pileup tool not run before using PhotonEfficiencyTool! SFs do not reflect PU distribution in data");
          return result;
        }
        runnumber = randomrunnumber(*(eventInfo));
  }
  
  // Get the needed values (et, etas2, conv. flag) from the egamma object
  
  // check if converted
  bool isConv=xAOD::EgammaHelpers::isConvertedPhoton(egam);

  // retrieve transvrse energy from e/cosh(etaS2)
  const xAOD::CaloCluster* cluster  = egam->caloCluster(); 
  if (!cluster){
        ATH_MSG_ERROR("ERROR no cluster associated to the Photon \n"); 
        return result;
    }
  double eta2   = fabsf(cluster->etaBE(2));
  double et = egam->pt();
  	
  // Check if photon in the range to get the SF
  if(eta2>MAXETA) {
        ATH_MSG_WARNING( "No correction factor provided for eta "<<cluster->etaBE(2)<<" Returning SF = 1 + / - 1");
        return result;
  }
  if(et<MIN_ET_OF_SF && m_sysSubstring=="ID_") {
        ATH_MSG_WARNING( "No ID scale factor uncertainty provided for et "<<et/1e3<<"GeV Returning SF = 1 + / - 1");
        return result;
  }
  if(et<MIN_ET_Iso_SF && m_sysSubstring=="ISO_") {
        ATH_MSG_WARNING( "No isolation scale factor uncertainty provided for et "<<et/1e3<<"GeV Returning SF = 1 + / - 1");
        return result;
  }
  if(et<MIN_ET_Trig_SF && m_sysSubstring=="TRIGGER_") {
        ATH_MSG_WARNING( "No trigger scale factor uncertainty provided for et "<<et/1e3<<"GeV Returning SF = 1 + / - 1");
        return result;
  }
  if(et>MAX_ET_OF_SF && m_sysSubstring=="ID_") {
        ATH_MSG_WARNING( "No scale factor provided for et "<<et/1e3<<"GeV Returning SF for "<<MAX_ET_OF_SF/1e3<<"GeV");
        et=MAX_ET_OF_SF;
  }
  if(et>MAX_ET_Iso_SF && m_sysSubstring=="ISO_") {
        ATH_MSG_WARNING( "No isolation scale factor provided for et "<<et/1e3<<"GeV Returning SF for "<<MAX_ET_Iso_SF/1e3<<"GeV");
        et=MAX_ET_Iso_SF;
  }
  if(et>MAX_ET_Trig_SF && m_sysSubstring=="TRIGGER_") {
        ATH_MSG_WARNING( "No trigger scale factor provided for et "<<et/1e3<<"GeV Returning SF for "<<MAX_ET_Trig_SF/1e3<<"GeV");
        et=MAX_ET_Trig_SF;
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
  return isConv ? m_rootTool_con->calculate( dataType,runnumber,eta2,et /* in MeV */) : m_rootTool_unc->calculate( dataType,runnumber,eta2,et /* in MeV */);
  
}

const Result AsgPhotonEfficiencyCorrectionTool::calculate( const xAOD::IParticle *part ) const
{
  const xAOD::Egamma* egam = dynamic_cast<const xAOD::Egamma*>(part);
  if ( egam ){
      return calculate(egam);
    } 
  else{
      ATH_MSG_ERROR ( " Could not cast to const egamma pointer!" );
      Result dummy;
      return dummy;
    }
}

CP::CorrectionCode AsgPhotonEfficiencyCorrectionTool::getEfficiencyScaleFactor(const xAOD::Egamma& inputObject, double& efficiencyScaleFactor) const{

    const xAOD::CaloCluster* cluster  = inputObject.caloCluster();  
    if (!cluster){
        ATH_MSG_ERROR("No  cluster associated to the Photon \n"); 
        efficiencyScaleFactor=1;
        return  CP::CorrectionCode::Error;
    } 
    // if not in the range: return OutOfVelidityRange with SF = 1 +/- 1
  
    if(fabs(cluster->etaBE(2))>MAXETA || inputObject.pt()<MIN_ET){
    efficiencyScaleFactor=1;
    if(m_appliedSystematics!=nullptr) efficiencyScaleFactor+=appliedSystematics().getParameterByBaseName("PH_EFF_"+m_sysSubstring+"Uncertainty");
	return CP::CorrectionCode::OutOfValidityRange;
  }
  
  if(m_appliedSystematics==nullptr){
    efficiencyScaleFactor=calculate(&inputObject).scaleFactor;
    return  CP::CorrectionCode::Ok;
  }
  
  //Get the result + the uncertainty
  float sigma(0);
  sigma=appliedSystematics().getParameterByBaseName("PH_EFF_"+m_sysSubstring+"Uncertainty");
  efficiencyScaleFactor=calculate(&inputObject).scaleFactor+sigma*calculate(&inputObject).totalUncertainty;
  return  CP::CorrectionCode::Ok;
}

CP::CorrectionCode AsgPhotonEfficiencyCorrectionTool::getEfficiencyScaleFactorError(const xAOD::Egamma& inputObject, double& efficiencyScaleFactorError) const{   

  // if not in the range: return OutOfVelidityRange with SF = 1 +/- 1
  if(fabs((inputObject.caloCluster())->etaBE(2))>MAXETA || inputObject.pt()<MIN_ET){
    efficiencyScaleFactorError=1;
	return CP::CorrectionCode::OutOfValidityRange;
  }
  
  efficiencyScaleFactorError=calculate(&inputObject).totalUncertainty;
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
  if(m_appliedSystematics==nullptr) eff  = calculate(&inputObject).scaleFactor;
  else{ // if SF is up or down varies by sigma
    float sigma(0);
	sigma=appliedSystematics().getParameterByBaseName("PH_EFF_"+m_sysSubstring+"Uncertainty");
	eff=calculate(&inputObject).scaleFactor+sigma*calculate(&inputObject).totalUncertainty;
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
std::string AsgPhotonEfficiencyCorrectionTool::getFileName(std::string isoWP, std::string trigWP, bool isConv) {  

  // First locate the map file:
  std::string mapFileName = PathResolverFindCalibFile( m_mapFile );
  if(mapFileName.empty()){
	ATH_MSG_ERROR ( "Somthing wrong with reading the map file, check you input: " << m_mapFile );
	return mapFileName;	// return an empty string
  }
  
  // Construct correction type:
  std::string correction_type = "ID_Tight";
  if(!trigWP.empty()) correction_type = "Trigger_"+trigWP+"_"+isoWP;
  else if(!isoWP.empty()) correction_type = "ISO_"+isoWP;
  
  // trigger SF same for con/unc photons
  if(trigWP.empty()) {correction_type += isConv ? "_Converted" : "_Unconverted";}

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



