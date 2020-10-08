/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "Pythia8_i/Pythia8_i.h"
#include "Pythia8_i/UserProcessFactory.h"
#include "Pythia8_i/UserResonanceFactory.h"
#include "Pythia8_i/IPythia8Custom.h"

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

#include <sstream>

// Name of AtRndmGenSvc stream
std::string     Pythia8_i::pythia_stream   = "PYTHIA8_INIT";

using boost::assign::operator+=;

// fix Pythia8 shower weights change in conventions
#define PYTHIA8_NWEIGHTS nWeights
#define PYTHIA8_WEIGHT weight
#define PYTHIA8_WLABEL weightLabel
#define PYTHIA8_CONVERSION 1.0

#ifdef PYTHIA_VERSION_INTEGER
  #if PYTHIA_VERSION_INTEGER > 8230
  #undef PYTHIA8_NWEIGHTS
  #undef PYTHIA8_WEIGHT
  #undef PYTHIA8_WLABEL
  #define PYTHIA8_NWEIGHTS nVariationGroups
  #define PYTHIA8_WEIGHT getGroupWeight
  #define PYTHIA8_WLABEL getGroupName
  #if PYTHIA_VERSION_INTEGER < 8244
    #undef PYTHIA8_CONVERSION
    #define PYTHIA8_CONVERSION 1.0e9
  #endif 
  #endif
#endif

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
m_sigmaTotal(0.),
m_conversion(1.),
m_failureCount(0),
m_procPtr(0),
m_userHooksPtrs(),
m_doLHE3Weights(false),
m_athenaTool("IPythia8Custom")
{
  declareProperty("Commands", m_commands);
  declareProperty("CollisionEnergy", m_collisionEnergy = 14000.0);
  declareProperty("useRndmGenSvc", m_useRndmGenSvc = true);
  declareProperty("Beam1", m_beam1 = "PROTON");
  declareProperty("Beam2", m_beam2 = "PROTON");
  declareProperty("LHEFile", m_lheFile = "");
  declareProperty("StoreLHE", m_storeLHE=false);
  declareProperty("CKKWLAcceptance", m_doCKKWLAcceptance = false);
  declareProperty("FxFxXS", m_doFxFxXS = false);
  declareProperty("MaxFailures", m_maxFailures = 10);//the max number of consecutive failures
  declareProperty("UserProcess", m_userProcess="");
  declareProperty("UserHooks", m_userHooks={});
  declareProperty("UserResonances", m_userResonances="");
  declareProperty("UseLHAPDF", m_useLHAPDF=true);
  declareProperty("ParticleData", m_particleDataFile="");
  declareProperty("OutputParticleData",m_outputParticleDataFile="ParticleData.local.xml");
  declareProperty("ShowerWeightNames",m_showerWeightNames);
  declareProperty("CustomInterface",m_athenaTool);


  m_particleIDs["PROTON"]      = PROTON;
  m_particleIDs["ANTIPROTON"]  = ANTIPROTON;
  m_particleIDs["ELECTRON"]    = ELECTRON;
  m_particleIDs["POSITRON"]    = POSITRON;
  m_particleIDs["NEUTRON"]     = NEUTRON;
  m_particleIDs["ANTINEUTRON"] = ANTINEUTRON;
  m_particleIDs["MUON"]        = MUON;
  m_particleIDs["ANTIMUON"]    = ANTIMUON;
  m_particleIDs["LEAD"]        = LEAD;

  ATH_MSG_INFO("XML Path is " + xmlpath());

}

////////////////////////////////////////////////////////////////////////////////
Pythia8_i::~Pythia8_i() {

  delete m_atlasRndmEngine;

  if(m_procPtr != 0)     delete m_procPtr;
  
  #ifndef PYTHIA8_3SERIES
  for(UserHooksPtrType ptr: m_userHooksPtrs){
    delete ptr;
  }
  #endif
}

