/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AthElectronEfficiencyCorrectionTool
   @brief Calculate the egamma scale factors in Athena

   @author Rob Roy Fletcher <rob.fletcher@cern.ch>, Karsten Koeneke
   @date   May 2014
*/

// Include this class's header
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"

// STL includes
#include <string>
#include <cfloat>
//#include <climits>
#include <iomanip>
#include <iostream>
#include <limits.h>

// Include the return object
#include "PATCore/TResult.h"
#include "PATCore/PATCoreEnums.h"

// xAOD includes
#include "xAODEgamma/Electron.h"
#include "xAODEventInfo/EventInfo.h"
#include "PathResolver/PathResolver.h"


// ROOT includes
#include "TSystem.h"



// =============================================================================
// Standard constructor
// =============================================================================
AsgElectronEfficiencyCorrectionTool::AsgElectronEfficiencyCorrectionTool( std::string myname ) :
  AsgTool(myname),
  m_rootTool(0),
  m_appliedSystematics(0),
  m_sysSubstring(""),
//  m_pteta_bins(0),
  m_UncorrRegions(0),
  m_nSimpleUncorrSyst(0)
{

  // Create an instance of the underlying ROOT tool
  m_rootTool = new Root::TElectronEfficiencyCorrectionTool( ("T"+(this->name())).c_str() );

  // Declare the needed properties
  declareProperty( "CorrectionFileNameList", m_corrFileNameList,
                   "List of file names that store the correction factors for simulation.");

  declareProperty( "ForceDataType", m_dataTypeOverwrite=-1,
                   "Force the DataType of the electron to specified value (to circumvent problem of incorrect DataType for forward electrons in some old releases)");

  declareProperty( "ResultPrefix",       m_resultPrefix="", "The prefix string for the result");
  declareProperty( "ResultName",         m_resultName="",                "The string for the result");

  declareProperty("CorrelationModel", m_correlation_model_name = "TOTAL","Uncertainty correlation model. At the moment FULL, SIMPLIFIED, SYST, MCTOYS and COMBMCTOYS are implemented. TOTAL is the default option.");
  declareProperty("NumberOfToys", m_number_of_toys = 100,"Number of ToyMC replica, affecting MCTOYS and COMBMCTOYS correlation models only.");
  declareProperty("MCToySeed", m_seed_toys = 0,"Seed for ToyMC replica, affecting MCTOYS and COMBMCTOYS correlation models only." );
  declareProperty("MCToyScale", m_scale_toys = 1,"Scales Toy systematics up by this factor, affecting MCTOYS and COMBMCTOYS correlation models only." );

}

// =============================================================================
// Standard destructor
// =============================================================================
AsgElectronEfficiencyCorrectionTool::~AsgElectronEfficiencyCorrectionTool()
{
  if(finalize().isFailure()){
    ATH_MSG_ERROR ( "Failure in AsgElectronEfficiencyCorrectionTool finalize()");
  }
  delete m_rootTool;
}

