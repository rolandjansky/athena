/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8_i/Pythia8_i.h"
#include "Pythia8_i/UserProcessFactory.h"
#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8_i/UserResonanceFactory.h"

#include "PathResolver/PathResolver.h"
#include "GeneratorObjects/McEventCollection.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

// calls to fortran routines
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// Name of AtRndmGenSvc stream
std::string     Pythia8_i::pythia_stream   = "PYTHIA8_INIT";

using boost::assign::operator+=;

/**
 * author: James Monk (jmonk@cern.ch)
 */
////////////////////////////////////////////////////////////////////////////////
Pythia8_i::Pythia8_i(const string &name, ISvcLocator *pSvcLocator)
: GenModule(name, pSvcLocator),
m_pythia(xmlpath()),
m_internal_event_number(0),
m_version(-1.),
m_atlasRndmEngine(0),
m_nAccepted(0.),
m_nMerged(0.),
m_failureCount(0),
m_procPtr(0),
m_userHookPtr(0),
m_doLHE3Weights(false)
{
  declareProperty("Commands", m_commands);
  declareProperty("UserParams", m_userParams);
  declareProperty("UserModes", m_userModes);
  declareProperty("CollisionEnergy", m_collisionEnergy = 14000.0);
  declareProperty("useRndmGenSvc", m_useRndmGenSvc = true);
  declareProperty("Beam1", m_beam1 = "PROTON");
  declareProperty("Beam2", m_beam2 = "PROTON");
  declareProperty("LHEFile", m_lheFile = "");
  declareProperty("CKKWLAcceptance", m_doCKKWLAcceptance = false);
  declareProperty("MaxFailures", m_maxFailures = 10);//the max number of consecutive failures
  declareProperty("UserProcess", m_userProcess="");
  declareProperty("UserHook", m_userHook="");
  declareProperty("UserResonances", m_userResonances="");
  declareProperty("UseLHAPDF", m_useLHAPDF=true);
  declareProperty("ParticleData", m_particleDataFile="");
  declareProperty("OutputParticleData",m_outputParticleDataFile="ParticleData.local.xml");
  
  m_particleIDs["PROTON"]     = PROTON;
  m_particleIDs["ANTIPROTON"] = ANTIPROTON;
  m_particleIDs["ELECTRON"]   = ELECTRON;
  m_particleIDs["POSITRON"]   = POSITRON;

}

////////////////////////////////////////////////////////////////////////////////
Pythia8_i::~Pythia8_i() {

  delete m_atlasRndmEngine;
  
  if(m_procPtr != 0)     delete m_procPtr;
  if(m_userHookPtr != 0) delete m_userHookPtr;
  
}

