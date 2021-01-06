/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// Local include(s):
#include "JetTileCorrection/JetTileCorrectionTool.h"

#ifndef XAOD_STANDALONE // For now metadata is Athena-only
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

#include "PATInterfaces/CorrectionTool.h"
#include "AsgMessaging/StatusCode.h"     
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h" 
#include "PathResolver/PathResolver.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "stdlib.h"



const double pi=3.14159265358979323846;
const double width=2.*pi/64.;

//Validity ranges
const float iov_pt_min = 20000.;   //pt_min > 20GeV
const float iov_pt_max = 3000000.; //pt_max > 3TeV
const float iov_aeta_max = 2.8;    //|eta| < 2.8

namespace CP {
  
  using namespace std;
  using namespace JTC;

  static SG::AuxElement::Decorator<unsigned int> dec_status("TileStatus");
  static SG::AuxElement::Decorator<float> dec_ptraw("Ptraw");
  

  JetTileCorrectionTool :: JetTileCorrectionTool( const std::string& name )
    : asg::AsgMetadataTool( name ),
      m_appliedSystematics(0)
  {
    declareProperty("CorrectionFileName", m_rootFileName="JetTileCorrection/JetTile_pFile_010216.root", "Parametrization file");
    declareProperty("MaskedRegionsMap", m_bd_dead_mapFile="JetTileCorrection/Tile_maskedDB_Run2.conf", "Masked regions DB file");
    declareProperty("UserMaskedRegions", m_v_user_dead, "List of (ad-hoc) TileCal dead modules"); //array of extra dead modules 
  }

  JetTileCorrectionTool :: ~JetTileCorrectionTool()
  {
    if ( m_rootFile ){                   
      delete m_rootFile;
      m_rootFile = 0;
    }
  }