// =============================================================================
// Athena initialize method
// =============================================================================
StatusCode AsgElectronEfficiencyCorrectionTool::initialize()
{

 // Forward the message level
 m_rootTool->msg().setLevel(this->msg().level());
 //   }
  // Do some consistency checks
  bool allOK(true);
  if ( m_corrFileNameList.empty() )
    {
      ATH_MSG_ERROR ( "FullSimCorrectionFileNameList is empty! Please configure it properly..." );
      allOK = false;
    }

  // Stop here if the user configuration is wrong
  if ( !allOK ) return StatusCode::FAILURE;


  // Resolve the paths to the input files for the full Geant4 simualtion corrections
  for ( unsigned int i=0; i<m_corrFileNameList.size(); ++i ){
    //First try the PathResolver
    std::string filename = PathResolverFindCalibFile( m_corrFileNameList.at(i) );
    // ROOTCore: Data folder
    char *rootCoreArea = getenv("ROOTCOREBIN");
    if(filename.empty() && rootCoreArea != NULL){
      filename = std::string(rootCoreArea) + "/data/"+ m_corrFileNameList.at(i);
    }
    if (filename.empty()){
      ATH_MSG_WARNING ( "Could NOT resolve file name " << m_corrFileNameList.at(i) );
    }  else{
      ATH_MSG_INFO(" Path found = "<<filename);
    }

    m_corrFileNameList.at(i) = filename;

  }

  // Configure the underlying Root tool
  for ( unsigned int i=0; i<m_corrFileNameList.size(); ++i )
    {
      m_rootTool->addFileName( m_corrFileNameList[i] );

      // Determine the systematics substring according to the name of the input file
      if(m_corrFileNameList[i].find("efficiencySF.") != std::string::npos){
	m_sysSubstring = "Trigger_";
      }
      if(m_corrFileNameList[i].find("efficiencySF.offline") != std::string::npos){
	m_sysSubstring = "ID_";
      }
      if(m_corrFileNameList[i].find("efficiencySF.offline.RecoTrk") != std::string::npos){
	m_sysSubstring = "Reco_";
      }
      if(m_corrFileNameList[i].find("efficiencySF.Isolation") != std::string::npos){
	m_sysSubstring = "Iso_";
      }
      if(m_corrFileNameList[i].find("efficiency.") != std::string::npos){
	m_sysSubstring = "TriggerEff_";
      }
      if(m_sysSubstring == "") return StatusCode::FAILURE;
    }

  m_rootTool->setResultPrefix( m_resultPrefix );
  m_rootTool->setResultName( m_resultName );


  if (m_correlation_model_name != "COMBMCTOYS" 
      && m_correlation_model_name != "MCTOYS" 
      && m_correlation_model_name != "FULL" 
      && m_correlation_model_name != "SIMPLIFIED" 
      && m_correlation_model_name != "TOTAL"
      && m_correlation_model_name != "SYST"
      )    {
    ATH_MSG_ERROR("Unknown correlation model "+m_correlation_model_name);      
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Correlation model: "+m_correlation_model_name);
  
  if (m_correlation_model_name == "SIMPLIFIED" ) {

    // cretate uncorrelated eta/pt bins
    // 
    std::vector<float> eta;
    eta.push_back(0);
    eta.push_back(1.37);
    eta.push_back(2.47);

    std::vector<float> pt;
    pt.push_back(7000);
    pt.push_back(10000);
    pt.push_back(15000);
    pt.push_back(20000);
    pt.push_back(25000);
    pt.push_back(30000);
    pt.push_back(60000);
    pt.push_back(80000);
    pt.push_back(13000000);

    m_UncorrRegions = new TH2F("UncorrRegions","UncorrRegions",pt.size()-1, &(pt[0]),eta.size()-1, &(eta[0]));
    m_nSimpleUncorrSyst = eta.size() * pt.size();

  }

  if (m_seed_toys != 0) m_rootTool->setSeed(m_seed_toys);
  
  if (m_correlation_model_name == "COMBMCTOYS") m_rootTool->bookCombToyMCScaleFactors(m_number_of_toys); 
  if (m_correlation_model_name == "MCTOYS") m_rootTool->bookToyMCScaleFactors(m_number_of_toys); 
    
  // We need to initialize the underlying ROOT TSelectorTool
  if ( 0 == m_rootTool->initialize() )
    {
      ATH_MSG_ERROR("Could not initialize the TElectronEfficiencyCorrectionTool!");
      return StatusCode::FAILURE;
    }
  // Copy the now filled TResult to the dummy
  m_resultDummy = m_rootTool->getTResult();
  // get Nsyst
  m_nCorrSyst = m_rootTool->getNSyst() ;

  if (m_correlation_model_name == "FULL" ) {
    m_nUncorrSyst =  m_rootTool->getNbins(m_pteta_bins);
  }

  // Add the recommended systematics to the registry
  if ( registerSystematics() != CP::SystematicCode::Ok) {
    return StatusCode::FAILURE;
  }

  // Configure for nominal systematics
  if(applySystematicVariation(CP::SystematicSet()) != CP::SystematicCode::Ok){
    ATH_MSG_ERROR("Could not configure for nominal settings");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS ;
}


// =============================================================================
// Athena finalize method
// =============================================================================
StatusCode AsgElectronEfficiencyCorrectionTool::finalize()
{
  if ( !(m_rootTool->finalize()) )
    {
      ATH_MSG_ERROR("Something went wrong at finalize!");
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS ;
}


// =============================================================================
// The main accept method: the actual cuts are applied here : USE OLD TOOL
// =============================================================================
const Root::TResult& AsgElectronEfficiencyCorrectionTool::calculate( const xAOD::Electron& egam, int &currentSimplifiedUncorrSystRegion, int& currentUncorrSystRegion ) const
{
  

  // Here, you have to make the translation from the ATLAS EDM object to the variables of simple type

  // Get the run number 
  const xAOD::EventInfo* eventInfo = evtStore()->retrieve< const xAOD::EventInfo> ("EventInfo");
  if(!eventInfo){
    ATH_MSG_ERROR ( "Could not retrieve EventInfo object!" );
    return m_resultDummy;
  }
 // const unsigned int MCrunnumber = eventInfo->runNumber();
// m_eventInfo->auxdata<unsigned int>("RandomRunNumber");
  
  static SG::AuxElement::Decorator<unsigned int> randomrunnumber("RandomRunNumber") ;
unsigned int runnumber = 999999;

  if(randomrunnumber.isAvailable(*(eventInfo)))
	runnumber = randomrunnumber(*(eventInfo));
  else { runnumber =  eventInfo->runNumber();//999999;
    ATH_MSG_WARNING ( "Pileup tool not run before using ElectronEfficiencyTool! SFs do not reflect PU distribution in data" );
}
//if ()
  //(or see the RandomRunNumber unsigned int decoration after apply) 
  // Get the needed values from the egamma object
  double cluster_eta(-9999.9);
  double et(0.0);
  et = egam.pt();
  const xAOD::CaloCluster* cluster = egam.caloCluster();
  if ( cluster ) {
    cluster_eta = cluster->etaBE(2); 
  }

  /* For now the dataType must be set by the user. May be added to the IParticle class later.  */
  PATCore::ParticleDataType::DataType dataType = PATCore::ParticleDataType::DataType::Data;
  if ( m_dataTypeOverwrite >= 0 ) dataType = (PATCore::ParticleDataType::DataType)m_dataTypeOverwrite;

  if (m_correlation_model_name == "SIMPLIFIED" ) {
    int ptbin=m_UncorrRegions->GetXaxis()->FindBin(et)-1;
    int etabin=m_UncorrRegions->GetXaxis()->FindBin(cluster_eta)-1;
    int reg = ((etabin)*m_UncorrRegions->GetNbinsX()+ptbin);
    currentSimplifiedUncorrSystRegion=reg;
  }
  if (m_correlation_model_name == "FULL" ) {
    int etabin=-1;
    int reg=0;
    bool found=false;

    std::map<float,std::vector<float>>::const_iterator itr_ptBEGIN = m_pteta_bins.begin();
    std::map<float,std::vector<float>>::const_iterator itr_ptEND = m_pteta_bins.end();

    for (  ; itr_ptBEGIN!=itr_ptEND;  itr_ptBEGIN++ ) {
      
      std::map<float,std::vector<float>>::const_iterator itr_ptBEGINplusOne =  itr_ptBEGIN;  
      itr_ptBEGINplusOne++;
      
      if ( (et > itr_ptBEGIN->first && itr_ptBEGINplusOne==itr_ptEND ) || (et > itr_ptBEGIN->first && et<itr_ptBEGINplusOne->first )) {//find the pt bin
	etabin++;
	for (unsigned int etab=0;etab<((itr_ptBEGIN->second).size()-1) ; etab++){//find the eta bin
	  if ( (cluster_eta) > (itr_ptBEGIN->second).at(etab) &&   (cluster_eta) < (itr_ptBEGIN->second).at(etab+1)) {
	    found=true;
	    break;
	  }
	}
      }
      if (found) {
	break;
      }
      reg=reg+(itr_ptBEGIN->second).size();
    }
    
    reg = reg+etabin;
    //std::cout << reg<< std::endl;
    currentUncorrSystRegion =reg;
  }

  // Call the ROOT tool to get an answer
  return m_rootTool->calculate( dataType,
                                runnumber,
                                cluster_eta,
                                et /* in MeV */
                                );
  
}

// =============================================================================
// TRANSFER RESULT OF UNDERLYING TOOL TO xAOD TOOL
// =============================================================================

CP::CorrectionCode AsgElectronEfficiencyCorrectionTool::getEfficiencyScaleFactor(const xAOD::Electron& inputObject, double& efficiencyScaleFactor) const{

  int currentUncorrSystRegion=0;
  int currentSimplifiedUncorrSystRegion=0;
  const Root::TResult m_result = calculate(inputObject,currentSimplifiedUncorrSystRegion,currentUncorrSystRegion  );
  if(appliedSystematics().empty() ) {
    efficiencyScaleFactor = m_result.getScaleFactor();
    return CP::CorrectionCode::Ok;
  }
  
  //Setup Toys
  //Get the result + all of the uncertainties. These will be zero until applySystematicVariation() is called.
  double m_sys(0);        
  if (m_correlation_model_name == "MCTOYS" || m_correlation_model_name == "COMBMCTOYS") {
    // Check if more than one set is requested
    if (appliedSystematics().size() > 1) {
      // This should not pass applySystematicVariation ...
      efficiencyScaleFactor = 1.0;
      return CP::CorrectionCode::Error;
    }
    else {
      if (m_correlation_model_name == "MCTOYS"){
	auto toy = appliedSystematics().getToyVariationByBaseName ("EL_EFF_" +m_sysSubstring+"MCTOY");
        toy.second = m_scale_toys;
	m_sys = m_result.getResult( (m_rootTool->getFirstToyMCPosition()-1 + toy.first + 1)*m_scale_toys);       
      }
      if (m_correlation_model_name == "COMBMCTOYS"){
        auto toy = appliedSystematics().getToyVariationByBaseName ("EL_EFF_" +m_sysSubstring+"COMBMCTOY");
	toy.second = m_scale_toys;
	m_sys = m_result.getResult( ( m_rootTool->getFirstToyMCPosition()-1 + toy.first)*m_scale_toys);
      }
    }	
  }
  //Special treatment for the correlated 
  else if(m_correlation_model_name == "SIMPLESYST" || m_nCorrSyst==0 ){
    if( appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+"CorrUncertainty", 1)) ){
      m_sys = sqrt(m_result.getTotalUncertainty()*m_result.getTotalUncertainty()-m_result.getResult(4)*m_result.getResult(4)); // total -stat
    }
    if( appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+"CorrUncertainty", -1)) ){
      m_sys = -1*sqrt(m_result.getTotalUncertainty()*m_result.getTotalUncertainty()-m_result.getResult(4)*m_result.getResult(4)); // total -stat
    }
  }
  else{  //Setup the correlated systematic variation for everything else, global way.
    for (int i=0;i<m_nCorrSyst;++i) {/// number of correlated sources
      if( appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+Form("CorrUncertaintyNP%d",i), 1)) ){
	m_sys = m_result.getResult(5 + i);
      }
      if( appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+Form("CorrUncertaintyNP%d",i), -1)) ){
	m_sys = -1*m_result.getResult(5 + i);
      }
    }
  }
  //Different treatment for the uncorrelated uncertainties
  if(m_correlation_model_name == "TOTAL"){ //one "TOTAL" uncertainty
    if( appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+m_correlation_model_name+"_"+"UncorrUncertainty", 1)) ){
      m_sys = m_result.getResult(4);
    //if (m_sys==0) m_sys = m_result.getTotalUncertainty();  
  }
    if( appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+m_correlation_model_name+"_"+"UncorrUncertainty", -1)) ){
      m_sys = -1*m_result.getResult(4);
    }
  }
  else if(m_correlation_model_name == "FULL"){//The Full Model
    for (int i=0;i<m_nUncorrSyst;++i) {
      if( appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+m_correlation_model_name+"_"+Form("UncorrUncertaintyNP%d",i), 1)) ){
	if (i==currentUncorrSystRegion){ //Need to know which one I am filling, i.e PtxEta bin
          m_sys = m_result.getResult(4);//
	}
	else { //Fill only the correct one, other 0
	  m_sys = 0;
	}
      }
      if( appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+m_correlation_model_name+"_"+Form("UncorrUncertaintyNP%d",i), -1)) ){
 	if (i==currentUncorrSystRegion){
          m_sys = -1*m_result.getResult(4);//
	}
	else {
	  m_sys = 0;
	}
      }
    }
  }
  else if(m_correlation_model_name == "SIMPLIFIED"){
    for (int i=0;i<m_nSimpleUncorrSyst;++i) {
      if( appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+m_correlation_model_name+"_"+Form("UncorrUncertaintyNP%d",i), 1)) ){
  	if (i==currentSimplifiedUncorrSystRegion){
          m_sys = m_result.getResult(4);//
	}
	else {
	  m_sys = 0;
	}
      }
      if( appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+m_correlation_model_name+"_"+Form("UncorrUncertaintyNP%d",i), -1)) ){
  	if (i==currentSimplifiedUncorrSystRegion){
          m_sys = -1*m_result.getResult(4);//
	}
	else {
	  m_sys = 0;
	}
      }
    }
  }
 
  if (m_correlation_model_name == "COMBMCTOYS" || m_correlation_model_name == "MCTOYS"){
    efficiencyScaleFactor = m_sys; 
  }
  else {
    efficiencyScaleFactor = m_result.getScaleFactor() + m_sys;
  }
  return  CP::CorrectionCode::Ok;
}