////////////////////////////////////////////////////////////////////////////////
StatusCode Pythia8_i::genInitialize() {

  ATH_MSG_DEBUG("Pythia8_i from genInitialize()");
  
  m_version = m_pythia.settings.parm("Pythia:versionNumber");
  
  std::string pythiaVersion = boost::lexical_cast<std::string>(m_version + 0.00000000001);
  pythiaVersion.erase(5);
  std::string libVersion = "8." + std::string(PY8VERSION);
  
  if(pythiaVersion != libVersion){
    ATH_MSG_ERROR("Version of Pythia in xmldoc (" + pythiaVersion + ") does not matched linked library version (" + libVersion + ")");
    return StatusCode::FAILURE;
  }
  
  Pythia8_i::pythia_stream =       "PYTHIA8_INIT";
  
  // We do explicitly set tune 4C, since it is the starting point for many other tunes
  // Tune 4C for pp collisions
  m_pythia.readString("Tune:pp = 5");

  // also use CTEQ6L1 from LHAPDF 6 by default
  // can be over-written using JO
  m_pythia.readString("PDF:pSet= LHAPDF6:cteq6ll.LHpdf");
  
  // have to find any old-style Pythia 8.18x PDF commands and convert them
  foreach(string &cmd, m_commands){
    try{
      string val = findValue(cmd, "PDF:LHAPDFset");
      if(val != ""){
        cmd = "PDF:pSet = LHAPDF6:" + val;
      }else{
        val = findValue(cmd, "BeamRemnants:reconnectRange");
        if(val != ""){
          cmd = "ColourReconnection:range = " + val;
          m_commands += "ColourReconnection:mode = 0";
        }
      }
        
    }catch(Pythia8_i::CommandException err){
      ATH_MSG_ERROR(err.what());
      return StatusCode::FAILURE;
    }

    if(cmd.find("PDF:useLHAPDF") != std::string::npos){
      ATH_MSG_WARNING("PDF:useLHAPDF is deprecated and ignored.");
      cmd="";
    }
  }

  foreach(const string &param, m_userParams){
    std::vector<string> splits;
    boost::split(splits, param, boost::is_any_of("="));
    if(splits.size() != 2){
      ATH_MSG_ERROR("Cannot interpret user param command: " + param);
      return StatusCode::FAILURE;
    }
    
    boost::erase_all(splits[0], " ");
    m_pythia.settings.addParm(splits[0], 0., false, false, 0., 0.);
    m_commands+=param;
  }

  foreach(const string &mode, m_userModes){
    std::vector<string> splits;
    boost::split(splits, mode, boost::is_any_of("="));
    if(splits.size() != 2){
      ATH_MSG_ERROR("Cannot interpret user mode command: " + mode);
      return StatusCode::FAILURE;
    }
    
    boost::erase_all(splits[0], " ");
    m_pythia.settings.addMode(splits[0], 0, false, false, 0, 0);
    m_commands+=mode;
  }
  
  // Now apply the settings from the JO
  foreach(const string &cmd, m_commands){
    
    if(cmd.compare("")==0) continue;
    try{
      string val = findValue(cmd, "Tune:pp");
      if(val != ""){
        int tune = boost::lexical_cast<int>(val);
        if(tune > s_allowedTunes(m_version)){
          ATH_MSG_ERROR("Tune setting is not known to this version of Pythia:" + cmd);
          return StatusCode::FAILURE;
        }
      }
    }catch(CommandException err){
      ATH_MSG_ERROR(err.what());
      return StatusCode::FAILURE;
    }
    
    bool read = m_pythia.readString(cmd);
    
    if(!read){
      ATH_MSG_ERROR("Pythia could not understand the command '"<< cmd<<"'");
      return StatusCode::FAILURE;
    }
  }

  PDGID beam1 = m_particleIDs[m_beam1];
  PDGID beam2 = m_particleIDs[m_beam2];

  ATH_MSG_INFO("Beam1 = "<<beam1);
  ATH_MSG_INFO("Beam2 = "<<beam2);

  if(beam1 == 0 || beam2 == 0){
    ATH_MSG_ERROR("Invalid beam particle!");
    return StatusCode::FAILURE;
  }

  if(m_useRndmGenSvc){

    ATH_MSG_INFO(" !!!!!!!!!!!!  WARNING ON PYTHIA RANDOM NUMBERS !!!!!!!!!! ");
    ATH_MSG_INFO("           THE ATHENA SERVICE AtRndmGenSvc IS USED.");
    ATH_MSG_INFO(" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ");

    if(m_atlasRndmEngine) delete m_atlasRndmEngine;
    m_atlasRndmEngine = new customRndm();

    m_atlasRndmEngine->init(atRndmGenSvc(),Pythia8_i::pythia_stream);
    m_pythia.setRndmEnginePtr(m_atlasRndmEngine);

    // Save the PYTHIA_INIT stream seeds....
    CLHEP::HepRandomEngine* engine = atRndmGenSvc().GetEngine(Pythia8_i::pythia_stream);

    const long*   sip     =       engine->getSeeds();
    long  int     si1     =       sip[0];
    long  int     si2     =       sip[1];

    atRndmGenSvc().CreateStream(si1, si2, Pythia8_i::pythia_stream);
    Pythia8_i::pythia_stream =       "PYTHIA8";
    m_atlasRndmEngine->m_stream=Pythia8_i::pythia_stream;

  }else{
    ATH_MSG_INFO(" !!!!!!!!!!!!  WARNING ON PYTHIA RANDOM NUMBERS !!!!!!!!!! ");
    ATH_MSG_INFO("    THE STANDARD PYTHIA8 RANDOM NUMBER SERVICE IS USED.");
    ATH_MSG_INFO("    THE ATHENA SERVICE AtRndmGenSvc IS ***NOT*** USED.");
    ATH_MSG_INFO(" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ");
  }

  m_procPtr = 0;

  if(m_userProcess != ""){
    m_procPtr = Pythia8_UserProcess::UserProcessFactory::create(m_userProcess);
  }

  bool canInit = true;
  
  m_userHookPtr = 0;

  if(m_userHook != ""){
    m_userHookPtr = Pythia8_UserHooks::UserHooksFactory::create(m_userHook);
    if(!m_pythia.setUserHooksPtr(m_userHookPtr)){
      ATH_MSG_ERROR("Unable to set requested user hook: " + m_userHook + " !!");
      ATH_MSG_ERROR("Pythia 8 initialisation will FAIL!");
      canInit = false;
    }
  }

  if(m_userResonances != ""){
   
    std::vector<string> resonanceArgs;
    
    boost::split(resonanceArgs, m_userResonances, boost::is_any_of(":"));
    if(resonanceArgs.size() != 2){
      ATH_MSG_ERROR("Cannot Understand UserResonance job option!");
      ATH_MSG_ERROR("You should specify it as a 'name:id1,id2,id3...'");
      ATH_MSG_ERROR("Where name is the name of your UserResonance, and id1,id2,id3 are a comma separated list of PDG IDs to which it is applied");
      canInit = false;
    }
    std::vector<string> resonanceIds;
    boost::split(resonanceIds, resonanceArgs.back(), boost::is_any_of(","));
    if(resonanceIds.size()==0){
      ATH_MSG_ERROR("You did not specifiy any PDG ids to which your user resonance width should be applied!");
      ATH_MSG_ERROR("You should specify a list as 'name:id1,id2,id3...'");
      ATH_MSG_ERROR("Where name is the name of your UserResonance, and id1,id2,id3 are a comma separated list of PDG IDs to which it is applied");
      canInit=false;
    }
    
    for(std::vector<string>::const_iterator sId = resonanceIds.begin();
        sId != resonanceIds.end(); ++sId){
      int idResIn = boost::lexical_cast<int>(*sId);
      m_userResonancePtrs.push_back(Pythia8_UserResonance::UserResonanceFactory::create(resonanceArgs.front(), idResIn));
    }
    
    for(std::vector<Pythia8::ResonanceWidths*>::const_iterator resonance = m_userResonancePtrs.begin();
        resonance != m_userResonancePtrs.end(); ++resonance){
      m_pythia.setResonancePtr(*resonance);
    }
    
  }
  
  if(m_particleDataFile != "") {
    if(!m_pythia.particleData.reInit(m_particleDataFile, true)){
      ATH_MSG_ERROR("Unable to read requested particle data table: " + m_particleDataFile + " !!");
      ATH_MSG_ERROR("Pythia 8 initialisation will FAIL!");
      canInit = false;
    }
  }

  if(m_lheFile != ""){
    if(m_procPtr != 0){
      ATH_MSG_ERROR("Both LHE file and user process have been specified");
      ATH_MSG_ERROR("LHE input does not make sense with a user process!");
      canInit = false;
    }
    
    canInit = canInit && m_pythia.readString("Beams:frameType = 4");
    canInit = canInit && m_pythia.readString("Beams:LHEF = " + m_lheFile);
    if(!canInit){
      ATH_MSG_ERROR("Unable to read requested LHE file: " + m_lheFile + " !");
      ATH_MSG_ERROR("Pythia 8 initialisation will FAIL!");
    }
  }else{
    canInit = canInit && m_pythia.readString("Beams:frameType = 1");
    canInit = canInit && m_pythia.readString("Beams:idA = " + boost::lexical_cast<string>(beam1));
    canInit = canInit && m_pythia.readString("Beams:idB = " + boost::lexical_cast<string>(beam2));
    canInit = canInit && m_pythia.readString("Beams:eCM = " + boost::lexical_cast<string>(m_collisionEnergy));
  }
  
  if(m_procPtr != 0){
    if(!m_pythia.setSigmaPtr(m_procPtr)){
      ATH_MSG_ERROR("Unable to set requested user process: " + m_userProcess + " !!");
      ATH_MSG_ERROR("Pythia 8 initialisation will FAIL!");
      canInit = false;
    }
  }
  
  StatusCode returnCode = SUCCESS;
  
  if(canInit){
    canInit = m_pythia.init();
  }
  
  if(!canInit){
    returnCode = StatusCode::FAILURE;
    ATH_MSG_ERROR(" *** Unable to initialise Pythia !! ***");
  }
  
  m_pythia.settings.listChanged();
  m_pythia.particleData.listChanged();
  m_pythia.particleData.listXML(m_outputParticleDataFile);
  
  //counter for event failures;
  m_failureCount = 0;

  m_internal_event_number = 0;
  
  m_pythiaToHepMC.set_store_pdf(true);
  
  return returnCode;
}

