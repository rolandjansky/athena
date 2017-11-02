
#include "Photospp_i/Photospp_i.h"

#include "Photos/Photos.h"

#include "Photos/PhotosHepMCEvent.h"
#include "Photos/Log.h"

#include "GeneratorObjects/McEventCollection.h"
#include "AthenaKernel/errorcheck.h"

#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

using namespace Photospp;

void ignore_unused_variable_warning(int *) {}

extern "C" double phoranc_(int *idum){
  ignore_unused_variable_warning(idum);
//  std::cout<<"*** Called Photospp_i version of phoranc ***"<<std::endl;
//  std::cout<<"photos stream = "<<Photospp_i::photospp_stream()<<std::endl;
  CLHEP::HepRandomEngine *engine = Photospp_i::atRndmGenSvc()->GetEngine(Photospp_i::photospp_stream());\
  return CLHEP::RandFlat::shoot(engine);
}

////////////////////////////////////////////////////////////////////////////////
Photospp_i::Photospp_i(const std::string& name, ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator){

  declareProperty("MCEventKey", m_genEventKey="GEN_EVENT");
  declareProperty("ExponentiationMode", m_exponentiation = true);
  declareProperty("InfraRedCutOff", m_infraRedCutOff=-1.);//1.e-07);//0.01/91.187);
  declareProperty("AlphaQED", m_alphaQED = 0.00729735039);
  declareProperty("WtInterference", m_maxWtInterference=3.);
  declareProperty("CreateHistory", m_createHistory = true);
  declareProperty("StopCriticalErrors", m_stopCritical=false);
  declareProperty("DelayInitialisation", m_delayInitialisation = false);
  declareProperty("ZMECorrection", m_ZMECorrection = false);
  declareProperty("WMECorrection", m_WMECorrection = false);
  declareProperty("PhotonSplitting", m_photonSplitting = false);
}

////////////////////////////////////////////////////////////////////////////////
IAtRndmGenSvc* &Photospp_i::atRndmGenSvc(){
  static IAtRndmGenSvc *p_AtRndmGenSvc = 0;
  return p_AtRndmGenSvc;
}

////////////////////////////////////////////////////////////////////////////////
std::string &Photospp_i::photospp_stream(){
  static std::string s_photospp_stream = "PHOTOSPP_INIT";
  return s_photospp_stream;
}

////////////////////////////////////////////////////////////////////////////////
StatusCode Photospp_i::initialize(){
  ATH_MSG_DEBUG("Photospp_i initializing");

#ifndef HEPMC_HAS_UNITS
  ATH_MSG_ERROR("Photospp_i Requires HepMC with support for units compiled (rel 17 and higher). Bye");
  return StatusCode::FAILURE;
#endif

  StatusCode RndmStatus = service("AtRndmGenSvc", atRndmGenSvc(), true);
  
  if(!RndmStatus.isSuccess() || atRndmGenSvc() == 0){
    ATH_MSG_ERROR("Could not get Random number service!");
    return StatusCode::FAILURE;
  }
  
  CLHEP::HepRandomEngine* engine = atRndmGenSvc()->GetEngine(photospp_stream());
  const long*   sip     =       engine->getSeeds();
  long  int     si1     =       sip[0];
  long  int     si2     =       sip[1];
  
  atRndmGenSvc()->CreateStream(si1, si2, photospp_stream());
  photospp_stream() = "PHOTOSPP";
  
  if(!m_delayInitialisation) setupPhotos();

  return StatusCode::SUCCESS;
}

void Photospp_i::setupPhotos(){
 
  Photos::initialize();
  Photos::setAlphaQED(m_alphaQED);
  Photos::setInterference(true);
  Photos::setCorrectionWtForW(true);
  Photos::maxWtInterference(m_maxWtInterference);
  Photos::setMeCorrectionWtForW(m_WMECorrection);
  Photos::setMeCorrectionWtForZ(m_ZMECorrection);
  Photos::setPairEmission(m_photonSplitting);
  Photos::forceMassFrom4Vector(true);
  Photos::forceMassFromEventRecord(13);
  Photos::forceMassFromEventRecord(15);
  Photos::forceMass(11, 0.510998910);
  Photos::forceMassFromEventRecord(211);
  Photos::setTopProcessRadiation(false);
  Photos::createHistoryEntries(m_createHistory, 3);

  if(m_exponentiation){
    Photos::setExponentiation(true);
  }else{
    Photos::setInfraredCutOff(0.01);
    Photos::setDoubleBrem(true);
    Photos::setQuatroBrem(false);
    phokey.iexp = 0;
  }
  
  // over-ride default IR cutoff if user has set a specific value
  if(m_infraRedCutOff > 0.){
    Photos::setInfraredCutOff(m_infraRedCutOff);
  }
  
  //bool doWarn = (msg().level() == MSG::DEBUG || msg().level()==MSG::VERBOSE);
  
  Photospp::Log::LogWarning(false);
  Photospp::Photos::setStopAtCriticalError(m_stopCritical);
  
  Photos::iniInfo();

  return;
}


StatusCode Photospp_i::execute(){

  // initialise if not done already
  
  if(m_delayInitialisation){
    setupPhotos();
    m_delayInitialisation = false;
  }
  
  // Get the event collection
  McEventCollection* eventCollection;
  StatusCode sc = evtStore()->retrieve(eventCollection, m_genEventKey);
  if (sc.isFailure() || eventCollection == 0) {
    ATH_MSG_ERROR("Unable to retrieve event collection from StoreGate with key " << m_genEventKey);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved event collection from StoreGate with key " << m_genEventKey);
  }

  // Get the event most recently added to the event collection
  HepMC::GenEvent *event = eventCollection->back();
  if(event==0){
    ATH_MSG_ERROR("Photospp_i received a null HepMC event");
    return StatusCode::FAILURE;
  }

  PhotosHepMCEvent photosEvent(event);
  photosEvent.process();

  return StatusCode::SUCCESS;
}

StatusCode Photospp_i::finalize(){

  return StatusCode::SUCCESS;
}