CP::CorrectionCode AsgElectronEfficiencyCorrectionTool::applyEfficiencyScaleFactor(const xAOD::Electron& inputObject) const {
  double efficiencyScaleFactor = 1.0;
  CP::CorrectionCode result = getEfficiencyScaleFactor(inputObject, efficiencyScaleFactor);
  const static SG::AuxElement::Decorator<float> dec (m_resultPrefix+m_resultName+"SF");
  dec(inputObject)= efficiencyScaleFactor;
  return result;
}


//=======================================================================
//   Systematics Interface
//=======================================================================
bool AsgElectronEfficiencyCorrectionTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const {
  if(!systematic.empty()){
    CP::SystematicSet sys = affectingSystematics();
    return sys.find(systematic) != sys.end();
  }
  return true;
}

/// returns: the list of all systematics this tool can be affected by
CP::SystematicSet AsgElectronEfficiencyCorrectionTool::affectingSystematics() const {
  CP::SystematicSet affSys;

  if (m_correlation_model_name == "COMBMCTOYS") {
    affSys.insert((CP::SystematicVariation::makeToyEnsemble("EL_EFF_" +m_sysSubstring+"COMBMCTOY")));
  }
  else  if (m_correlation_model_name == "MCTOYS") {
    affSys.insert((CP::SystematicVariation::makeToyEnsemble ("EL_EFF_" +m_sysSubstring+"MCTOY")));
  }
  else if(m_correlation_model_name == "SIMPLESYST"){
    affSys.insert(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+"CorrUncertainty", 1)); 
    affSys.insert(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+"CorrUncertainty", -1)); 
  }
