/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HepMC/GenEvent.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "Sherpa_i/Sherpa_i.h"

//needed from Sherpa EvtGenerator
#include "SHERPA/Main/Sherpa.H"
#include "SHERPA/Initialization/Initialization_Handler.H"
#include "SHERPA/Tools/Variations.H"
#include "ATOOLS/Org/Exception.H"
#include "ATOOLS/Org/Run_Parameter.H"

#include <cstdio>
#include <cstring>
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

CLHEP::HepRandomEngine* p_rndEngine;

Sherpa_i::Sherpa_i(const std::string& name, ISvcLocator* pSvcLocator) 
  : GenModule(name, pSvcLocator), p_sherpa(NULL)
{
  declareProperty("RunPath", m_path = ".");
  declareProperty("Parameters", m_params);
  declareProperty("CrossSectionScaleFactor", m_xsscale=1.0);
  declareProperty("ScaleVariationReference", m_scalevarref="MUR1_MUF1_PDF261000");
}



StatusCode Sherpa_i::genInitialize(){
  ATH_MSG_INFO("Sherpa initialising...");

  p_rndEngine = atRndmGenSvc().GetEngine("SHERPA");
  const long* sip = p_rndEngine->getSeeds();
  long int si1 = sip[0];
  long int si2 = sip[1];
  atRndmGenSvc().CreateStream(si1, si2, "SHERPA");

  p_sherpa = new SHERPA::Sherpa();


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
    p_sherpa->InitializeTheRun(argc,(char **)argv);
    delete [] argv;
  }
  catch (ATOOLS::Exception exception) {
    if (exception.Class()=="Matrix_Element_Handler" && exception.Type()==ATOOLS::ex::normal_exit) {
      delete p_sherpa;

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
      p_sherpa = new SHERPA::Sherpa();
      p_sherpa->InitializeTheRun(argc,(char **)argv);
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
  p_sherpa->InitializeTheEventHandler();

  return StatusCode::SUCCESS;
}


StatusCode Sherpa_i::callGenerator() {
  ATH_MSG_DEBUG("Sherpa_i in callGenerator()");

  do {
    ATH_MSG_DEBUG("Trying to generate event with Sherpa");
  } while (p_sherpa->GenerateOneEvent()==false);
  
  if (ATOOLS::rpa->gen.NumberOfGeneratedEvents()%1000==0) {
    ATH_MSG_INFO("Passed "<<ATOOLS::rpa->gen.NumberOfGeneratedEvents()<<" events.");
  }

  return StatusCode::SUCCESS;
}

StatusCode Sherpa_i::fillEvt(HepMC::GenEvent* event) {
  ATH_MSG_DEBUG( "Sherpa_i Filling event");

  p_sherpa->FillHepMCEvent(*event);
  if (event->weights().size()>2) {
    double weight_normalisation = event->weights()[2];
    for (size_t i=0; i<event->weights().size(); ++i) {
      if (i==0 || i>3) event->weights()[i] /= weight_normalisation;
      ATH_MSG_DEBUG("Sherpa WEIGHT " << i << " value="<< event->weights()[i]);
    }

    // workaround according to https://sherpa.hepforge.org/trac/ticket/365
    if (event->weights().has_key(m_scalevarref)) {
      double correction_factor = event->weights()[0] / event->weights()[m_scalevarref];
      if (correction_factor != 1.0) {
        ATH_MSG_DEBUG("correction_factor = " << correction_factor);
        for (size_t i=4; i<event->weights().size(); ++i) {
          event->weights()[i] *= correction_factor;
        }
      }
    }
    else {
      ATH_MSG_DEBUG("No weight with key " << m_scalevarref);
    }

  }

  GeVToMeV(event); //unit check

  return StatusCode::SUCCESS;
}

StatusCode Sherpa_i::genFinalize() {

  ATH_MSG_INFO("Sherpa_i finalize()");

  std::cout << "MetaData: generator = Sherpa" << SHERPA_VERSION << "." << SHERPA_SUBVERSION << std::endl;
  std::cout << "MetaData: cross-section (nb)= " << p_sherpa->TotalXS()/1000.0 << std::endl;
  if (m_xsscale!=1.0) {
    std::cout << "MetaData: cross-section*CrossSectionScaleFactor (nb)= " 
              << p_sherpa->TotalXS()/1000.0*m_xsscale << std::endl;
    std::cout << "MetaData: CrossSectionScaleFactor = " << m_xsscale << std::endl;
  }

  std::cout << "MetaData: PDF = " << p_sherpa->PDFInfo() << std::endl;

  std::cout << "Named variations initialised by Sherpa:" << std::endl;
  std::cout << *p_sherpa->GetInitHandler()->GetVariations() << std::endl;

  p_sherpa->SummarizeRun();
  delete p_sherpa;
  
  return StatusCode::SUCCESS;
}


void Sherpa_i::getParameters(int &argc, char** &argv) {
  std::vector<std::string> params;

  // set some ATLAS specific default values as a starting point
  params.push_back("PATH="+m_path);
  params.push_back("EXTERNAL_RNG=Atlas_RNG");
  params.push_back("MAX_PROPER_LIFETIME=10.0");
  params.push_back("BEAM_1=2212");
  params.push_back("BEAM_2=2212");

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
  ATH_MSG_INFO("Further Sherpa initialisation output will be redirected to the LOG_FILE specified above.");

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
DECLARE_GETTER(Atlas_RNG,"Atlas_RNG",External_RNG,RNG_Key);

External_RNG *ATOOLS::Getter<External_RNG,RNG_Key,Atlas_RNG>::operator()(const RNG_Key &) const
{ return new Atlas_RNG(p_rndEngine); }

void ATOOLS::Getter<External_RNG,RNG_Key,Atlas_RNG>::PrintInfo(std::ostream &str,const size_t) const
{ str<<"Atlas RNG interface"; }