////////////////////////////////////////////////////////////////////////////////
StatusCode Pythia8_i::genInitialize() {

  ATH_MSG_DEBUG("Pythia8_i from genInitialize()");

  bool canInit = true;

  m_version = m_pythia.settings.parm("Pythia:versionNumber");

  Pythia8_i::pythia_stream =       "PYTHIA8_INIT";

  // By default add "nominal" to the list of shower weight names
  m_showerWeightNames.insert(m_showerWeightNames.begin(), "nominal");

  // We do explicitly set tune 4C, since it is the starting point for many other tunes
  // Tune 4C for pp collisions
  m_pythia.readString("Tune:pp = 5");

  // also use CTEQ6L1 from LHAPDF 6 by default
  // can be over-written using JO
  m_pythia.readString("PDF:pSet= LHAPDF6:cteq6l1");

  // switch off verbose event print out
  m_pythia.readString("Next:numberShowEvent = 0");

  // Add UserHooks first because these potentially add new settings that must exist prior to parsing commands

  bool firstHook=true;
  for(const auto &hook: m_userHooks){
    ATH_MSG_INFO("Adding user hook " + hook + ".");
    m_userHooksPtrs.push_back(PYTHIA8_PTRWRAP(Pythia8_UserHooks::UserHooksFactory::create(hook)) );
    bool canSetHook = true;
    if(firstHook){
      canSetHook = m_pythia.setUserHooksPtr(m_userHooksPtrs.back());
      firstHook = false;
    }else{
      canSetHook = m_pythia.addUserHooksPtr(m_userHooksPtrs.back());
    }

    if(!canSetHook){
      ATH_MSG_ERROR("Unable to set requested user hook.");
      ATH_MSG_ERROR("Pythia 8 initialisation will FAIL!");
      canInit=false;
    }
  }

  for(const std::pair<std::string, double> &param : Pythia8_UserHooks::UserHooksFactory::userSettings<double>()){
    m_pythia.settings.addParm(param.first, param.second, false, false, 0., 0.);
  }

  for(const std::pair<std::string, int> &param : Pythia8_UserHooks::UserHooksFactory::userSettings<int>()){
    m_pythia.settings.addMode(param.first, param.second, false, false, 0., 0.);
  }

  for(const std::pair<std::string, int> &param : Pythia8_UserHooks::UserHooksFactory::userSettings<bool>()){
    m_pythia.settings.addFlag(param.first, param.second);
  }

  for(const std::pair<std::string, std::string> &param : Pythia8_UserHooks::UserHooksFactory::userSettings<std::string>()){
    m_pythia.settings.addWord(param.first, param.second);
  }

  if(m_athenaTool.typeAndName() != "IPythia8Custom"){
    if(m_athenaTool.retrieve().isFailure()){
      ATH_MSG_ERROR("Unable to retrieve Athena Tool for custom Pythia processing");
      return StatusCode::FAILURE;
    }
    else {
      StatusCode status = m_athenaTool->InitializePythiaInfo(m_pythia);
      if(status != StatusCode::SUCCESS) return status;
    }
  }


  // Now apply the settings from the JO
  foreach(const string &cmd, m_commands){

    if(cmd.compare("")==0) continue;

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

  if(m_athenaTool.typeAndName() != "IPythia8Custom"){
    returnCode = returnCode && m_athenaTool->ModifyPythiaEvent(m_pythia);
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

       // For FxFx cross section:
       m_sigmaTotal+=m_pythia.info.weight();
     }

  return returnCode;
}

///////////////////////////////////////////////////////////////////////////////
StatusCode Pythia8_i::fillEvt(HepMC::GenEvent *evt){

  ATH_MSG_DEBUG(">>> Pythia8_i from fillEvt");

  evt->set_event_number(m_internal_event_number);

  // if using "getGroupWeight" and | lhastrategy | = 4, then need to convert mb to pb ( done otherwise when calling info.weight(), [...] )
  if( m_internal_event_number == 1 && abs(m_pythia.info.lhaStrategy()) == 4 ) {
     m_conversion = ( (double) PYTHIA8_CONVERSION);
     ATH_MSG_DEBUG(" LHA strategy needs a conversion to fix Pythia8 shower weights bug(s) equal to " << m_conversion);
  }


  if(m_pythia.event.size() < 2){
    ATH_MSG_ERROR("Something wrong with this event - it contains fewer than 2 particles!");
    ATH_MSG_ERROR("internal event number is "<<m_internal_event_number);
    return StatusCode::FAILURE;
  }

  m_pythiaToHepMC.fill_next_event(m_pythia, evt, m_internal_event_number);

  if(m_lheFile != "" && m_storeLHE) addLHEToHepMC(evt);

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

  // fill weights
  StatusCode returnCode = fillWeights(evt);

  return returnCode;
}

///////////////////////////////////////////////////////////////////////////////
StatusCode Pythia8_i::fillWeights(HepMC::GenEvent *evt){

  // reset
  m_mergingWeight    = m_pythia.info.mergingWeightNLO(); // first initialisation, good for the nominal weightin any case (see Merging:includeWeightInXsection).
  m_enhanceWeight    = 1.0;  // better to keep the enhancement from UserHooks in a separate variable, to keep the code clear
  evt->weights().clear();

  // include Enhance userhook weight
  for(const auto &hook: m_userHooksPtrs) {
    if (hook->canEnhanceEmission()) {
      m_enhanceWeight *= hook->getEnhancedEventWeight();
    }
  }

  // DO NOT try to distinguish between phase space and merging contributions: only their product contains both, so always multiply them in order to be robust
  double eventWeight = m_pythia.info.weight() * m_mergingWeight * m_enhanceWeight; 

  // save as Default the usual pythia8 weight, including enhancements
  // NB: info.weight() is built upon info.eventWeightLHEF (but includes shower "decorations"), not from the rwgt LHE vector.
  size_t atlas_specific_weights = 1;
  evt->weights()["Default"]=eventWeight;
  if(m_internal_event_number == 1)  m_weightIDs.push_back("Default");

  // Now systematic weights: a) generator and b) shower variations
  // merging implies reading from lhe files (while the opposite is not true, ok, but still works)
  if (m_lheFile!="") {
       // as a backup, save also the LHEF weight without enhancements (useful for possible, rare, non-default cases)
       // Also remember that the true merging component can be disentangled only when there are LHE input events
       evt->weights()["Bare_not_for_analyses"]=m_pythia.info.eventWeightLHEF;
       if(m_internal_event_number == 1)  m_weightIDs.push_back("Bare_not_for_analyses");
       atlas_specific_weights=2; // "Default" and "Bare_not_for_analyses" 


       // make sure to get the merging weight correctly: this is needed  -only- for systematic weights (generator and parton shower variations)
       // NB: most robust way, regardless of the specific value of Merging:includeWeightInXsection (if on, info.mergingWeight() is always 1.0 ! )
       m_mergingWeight *= ( m_pythia.info.weight() / m_pythia.info.eventWeightLHEF ); // this is the actual merging component, regardless of conventions,
                                                                                      // needed by systematic variation weights
                                                                                      // this includes rare compensation or selection-biased weights
  }

  ATH_MSG_DEBUG("Event weights: enhancing weight, merging weight, total weight = "<<m_enhanceWeight<<", "<<m_mergingWeight<<", "<<eventWeight);

  // we already filled the "Default" and, possibly, "Bare_not_for_analyses" weights
  std::vector<string>::const_iterator id = m_weightIDs.begin()+atlas_specific_weights;

  // a) fill generator weights
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
        evt->weights()[weightName->second.contents] = m_mergingWeight * m_enhanceWeight * wgt->second.contents;
      }else{
        evt->weights()[wgt->first] = m_mergingWeight * m_enhanceWeight * wgt->second.contents;
      }

    }
  }

  // b) shower weight variations
  // always start from second shower weight: the first was saved already as "Default"

  for(int iw = 1; iw < m_pythia.info.PYTHIA8_NWEIGHTS(); ++iw){

    std::string wtName = ((int)m_showerWeightNames.size() == m_pythia.info.PYTHIA8_NWEIGHTS())? m_showerWeightNames[iw]: "ShowerWt_" + std::to_string(iw);

    evt->weights()[wtName] = m_mergingWeight * m_enhanceWeight * m_pythia.info.PYTHIA8_WEIGHT(iw)*m_conversion;

    if(m_internal_event_number == 1) {
        m_weightIDs.push_back(wtName);
        ATH_MSG_DEBUG("Shower weight name, value, conversion: "<<wtName<<", "<< evt->weights()[wtName]  <<","<<m_conversion );
    }
  }

  return SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