else if(  m_nCorrSyst==0){
    affSys.insert(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+"CorrUncertainty", 1));
    affSys.insert(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+"CorrUncertainty", -1));
}
  else{
    for (int i=0;i<m_nCorrSyst;++i) {
      affSys.insert(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+Form("CorrUncertaintyNP%d",i), 1)) ;
      affSys.insert(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+Form("CorrUncertaintyNP%d",i), -1)) ;
    }
  }
  //Different tratement for the uncorrelated
  if (m_correlation_model_name == "TOTAL") {
    affSys.insert(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+m_correlation_model_name+"_"+"UncorrUncertainty", 1)); 
    affSys.insert(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+m_correlation_model_name+"_"+"UncorrUncertainty", -1)); 
  }
  else if(m_correlation_model_name == "FULL"){
    for (int i=0;i<m_nUncorrSyst;++i) {
      affSys.insert(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+m_correlation_model_name+"_"+Form("UncorrUncertaintyNP%d",i), 1)) ;
      affSys.insert(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+m_correlation_model_name+"_"+Form("UncorrUncertaintyNP%d",i), -1)) ;
    }
  }    
  else if(m_correlation_model_name == "SIMPLIFIED"){
    for (int i=0;i<m_nSimpleUncorrSyst;++i) {
      affSys.insert(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+m_correlation_model_name+"_"+Form("UncorrUncertaintyNP%d",i), 1)) ;
      affSys.insert(CP::SystematicVariation("EL_EFF_"+m_sysSubstring+m_correlation_model_name+"_"+Form("UncorrUncertaintyNP%d",i), -1)) ;
    }
  }
  return  affSys;
}