  StatusCode JetTileCorrectionTool :: initialize() {

    // Greet the user:
    ATH_MSG_INFO( "Initialising..." );

#ifdef XAOD_STANDALONE
    // Retrieve the event information (check if MC or data)
    const xAOD::EventInfo* ei(0);
    ATH_CHECK( evtStore()->retrieve( ei, "EventInfo" ) );

    m_isMC = ei->eventType( xAOD::EventInfo::IS_SIMULATION );
#else
    // Retrieve the metadata (check if MC or data)
    std::string projectName = "";
    ATH_CHECK( AthAnalysisHelper::retrieveMetadata("/TagInfo", "project_name", projectName, inputMetaStore() ) );
    if ( projectName == "IS_SIMULATION" ) m_isMC = true;
    else if (projectName.compare(0, 4, "data") == 0 ) m_isMC = false;
    ATH_MSG_INFO("Set up JetTileCorrectionTool -- this is MC? " << m_isMC);
#endif


    //set RJET
    setRJET( (float) RJET );
    
    //Load user dead regions map
    loadDeadUser();

    //Load dead regions in DB map
    loadDeadDB();
 
    // Load the ROOT file
    std::string fname = PathResolverFindCalibFile(m_rootFileName); 
    m_rootFile = TFile::Open( fname.c_str(), "READ" );     

    if ( !m_rootFile ) {
      ATH_MSG_ERROR("Parametrization file " << m_rootFileName << "not found!");
      return StatusCode::FAILURE;
    }

    //Load parametrizations
    TString sub="";

    m_pars_LB={};
    m_pars_EB={};


    for(unsigned int ieta=0; ieta < Pix_eta ; ieta++){
      for(unsigned int iphi=0; iphi < Pix_phi ; iphi++){
	sub = Form("_%d_%d",ieta,iphi);
	
	//flatten 2D-etaphi-to-1D
	m_pars_LB[ieta+iphi*Pix_eta]=(TH1F*)m_rootFile->Get("param_LB_fit"+sub);
	m_pars_EB[ieta+iphi*Pix_eta]=(TH1F*)m_rootFile->Get("param_EB_fit"+sub);
      	m_pars_LB[ieta+iphi*Pix_eta]->SetDirectory(0);
	m_pars_EB[ieta+iphi*Pix_eta]->SetDirectory(0);
      }
    }

    //***
    m_core_sys_LB=(TH1F*)m_rootFile->Get("sys_LB");
    m_core_sys_EB=(TH1F*)m_rootFile->Get("sys_EB");
    //***

    //book number of pt bins in parametrization for later
    m_NbinsPt = m_pars_LB[0]->GetNbinsX(); 


    // set up for default running without systematics
    if (!applySystematicVariation (SystematicSet ())) {
      ATH_MSG_ERROR("Loading the central value systematic set failed.");
      return StatusCode::FAILURE;
    }
    // Add the affecting systematics to the global registry
    SystematicRegistry& registry = SystematicRegistry::getInstance();
    if (registry.registerSystematics(*this) != StatusCode::SUCCESS){
      ATH_MSG_ERROR("Unable to register the systematics");
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully initialized! ");
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }


  CorrectionCode JetTileCorrectionTool :: applyCorrection( xAOD::Jet& jet ){

    //init decorations
    dec_ptraw(jet)  = jet.pt();
    dec_status(jet) = (unsigned int) TS::UNKNOWN;

    //check validity range of the correction
    if( fabs(jet.eta()) > iov_aeta_max ) return CorrectionCode::OutOfValidityRange;
    if( jet.pt() < iov_pt_min || jet.pt() > iov_pt_max ) return CorrectionCode::OutOfValidityRange;    

    JTC::TS status = TS::GOOD;
    if( loadAllModules(jet, status) != StatusCode::SUCCESS )
      return CorrectionCode::Error;
    
    dec_status(jet) = (unsigned int) status; //save status decoration

    // Nothing to do, return gracefully:
    if(status == TS::GOOD) return CorrectionCode::Ok;

    //get relative-pt factor to correct for
    std::vector<float> cfactors = getCorrections(jet);

    //if not correction need it, just leave
    if(!cfactors.size())    return CorrectionCode::Ok;                                                                                                         

    // Redefine the jet 4vector by scaling both pt and mass
    float newPt = jet.pt();
    float newM  = jet.m();

    for(auto cf : cfactors){

      newPt /= cf;
      newM /= cf;
    }

    // Set the  new jet 4vector
    xAOD::JetFourMom_t newp4;
    newp4.SetCoordinates(newPt, jet.eta(), jet.phi(), newM);

    jet.setJetP4( newp4 );

    ATH_MSG_DEBUG("JetTileCorrection applied.");

    // Return gracefully:
    return CorrectionCode::Ok;
  }

  CorrectionCode JetTileCorrectionTool :: correctedCopy( const xAOD::Jet& input, xAOD::Jet*& output) {

    // A sanity check:
    if( output ) {
      ATH_MSG_WARNING( "Non-null pointer received. "
		       "There's a possible memory leak!" );
    }

    ATH_MSG_DEBUG("making the copy");

    // Create the copy:
    std::unique_ptr< xAOD::Jet > newobj( new xAOD::Jet( input ) );

    // Apply the correction to it:
    const CorrectionCode result = this->applyCorrection( *newobj );
    if( result == CorrectionCode::Error ) {
       ATH_MSG_ERROR( "Failed to apply correction to jet" );
    } else {
       output = newobj.release();
    }

    // Return the value from applyCorrection:
    return result;
  }

  bool JetTileCorrectionTool :: isAffectedBySystematic( const SystematicVariation& systematic ) const
  {
    SystematicSet sys = affectingSystematics();
    return sys.find (systematic) != sys.end ();
  }


  SystematicSet JetTileCorrectionTool :: affectingSystematics() const
  {
    SystematicSet result;

    result.insert(CP::SystematicVariation("JET_TILECORR_Uncertainty", 1));
    result.insert(CP::SystematicVariation("JET_TILECORR_Uncertainty", -1));

    return result;
  }

  SystematicSet JetTileCorrectionTool :: recommendedSystematics() const
  {
    return affectingSystematics();
  }


  StatusCode JetTileCorrectionTool :: applySystematicVariation ( const SystematicSet& systConfig ) {

    // First, check if we already know this systematic configuration
    auto itr = m_systFilter.find(systConfig);
    
    // If it's a new input set, we need to filter it
    if( itr == m_systFilter.end() ){
      
      // New systematic. We need to parse it.
      static CP::SystematicSet affectingSys = affectingSystematics();
      CP::SystematicSet filteredSys;   
      if (!CP::SystematicSet::filterForAffectingSystematics(systConfig, affectingSys, filteredSys)){
	ATH_MSG_ERROR("Unsupported combination of systematics passed to the tool!");
	return StatusCode::FAILURE;
      }
      
      // Insert filtered set into the map
      itr = m_systFilter.insert(std::make_pair(systConfig, filteredSys)).first;
    }
    
    CP::SystematicSet& mySysConf = itr->second;
    m_appliedSystematics = &mySysConf;
    return StatusCode::SUCCESS;
  }
  

  JTC::TS JetTileCorrectionTool :: overlap(const xAOD::Jet& j, Hole region){

    double phisize   = region.phi2-region.phi1;
    double phicenter = (region.phi1+region.phi2)/2.;
    float  jet_eta   = j.jetP4(xAOD::JetConstitScaleMomentum).eta();
    float  jet_phi   = j.jetP4(xAOD::JetConstitScaleMomentum).phi();

    // GOOD?
    if( region.eta2 < (jet_eta-m_RJET) ) return TS::GOOD;
    if( region.eta1 > (jet_eta+m_RJET) ) return TS::GOOD;
    if( fabs(TVector2::Phi_mpi_pi(jet_phi-phicenter)) > m_RJET+phisize/2.) return TS::GOOD;
    
    // CORE-BAD?
    if( inHole(jet_eta, jet_phi, region) ) return TS::CORE;
    
    // CORE-EDGE? (elsewhere)
    return TS::EDGE;
  }
  
  void JetTileCorrectionTool :: loadDeadUser(){
    
    m_user_dead_LB = {};
    m_user_dead_EB = {};

    //Format accepted for module input:  "LBA 10" == "0 9"
    for(auto& r : m_v_user_dead){

      std::stringstream ss(r);
      std::string s;

      std::vector<std::string> tokens;
      while (getline(ss, s, ' ')) {
	tokens.push_back(s);
      }

      if(tokens.size() < 2){
	ATH_MSG_ERROR("Part-Module pair " << r << " not known! Please use \"PART MOD\" format "); 
	continue;
      }

      int i_part = 0;
      int i_mod  = atoi((tokens.at(1)).c_str());
      if (tokens.at(0).find("B") != string::npos)
	i_mod -= 1; // substract 1 from second coor if given in format "LBA 4"

      if(tokens.at(0)=="LBA" || tokens.at(0)== "0"){
	i_part = 0;
      }else if (tokens.at(0)=="LBC" || tokens.at(0)== "1"){
	i_part = 1;
      }else if (tokens.at(0)=="EBA" || tokens.at(0)== "2"){
	i_part = 2;
      }else if (tokens.at(0)=="EBC" || tokens.at(0)== "3"){
	i_part = 3;
      }else{
	ATH_MSG_ERROR("Part-Module pair " << r << " not known! Please use \"PART MOD\" format "); 
	continue;
      }

      //Classify into long and extended barrel modules
      Hole rdead = partModToHole(i_part, i_mod);

      if( i_part < 2 ) 
	m_user_dead_LB[r] = rdead; //LBA, LBC
      else 
	m_user_dead_EB[r] = rdead; //EBA, EBC

    }
  }

  //Load dead regions from DB file
  void JetTileCorrectionTool :: loadDeadDB(){
   
    m_db_dead_LB = {};
    m_db_dead_EB = {};

    std::vector<Hole> dbholes={};

    //Simulation
    if(m_isMC){

      //-- MC15c : no dead modules
      //COOLOFL_TILE/OFLP200 /TILE/OFL02/STATUS/ADC TileOfl02StatusAdc-IOVDEP-05
      //COOLOFL_TILE/OFLP200 /TILE/OFL02/NOISE/CELL TileOfl02NoiseCell-OF2-07
      //

      //TODO:  add RunNumber check and add these below if running on MC15b //M.T.

      //-- MC15b : two dead modules
      //COOLOFL_TILE/OFLP200 /TILE/OFL02/STATUS/ADC TileOfl02StatusAdc-IOVDEP-05
      //COOLOFL_TILE/OFLP200 /TILE/OFL02/NOISE/CELL TileOfl02NoiseCell-OF2-07
      //
      // //LBA10 
      // Hole rdead = partModToHole(0, 9);
      // rdead.iov = make_pair(0,1000000); //no range
      // dbholes.push_back( rdead );  

      // //EBC21
      // rdead = partModToHole(3, 20);
      // rdead.iov = make_pair(0,1000000); //no range
      // dbholes.push_back( rdead );  

    }
    else{ //DATA

      //read map file
      std::string mapFilename = PathResolverFindCalibFile(m_bd_dead_mapFile); 
      std::ifstream mapFile;
      mapFile.open(mapFilename);
     
      std::string line;
      while (std::getline(mapFile, line)){
	
	if(line[0]=='#') continue;
	
	std::istringstream iss(line);
	int part,mod, irun, erun;
	std::string modname;
	
	if (!(iss >> part >> mod >> irun >> erun >> modname)) { break; } // error
	
	Hole rdead = partModToHole(part, mod);
	rdead.iov = make_pair(irun,erun);
	dbholes.push_back( rdead );  
	
      }
    }
      
    int dbh=1;
    for(const auto& h : dbholes){
      if(fabs(h.eta1)>1 || fabs(h.eta2)>1){
	m_db_dead_EB[Form("DB%d",dbh)] = h;
      }
      else{
	m_db_dead_LB[Form("DB%d",dbh)] = h;
      }
      
      dbh++;

      //      ATH_MSG_INFO("Adding DB dead module at (eta1,phi1)=(" << h.eta1 << "," << h.phi1 << ")");
      ATH_MSG_DEBUG("Adding DB dead module at (eta1,phi1)=(" << h.eta1 << "," << h.phi1 << ")");
    }
  }


  IPair JetTileCorrectionTool :: getModulePosition(const xAOD::Jet& jet, Hole mod){

    float eta = (mod.eta1+mod.eta2)/2.;
    float phi = (mod.phi1+mod.phi2)/2.;

    float jet_eta = jet.jetP4(xAOD::JetConstitScaleMomentum).eta();
    float jet_phi = jet.jetP4(xAOD::JetConstitScaleMomentum).phi();

    float eta_dist = jet_eta-eta;
    float phi_dist = TVector2::Phi_mpi_pi((double)jet_phi - (double)phi);

    int inphi;

    if(fabs(phi_dist)<0.05){ inphi=0; }
    else if(fabs(phi_dist)<0.1){ inphi=1; }
    else if(fabs(phi_dist)<0.2){ inphi=2; }
    else if(fabs(phi_dist)<0.3){ inphi=3; } 
    else if(fabs(phi_dist)<0.4){ inphi=4; }
    else { inphi=5;}

    float ieta  = eta_dist/PIXWIDTH;
    int   ineta = (int)ieta;


    //get parametrization from positive side always!
    if(mod.eta1 < -0.1) ineta = -ineta;

    return make_pair(ineta+8, inphi);
  }

  void JetTileCorrectionTool :: loadModulesFromMap(const xAOD::Jet& jet, JTC::TS &status, std::map<std::string,Hole> hmap, PART part, TYPE type){

    float cfactor(0.);
    IPair mpos;
    JTC::TS cstatus = TS::GOOD; 

    for (const auto& mod : hmap){

      //check IOV for DB defined modules
      if(!m_isMC && type == TYPE::DB && !inIOV(mod.second, m_current_run)) continue;

      cstatus = overlap(jet,mod.second);      
      if(cstatus != TS::GOOD){

	//get eta-phi position (relative to jet axis)
	mpos = getModulePosition(jet,mod.second);

	//get correction (1D-array)
	if(part == PART::LB)
	  cfactor = m_pars_LB[mpos.first + Pix_eta * mpos.second]->GetBinContent(getPtBin(jet.pt()));
	else
	  cfactor = m_pars_EB[mpos.first + Pix_eta * mpos.second]->GetBinContent(getPtBin(jet.pt()));

	//overwrite global status with worst case
	if(cstatus > status)      status = cstatus;

	//create new d-region and add it to the list     
	m_position_masked.emplace_back( mod.second,mpos,part,cfactor,cstatus,type );

      }
    }

  }

  
  JTC::TS JetTileCorrectionTool :: getTileStatus(const xAOD::Jet& jet){
    JTC::TS status = TS::GOOD;
    if( loadAllModules(jet, status) != StatusCode::SUCCESS ){
      ATH_MSG_ERROR( "Something went wrong while loading/checking the modules!");
      return TS::UNKNOWN;
    }
    return status;
  }


  StatusCode JetTileCorrectionTool :: addTileStatus(const xAOD::Jet& jet){
    JTC::TS status = getTileStatus(jet);
    dec_status(jet) = (unsigned int) status; //save status decoration
    return StatusCode::SUCCESS;
  }

  void JetTileCorrectionTool :: setRJET(float r){ 
    m_RJET = r; 
  }


  StatusCode JetTileCorrectionTool :: loadAllModules(const xAOD::Jet& jet, JTC::TS &status){

    m_position_masked.clear();

    //Read dead regions DB if we change run/LB
    if(!m_isMC){

      const xAOD::EventInfo* ei = 0;
      if( evtStore()->retrieve( ei, "EventInfo" ).isFailure() ) {
	ATH_MSG_WARNING( "No EventInfo object could be retrieved" );
      }
      
      if (!ei) {
	ATH_MSG_ERROR( "Cannot retrieve the EventInfo" );
	return StatusCode::FAILURE;
      }
    
      m_current_run = ei->runNumber();
      
    }

    //load DB-defined modules in LB
    loadModulesFromMap(jet, status, m_db_dead_LB, PART::LB, TYPE::DB);
    //load DB-defined modules in EB
    loadModulesFromMap(jet, status, m_db_dead_EB, PART::EB, TYPE::DB);
    
    //load user-defined modules in LB
    loadModulesFromMap(jet, status, m_user_dead_LB, PART::LB, TYPE::User);
    //load user-defined modules in EB
    loadModulesFromMap(jet, status, m_user_dead_EB, PART::EB, TYPE::User);

    //sort modules by correction size (in decreasing order)
    std::sort(m_position_masked.begin(), m_position_masked.end(), greater<Region>());

    return StatusCode::SUCCESS;
  }

  std::vector<float> JetTileCorrectionTool :: getCorrections(const xAOD::Jet& jet){

    float ptlast  = jet.pt();
    int ptbin     = getPtBin(ptlast);

    //***
    int ptbin_sys=m_core_sys_LB->FindBin(ptlast);
    //***

    std::vector<float> corrections = {};

    if (ptbin < 0) return corrections; // no correction below 20GeV

    //***
    if (ptlast < 40000) return corrections;
    //***

    float clast   = 1.;
    float sigma = 0.;

    if(&appliedSystematics()!=0){ //nomimal value
      sigma = appliedSystematics().getParameterByBaseName("JET_TILECORR_Uncertainty");
    }

    for(const auto& region : m_position_masked){
	
      ptlast *= clast;
      ptbin  = getPtBin( ptlast );

      if(region.part == PART::LB) {

	//***
	if (region.status == TS::CORE) clast = m_pars_LB[region.ep.first + Pix_eta * region.ep.second]->GetBinContent(ptbin) + sigma * m_core_sys_LB->GetBinContent(ptbin_sys); //systematics are bigger if we are in the core
	else  clast = m_pars_LB[region.ep.first + Pix_eta * region.ep.second]->GetBinContent(ptbin) + sigma * m_pars_LB[region.ep.first + Pix_eta * region.ep.second]->GetBinError(ptbin);
	//***

      }else{

	//***
	if (region.status == TS::CORE) clast = m_pars_EB[region.ep.first + Pix_eta * region.ep.second]->GetBinContent(ptbin) + sigma * m_core_sys_EB->GetBinContent(ptbin_sys); //systematics are bigger if we are in the core
	else  clast = m_pars_EB[region.ep.first + Pix_eta * region.ep.second]->GetBinContent(ptbin) + sigma * m_pars_EB[region.ep.first + Pix_eta * region.ep.second]->GetBinError(ptbin);
	//***

	
      }
      corrections.push_back(clast);
    }
  
    return corrections;
  }


  int JetTileCorrectionTool :: getPtBin(float pt){

    if(pt < 20000.) return -1;

    int ptbin = m_pars_LB[0]->FindBin(pt*0.001); // no correction below 20GeV

    //adjust under/overflows
    if (ptbin < 1) ptbin = 1;
    if (ptbin > m_NbinsPt) ptbin = m_NbinsPt;

    return ptbin;
  }
 
  bool JetTileCorrectionTool :: inIOV(Hole region, int run){
    if( region.iov.first>=0 &&  (run < region.iov.first) ) return false;
    if( region.iov.second>=0 &&  (run > region.iov.second) ) return false;
    return true;
  }

  bool JetTileCorrectionTool :: inHole(const xAOD::Jet& jet, Hole rdead){
    return inHole(jet.eta(), jet.phi(), rdead);
  }


  bool JetTileCorrectionTool :: inHole(float eta, float phi, Hole rdead){

    if(rdead.eta1==rdead.eta2 || rdead.phi1==rdead.phi2) return false;
    if((eta > rdead.eta1) && (eta < rdead.eta2) && (phi > rdead.phi1) && (phi < rdead.phi2)) return true;
    
    return false;
  }

  Hole JetTileCorrectionTool::partModToHole(int part,int mod){

    Hole region;
    region.iov = make_pair(0,1000000); //a dummy full IOV is set by default

    switch(part){
    case 0: //LBA                                                                                                                                               
      region.eta1=0;
      region.eta2=0.9;
      break;
    case 1: //LBC                                                                                                                                               
      region.eta1=-0.9;
      region.eta2=0;
      break;
    case 2: //EBA                                                                                                                                               
      region.eta1=0.8;
      region.eta2=1.7;
      break;
    case 3: //EBC                                                                                                                                               
      region.eta1=-1.7;
      region.eta2=-0.8;
      break;
    default:
      cout<<"Bad partition value passed!\n";
      region.eta1=-999;
      region.eta2=-999;
      region.phi1=-999;
      region.phi2=-999;
      return region;
    }
    if(mod<32){
      region.phi1=((double)mod)*width;
      region.phi2=region.phi1+width;
    }
    else if(mod<64){
      region.phi1=((double)mod)*width-2.*pi;
      region.phi2=region.phi1+width;
    }
    else{
      cout<<"Bad module value passed!\n";
      region.eta1=-999;
      region.eta2=-999;
      region.phi1=-999;
      region.phi2=-999;
      return region;
    }
    return region;
  }


} // namespace CP