////////////////////////////////////////////////////////////////////////////////
StatusCode Pythia8_i::callGenerator(){

  ATH_MSG_DEBUG(">>> Pythia8_i from callGenerator");

  if(m_useRndmGenSvc){
    // Save the random number seeds in the event
    CLHEP::HepRandomEngine*  engine  = atRndmGenSvc().GetEngine(Pythia8_i::pythia_stream);
    const long* s =  engine->getSeeds();
    m_seeds.clear();
    m_seeds.push_back(s[0]);
    m_seeds.push_back(s[1]);
  }

  bool status = m_pythia.next();

  StatusCode returnCode = StatusCode::SUCCESS;

  if(!status){
    ++m_failureCount;
    if(m_failureCount >= m_maxFailures){
      ATH_MSG_ERROR("Exceeded the max number of consecutive event failures.");
      returnCode =  StatusCode::FAILURE;
    }else{
      ATH_MSG_INFO("Event generation failed - re-trying.");
      returnCode = this->callGenerator();
    }
  }

  m_failureCount = 0;
  
  m_nAccepted += 1.;
  // some CKKWL merged events have zero weight (or unfilled event). 
  // start again with such events
  
  double eventWeight = m_pythia.info.mergingWeight()*m_pythia.info.weight();
  
  if(returnCode != StatusCode::FAILURE &&
     (fabs(eventWeight) < 1.e-18 ||
      m_pythia.event.size() < 2)){
       
       returnCode = this->callGenerator();
     }else{
       m_nMerged += eventWeight;
       ++m_internal_event_number;
     }
  
  return returnCode;
}