// Register the systematics with the registry and add them to the recommended list
CP::SystematicCode AsgElectronEfficiencyCorrectionTool::registerSystematics() {

  CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  if (registry.registerSystematics(*this) != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Failed to add systematic to list of recommended systematics.");
    return CP::SystematicCode::Unsupported;
  }

  return CP::SystematicCode::Ok;
}

/// returns: the list of all systematics this tool recommends to use
CP::SystematicSet AsgElectronEfficiencyCorrectionTool::recommendedSystematics() const {
  return affectingSystematics();
}

CP::SystematicCode AsgElectronEfficiencyCorrectionTool::applySystematicVariation ( const CP::SystematicSet& systConfig )
{
  // First, check if we already know this systematic configuration
  auto itr = m_systFilter.find(systConfig);    
  //It is in the registry of well behaved reasonable tests
  //So now lets return the pre-existing well behaved filtered ones
  if( itr != m_systFilter.end() ){
    CP::SystematicSet& mySysConf = itr->second;
    m_appliedSystematics = &mySysConf;
  }
  //if not , register , after it passes all sanity checks
  else {
    //Lets get the filtered 
    // If it's a new input set, we need to filter it
    // New systematic. We need to parse it.
    CP::SystematicSet affectingSys = affectingSystematics();
    CP::SystematicSet filteredSys;
    if (!CP::SystematicSet::filterForAffectingSystematics(systConfig, affectingSys, filteredSys)){
      ATH_MSG_ERROR("Unsupported combination of systematics passed to the tool!");
      return CP::SystematicCode::Unsupported;
    }        
    // Filtered make sense , only now insert in my map of  systematic sets and return the “filtered"
    // Insert filtered set into the map
    itr = m_systFilter.insert(std::make_pair(systConfig, filteredSys)).first;
    CP::SystematicSet& mySysConf = itr->second;
    // Check for Toy MC variations, if the same toy is used twice, which is forbidden:
    if (m_correlation_model_name == "MCTOYS" || m_correlation_model_name == "COMBMCTOYS") {
      int NToys = 0;
      for ( auto source : mySysConf ) {
	if ( source.name().find("MCTOY") != std::string::npos ) {
	  NToys++;
	}
      }
      if (NToys > 1) {
	return CP::SystematicCode::Unsupported;
      }
    }
    m_appliedSystematics = &mySysConf;
  }
  return CP::SystematicCode::Ok;
}