StatusCode Pythia8_i::genFinalize(){

  ATH_MSG_INFO(">>> Pythia8_i from genFinalize");

  m_pythia.stat();

  Pythia8::Info info = m_pythia.info;
  double xs = info.sigmaGen(); // in mb

  if(m_doCKKWLAcceptance){
    const double accfactor = m_nMerged / info.nAccepted();
    ATH_MSG_DEBUG("Multiplying cross-section by CKKWL merging acceptance of "<<m_nMerged <<"/" <<info.nAccepted() << " = " << accfactor
                  << ": " << xs << " -> " << xs*accfactor);
    xs *= accfactor;
  }

  if(m_doFxFxXS){
    ATH_MSG_DEBUG("Using FxFx cross section recipe: xs = "<< m_sigmaTotal << " / " << 1e9*info.nTried());
    xs = m_sigmaTotal / (1e9*info.nTried());
    std::cout << "Using FxFx cross section recipe: xs = "<< m_sigmaTotal << " / " << 1e9*info.nTried() << std::endl;
  }

  if(m_athenaTool.typeAndName() != "IPythia8Custom"){
    double xsmod = m_athenaTool->CrossSectionScaleFactor();
    ATH_MSG_DEBUG("Multiplying cross-section by Pythia Modifier tool factor " << xsmod );
    xs *= xsmod;
  }

  xs *= 1000. * 1000.;//convert to nb

  std::cout << "MetaData: cross-section (nb)= " << xs <<std::endl;
  std::cout << "MetaData: generator= Pythia 8." << PY8VERSION <<std::endl;

  if(m_doLHE3Weights || m_weightIDs.size()>1 ){
    std::cout<<"MetaData: weights = ";

    foreach(const string &id, m_weightIDs){
      if(m_doLHE3Weights) {
        std::map<string, Pythia8::LHAweight>::const_iterator weight = m_pythia.info.init_weights->find(id);
        if(weight != m_pythia.info.init_weights->end()){
	  std::cout<<weight->second.contents<<" | ";
        }else{
	  std::cout<<id << " | ";
        }
      }
      else {
        std::cout << id << " | ";
      }
    }
  std::cout<<std::endl;
  }

  return SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
void Pythia8_i::addLHEToHepMC(HepMC::GenEvent *evt){

  HepMC::GenEvent *procEvent = new HepMC::GenEvent(evt->momentum_unit(), evt->length_unit());

  // Adding the LHE event to the HepMC results in undecayed partons in the event record.
  // Pythia's HepMC converter throws up undecayed partons, so we ignore that
  // (expected) exception this time
  try{
    m_pythiaToHepMC.fill_next_event(m_pythia.process, procEvent, evt->event_number(), &m_pythia.info, &m_pythia.settings);
  }catch(HepMC::PartonEndVertexException &ignoreIt){}

  for(HepMC::GenEvent::particle_iterator p = procEvent->particles_begin();
      p != procEvent->particles_end(); ++p){
    (*p)->set_status(1003);
  }

  std::vector<HepMC::GenParticle*> beams;
  std::vector<HepMC::GenParticle*> procBeams;
  beams.push_back(evt->beam_particles().first);
  beams.push_back(evt->beam_particles().second);

  if(beams[0]->momentum().pz() * procEvent->beam_particles().first->momentum().pz() > 0.){
    procBeams.push_back(procEvent->beam_particles().first);
    procBeams.push_back(procEvent->beam_particles().second);
  }else{
    procBeams.push_back(procEvent->beam_particles().second);
    procBeams.push_back(procEvent->beam_particles().first);
  }

  std::map<const HepMC::GenVertex*, HepMC::GenVertex*> vtxCopies;

  for(HepMC::GenEvent::vertex_const_iterator v = procEvent->vertices_begin();
      v != procEvent->vertices_end(); ++v ) {
    if(*v == procBeams[0]->end_vertex() || *v == procBeams[1]->end_vertex()) continue;
    HepMC::GenVertex* vCopy = new HepMC::GenVertex((*v)->position(), (*v)->id(), (*v)->weights());
    vCopy->suggest_barcode(-(evt->vertices_size()));
    vCopy->set_id(1);
    vtxCopies[*v] = vCopy;
    evt->add_vertex(vCopy);
  }

  for(HepMC::GenEvent::particle_const_iterator p = procEvent->particles_begin();
      p != procEvent->particles_end(); ++p ){
    if((*p)->is_beam()) continue;

    HepMC::GenParticle *pCopy = new HepMC::GenParticle(*(*p));
    pCopy->suggest_barcode(evt->particles_size());

    std::map<const HepMC::GenVertex*, HepMC::GenVertex*>::iterator vit;
    for(size_t ii =0; ii != 2; ++ii){
      if((*p)->production_vertex() == procBeams[ii]->end_vertex()){
        beams[ii]->end_vertex()->add_particle_out(pCopy);
        break;
      }

      if(ii == 1){
        vit = vtxCopies.find((*p)->production_vertex());
        if(vit != vtxCopies.end()) vit->second->add_particle_out(pCopy);
      }
    }

    vit = vtxCopies.find((*p)->end_vertex());
    if(vit != vtxCopies.end()) vit->second->add_particle_in(pCopy);
  }

  return;
}

////////////////////////////////////////////////////////////////////////


double Pythia8_i::pythiaVersion()const{
  return m_version;
}

////////////////////////////////////////////////////////////////////////
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

// fix Pythia8 shower weights change in conventions
#ifdef PYTHIA8_NWEIGHTS
  #undef PYTHIA8_NWEIGHTS
  #undef PYTHIA8_WEIGHT
  #undef PYTHIA8_WLABEL
  #undef PYTHIA8_CONVERSION
#endif
