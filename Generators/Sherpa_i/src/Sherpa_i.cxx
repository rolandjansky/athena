/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HepMC/GenEvent.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "Sherpa_i/Sherpa_i.h"

//needed from Sherpa EvtGenerator
#include "SHERPA/Main/Sherpa.H"
#include "SHERPA/Tools/HepMC2_Interface.H"
#include "SHERPA/Single_Events/Event_Handler.H"
#include "SHERPA/Initialization/Initialization_Handler.H"
#include "SHERPA/PerturbativePhysics/Matrix_Element_Handler.H"
#include "PDF/Main/ISR_Handler.H"
#include "ATOOLS/Phys/Blob_List.H"
#include "ATOOLS/Org/Exception.H"
#include "ATOOLS/Org/Run_Parameter.H"
#include "ATOOLS/Org/MyStrStream.H"
#include "ATOOLS/Org/CXXFLAGS.H"
#include "SHERPA/Tools/Output_Base.H"
#include "SHERPA/Tools/Output_Sherpa.H"

#include <cstdio>
#include <cstring>
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

CLHEP::HepRandomEngine* p_rndEngine;

Sherpa_i::Sherpa_i(const std::string& name, ISvcLocator* pSvcLocator) 
  : GenModule(name, pSvcLocator),
    p_generator(NULL), p_hepmc2_i(NULL)
{
  declareProperty("RunPath", m_path = ".");
  declareProperty("Parameters", m_params);
  declareProperty("CrossSectionScaleFactor", m_xsscale=1.0);
  declareProperty("ResetWeight", m_resetweight=1);
}



StatusCode Sherpa_i::genInitialize(){
  ATH_MSG_INFO("Sherpa initialising...");

  p_rndEngine = atRndmGenSvc().GetEngine("SHERPA");
  const long* sip = p_rndEngine->getSeeds();
  long int si1 = sip[0];
  long int si2 = sip[1];
  atRndmGenSvc().CreateStream(si1, si2, "SHERPA");

  p_generator = new SHERPA::Sherpa();
  p_hepmc2_i = new SHERPA::HepMC2_Interface();


  /***
      translate ATOOLS:SignalHandler
  ***/
  std::set_terminate(ATOOLS::Terminate);
  std::set_unexpected(ATOOLS::Terminate);
  signal(SIGSEGV,ATOOLS::SignalHandler);
  signal(SIGINT,ATOOLS::SignalHandler);
  signal(SIGBUS,ATOOLS::SignalHandler);
  signal(SIGFPE,ATOOLS::SignalHandler);
  signal(SIGABRT,ATOOLS::SignalHandler);
  signal(SIGTERM,ATOOLS::SignalHandler);
  signal(SIGXCPU,ATOOLS::SignalHandler);

  try {
    int argc;
    char** argv;
    getParameters(argc, argv);
    p_generator->InitializeTheRun(argc,(char **)argv);
    delete [] argv;
  }
  catch (ATOOLS::Exception exception) {
    if (exception.Class()=="Amegic" && exception.Type()==ATOOLS::ex::normal_exit) {
      delete p_generator;

      ATH_MSG_INFO("Have to compile Amegic libraries");
      std::string tmp="cd "+m_path+"; ./makelibs && cd -;";
      int stat = system(tmp.c_str());
      if (stat==0) {
	ATH_MSG_INFO("Finished compiling Amegic libraries");
      }
      else {
	ATH_MSG_ERROR("Error while compiling Amegic libraries.");
	return StatusCode::FAILURE;
      }

      int argc;
      char** argv;
      getParameters(argc, argv);
      p_generator = new SHERPA::Sherpa();
      p_generator->InitializeTheRun(argc,(char **)argv);
      delete [] argv;
    }
    else {
      ATH_MSG_ERROR("Unwanted ATOOLS::exception caught.");
      ATH_MSG_ERROR(exception);
      return StatusCode::FAILURE;
    }
  }
  catch (std::exception exception) {
    ATH_MSG_ERROR("std::exception caught.");
    return StatusCode::FAILURE;
  }
  p_generator->InitializeTheEventHandler();

  return StatusCode::SUCCESS;
}


StatusCode Sherpa_i::callGenerator() {
  ATH_MSG_DEBUG("Sherpa_i in callGenerator()");

  p_generator->GenerateOneEvent();
  
  if (ATOOLS::rpa->gen.NumberOfGeneratedEvents()%10000==0) {
    ATH_MSG_WARNING("Passed "<<ATOOLS::rpa->gen.NumberOfGeneratedEvents()<<" events.");
  }
  else if (ATOOLS::rpa->gen.NumberOfGeneratedEvents()%1000==0) {
    ATH_MSG_INFO("Passed "<<ATOOLS::rpa->gen.NumberOfGeneratedEvents()<<" events.");
  }

  return StatusCode::SUCCESS;
}