///////////////////////////////////////////////////////////////////////////////
StatusCode Pythia8_i::fillEvt(HepMC::GenEvent *evt){

  ATH_MSG_DEBUG(">>> Pythia8_i from fillEvt");

  evt->set_event_number(m_internal_event_number);

  if(m_pythia.event.size() < 2){
    ATH_MSG_ERROR("Something wrong with this event - it contains fewer than 2 particles!");
    ATH_MSG_ERROR("internal event number is "<<m_internal_event_number);
    return StatusCode::FAILURE;
  }
  
  m_pythiaToHepMC.fill_next_event(m_pythia, evt, m_internal_event_number);

  // in debug mode you can check whether the pdf information is stored
  if(evt->pdf_info()){
    ATH_MSG_DEBUG("PDFinfo id1:" << evt->pdf_info()->id1());
    ATH_MSG_DEBUG("PDFinfo id2:" << evt->pdf_info()->id2());
    ATH_MSG_DEBUG("PDFinfo x1:" << evt->pdf_info()->x1());
    ATH_MSG_DEBUG("PDFinfo x2:" << evt->pdf_info()->x2());
    ATH_MSG_DEBUG("PDFinfo scalePDF:" << evt->pdf_info()->scalePDF());
    ATH_MSG_DEBUG("PDFinfo pdf1:" << evt->pdf_info()->pdf1());
    ATH_MSG_DEBUG("PDFinfo pdf2:" << evt->pdf_info()->pdf2());
  }
  else
    ATH_MSG_DEBUG("No PDF information available in HepMC::GenEvent!");

  // set the randomseeds
  if(m_useRndmGenSvc)evt->set_random_states(m_seeds);
  
  double phaseSpaceWeight = m_pythia.info.weight();
  double mergingWeight    = m_pythia.info.mergingWeight();
  double eventWeight = phaseSpaceWeight*mergingWeight;
  
  ATH_MSG_DEBUG("Event weights: phase space weight, merging weight, total weight = "<<phaseSpaceWeight<<", "<<mergingWeight<<", "<<eventWeight);
  
  // set the event weight
  evt->weights().clear();
  
  std::vector<string>::const_iterator id = m_weightIDs.begin();
  
  if(m_pythia.info.getWeightsDetailedSize() != 0){
    for(std::map<string, Pythia8::LHAwgt>::const_iterator wgt = m_pythia.info.rwgt->wgts.begin();
        wgt != m_pythia.info.rwgt->wgts.end(); ++wgt){
      
      if(m_internal_event_number == 1){
        m_doLHE3Weights = true;
        m_weightIDs.push_back(wgt->first);
      }else{
        if(*id != wgt->first){/// mismatch in weight name!
          ATH_MSG_ERROR("Mismatch in LHE3 weight name.  Found "<<wgt->first<<", expected "<<*id);
          return StatusCode::FAILURE;
        }
        ++id;
      }
      
      std::map<string, Pythia8::LHAweight>::const_iterator weightName = m_pythia.info.init_weights->find(wgt->first);
      if(weightName != m_pythia.info.init_weights->end()){
        evt->weights()[weightName->second.contents] = mergingWeight * wgt->second.contents;
      }else{
        evt->weights()[wgt->first] = mergingWeight * wgt->second.contents;
      }
      
    }
  }else{
    evt->weights().push_back(eventWeight);
  }

  // Units correction
  #ifdef HEPMC_HAS_UNITS
  //std::cout << (evt->momentum_unit() == HepMC::Units::GEV ? "GeV" : "MeV") << std::endl;
  /// @todo We shouldn't be having to rescale these events if they're already in MeV :S Where's the screw-up: HepMC or Py8?
  /// Hopefully this is permanently fixed in version 8.170 onwards
  if(pythiaVersion() < 8.170 ){
    GeVToMeV(evt);
  }
  #else
  ATH_MSG_DEBUG("Not scaling HepMC energies for Py8 > 8.153!");
  /// @todo We *should* be having to rescale these events! Again, is HepMC or Py8 wrong? Or us?
  //GeVToMeV(evt);
  #endif

#ifndef HEPMC_HAS_UNITS
  ATH_MSG_DEBUG();
#endif
  
  //HepMC::GenEvent *evtCopy = new HepMC::GenEvent(*evt);

  return SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
StatusCode Pythia8_i::genFinalize(){

  ATH_MSG_INFO(">>> Pythia8_i from genFinalize");

  m_pythia.stat();
  
  Pythia8::Info info = m_pythia.info;
  double xs = info.sigmaGen(); // in mb
  
  if(m_doCKKWLAcceptance){
    ATH_MSG_DEBUG("Multiplying cross-section by CKKWL merging acceptance of "<<m_nMerged <<"/" <<info.nAccepted());
    xs *= m_nMerged / info.nAccepted();
  }
  xs *= 1000. * 1000.;//convert to nb

  std::cout << "MetaData: cross-section (nb)= " << xs <<std::endl;
  std::cout << "MetaData: generator= Pythia 8." << PY8VERSION <<std::endl;

  if(m_doLHE3Weights){
    
    std::cout<<"MetaData: weights = ";
    foreach(const string &id, m_weightIDs){
      
      std::map<string, Pythia8::LHAweight>::const_iterator weight = m_pythia.info.init_weights->find(id);
      
      if(weight != m_pythia.info.init_weights->end()){
        std::cout<<weight->second.contents<<" | ";
      }else{
        std::cout<<"Unknown | ";
      }
    }
    std::cout<<std::endl;
  }
  
  return SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
double Pythia8_i::pythiaVersion()const{
  return m_version;
}

////////////////////////////////////////////////////////////////////////////////
string Pythia8_i::findValue(const string &command, const string &key){
  if(command.find(key) == std::string::npos) return "";
  
  std::vector<string> splits;
  boost::split(splits, command, boost::is_any_of("="));
  if(splits.size() != 2){
    throw Pythia8_i::CommandException(command);
  }
  
  boost::erase_all(splits[1], " ");
  
  return splits[1];
}

////////////////////////////////////////////////////////////////////////////////
string Pythia8_i::xmlpath(){
    
  char *cmtpath = getenv("CMTPATH");
  char *cmtconfig = getenv("CMTCONFIG");
  
  string foundpath = "";
  
  if(cmtpath != 0 && cmtconfig != 0){
    
    std::vector<string> cmtpaths;
    boost::split(cmtpaths, cmtpath, boost::is_any_of(string(":")));
    
    string installPath = "/InstallArea/" + string(cmtconfig) + "/share/Pythia8/xmldoc";
    
    for(std::vector<string>::const_iterator path = cmtpaths.begin();
        path != cmtpaths.end() && foundpath == ""; ++path){
      string testPath = *path + installPath;
      std::ifstream testFile(testPath.c_str());
      if(testFile.good()) foundpath = testPath;
      testFile.close();
    }
    
  } else {
     // If the CMT environment is missing, try to find the xmldoc directory
     // using PathResolver:
     foundpath = PathResolverFindCalibDirectory( "Pythia8/xmldoc" );
  }

  return foundpath;
}

  ////////////////////////////////////////////////////////////////////////////////
int Pythia8_i::s_allowedTunes(double version){
  static std::map<double, int> allowedTunes;
  if(allowedTunes.size()==0){
    allowedTunes[0.]    = 0;
    allowedTunes[8.126] = 1;
    allowedTunes[8.139] = 2;
    allowedTunes[8.140] = 4;
    allowedTunes[8.145] = 5;
    allowedTunes[8.153] = 6;
    allowedTunes[8.160] = 7;
    allowedTunes[8.165] = 11;
    allowedTunes[8.183] = 14;
    allowedTunes[8.2]   = 17;
    allowedTunes[8.204] = 18;
    allowedTunes[8.205] = 32;
  }
  
  std::map<double, int>::const_iterator maxTune = allowedTunes.upper_bound(version + 0.0000001);
  
  if(maxTune != allowedTunes.begin()) --maxTune;
  
  return maxTune->second;
}
  