StatusCode Sherpa_i::fillEvt(HepMC::GenEvent* event) {
  ATH_MSG_DEBUG( "Sherpa_i Filling event");

  // Fill event from Sherpa
  ATOOLS::Blob_List* blobs=p_generator->GetEventHandler()->GetBlobs();

  double weight = blobs->Weight();
  // if evgen mode is unweighted, then set the weight to
  // 1.0(/enhancefactor if present) to avoid confusion with the weights
  // being constant but != 1.0
  SHERPA::Matrix_Element_Handler* meh=p_generator->GetInitHandler()->GetMatrixElementHandler();
  if (meh->EventGenerationMode()==1 && m_resetweight) {
    if (weight>0.0) weight=1.0;
    else weight=-1.0;
    ATOOLS::Blob* signal=blobs->FindFirst(ATOOLS::btp::Signal_Process);
    if (signal) {
      ATOOLS::Blob_Data_Base* data = (*signal)["Enhance"];
      if (data) {
        weight /= data->Get<double>();
      }
      else ATH_MSG_WARNING("Did not find Enhance factor information.");
    }
    else {
      ATH_MSG_WARNING("Did not find signal blob.");
    }
  }
  p_hepmc2_i->Sherpa2HepMC(blobs, *event, weight);

  // Set beam particle status = 4
  if (event->valid_beam_particles()) {
    event->beam_particles().first->set_status(4);
    event->beam_particles().second->set_status(4);
  }

  GeVToMeV(event); //unit check

  return StatusCode::SUCCESS;
}

StatusCode Sherpa_i::genFinalize() {

  ATH_MSG_INFO("Sherpa_i finalize()");

  std::cout << "MetaData: generator = Sherpa " << SHERPA_VERSION << "." << SHERPA_SUBVERSION << std::endl;
  std::cout << "MetaData: cross-section (nb)= " 
            << p_generator->GetEventHandler()->TotalXS()/1000.0 << std::endl;
  if (m_xsscale!=1.0) {
    std::cout << "MetaData: cross-section*CrossSectionScaleFactor (nb)= " 
              << p_generator->GetEventHandler()->TotalXS()/1000.0*m_xsscale << std::endl;
    std::cout << "MetaData: CrossSectionScaleFactor = " << m_xsscale << std::endl;
  }

  std::string pdf = "Unknown";
  if (p_generator->GetInitHandler()->GetISRHandler(PDF::isr::hard_process) &&
      p_generator->GetInitHandler()->GetISRHandler(PDF::isr::hard_process)->PDF(0))
    pdf=p_generator->GetInitHandler()->GetISRHandler(PDF::isr::hard_process)->PDF(0)->Type();
  std::cout << "MetaData: PDF = " << pdf << std::endl;


  p_generator->SummarizeRun();
  delete p_generator;
  
  return StatusCode::SUCCESS;
}


void Sherpa_i::getParameters(int &argc, char** &argv) {
  std::vector<std::string> params;

  // set some ATLAS specific default values as a starting point
  params.push_back("EVENTS=1"); // will be ignored and steered by Athenas EvtMax
  params.push_back("PATH="+m_path);
  params.push_back("EXTERNAL_RNG=Atlas_RNG");
  params.push_back("MASS[6]=172.5");
  params.push_back("MASS[23]=91.1876");
  params.push_back("MASS[24]=80.399");
  params.push_back("WIDTH[23]=2.4952");
  params.push_back("WIDTH[24]=2.085");
  params.push_back("BEAM_1=2212");
  params.push_back("BEAM_2=2212");
  params.push_back("BEAM_ENERGY_1=3500");
  params.push_back("BEAM_ENERGY_2=3500");
  params.push_back("MAX_PROPER_LIFETIME=10.0");
  params.push_back("MI_HANDLER=Amisic");

  /***
      Adopt Atlas Debug Level Scheme
  ***/

  std::string verbose_arg;
  MsgStream log(messageService(), name());
  if( log.level()==MSG::FATAL || log.level()==MSG::ERROR || log.level()==MSG::WARNING ){
    params.push_back("OUTPUT=0");
  }
  else if(log.level()==MSG::INFO){
    params.push_back("OUTPUT=2");
  }
  else if(log.level()==MSG::DEBUG){
    params.push_back("OUTPUT=3");
  }
  else{
    params.push_back("OUTPUT=15");
  }
  
  // allow to overwrite all parameters from JO file
  params.insert(params.begin()+params.size(), m_params.begin(), m_params.end());

  /***
      Convert into Sherpas argc/argv arguments
  ***/
  argc = 1+params.size();
  argv = new char * [ 1+params.size() ];
  argv[0] = new char[7];
  strcpy(argv[0], "Sherpa");

  ATH_MSG_INFO("Sherpa_i using the following Arguments");
  for(size_t i=0; i<params.size(); i++) {
    ATH_MSG_INFO(" [ " << params[i] << " ] ");
    argv[i+1] = new char[params[i].size()+1];
    strcpy(argv[i+1], params[i].c_str());
  }
  ATH_MSG_INFO("End Sherpa_i Argument List");

}




/**
   Use ATLAS random number generator for Sherpa's internal random numbers
**/
using namespace ATOOLS;

Atlas_RNG::Atlas_RNG(CLHEP::HepRandomEngine* engine) :
  External_RNG(), p_engine(engine)
{
}

double Atlas_RNG::Get(){

  return CLHEP::RandFlat::shoot(p_engine);

}

// some getter magic to make this random number generator available to sherpa
DECLARE_GETTER(Atlas_RNG_Getter,"Atlas_RNG",External_RNG,RNG_Key);
External_RNG *Atlas_RNG_Getter::operator()(const RNG_Key &) const
{ return new Atlas_RNG(p_rndEngine); }
void Atlas_RNG_Getter::PrintInfo(std::ostream &str,const size_t) const
{ str<<"Atlas RNG interface"; }
