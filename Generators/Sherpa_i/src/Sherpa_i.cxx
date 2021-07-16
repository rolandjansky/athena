/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HepMC/GenEvent.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "Sherpa_i/Sherpa_i.h"

//needed from Sherpa EvtGenerator
#include "SHERPA/Main/Sherpa.H"
#include "SHERPA/Initialization/Initialization_Handler.H"
#include "ATOOLS/Phys/Variations.H"
#include "ATOOLS/Org/Exception.H"
#include "ATOOLS/Org/Run_Parameter.H"

#include <cstdio>
#include <cstring>
#include <signal.h>
#include <stdlib.h>
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

CLHEP::HepRandomEngine* p_rndEngine;

Sherpa_i::Sherpa_i(const std::string& name, ISvcLocator* pSvcLocator) 
  : GenModule(name, pSvcLocator), p_sherpa(NULL)
{
  declareProperty("BaseFragment", m_basefragment = "");
  declareProperty("RunCard", m_runcard = "");
  declareProperty("OpenLoopsLibs", m_openloopslibs);
  declareProperty("ExtraFiles", m_extrafiles);
  declareProperty("NCores", m_ncores=1);
  declareProperty("MemoryMB", m_memorymb=2500.);
  declareProperty("PluginCode", m_plugincode = "");

  declareProperty("VariationWeightCap", m_variation_weight_cap=10.0);
  declareProperty("CrossSectionScaleFactor", m_xsscale=1.0);
  declareProperty("CleanupGeneratedFiles", m_cleanup=true);
}



StatusCode Sherpa_i::genInitialize(){
  if (m_plugincode != "") {
    compilePlugin(m_plugincode);
    m_basefragment += "SHERPA_LDADD: Sherpa_iPlugin \n";
  }
  
  ATH_MSG_INFO("Sherpa initialising...");

  p_rndEngine = atRndmGenSvc().GetEngine("SHERPA");
  const long* sip = p_rndEngine->getSeeds();
  long int si1 = sip[0];
  long int si2 = sip[1];
  atRndmGenSvc().CreateStream(si1, si2, "SHERPA");

  std::map<std::string,std::string> inputfiles;
  inputfiles["Base.yaml"] = m_basefragment;
  inputfiles["Sherpa.yaml"] = m_runcard;
  for(auto& inputfile : inputfiles) {
    // remove first line and last character containing '"'
    // TODO fix Python/C++ string passing, cf. also below
    inputfile.second.erase(0, inputfile.second.find("\n") + 1);
    inputfile.second.pop_back();

    // write input content to file in working directory
    FILE *file = fopen(inputfile.first.c_str(),"w");
    fputs(inputfile.second.c_str(),file);
    fclose(file);
    ATH_MSG_INFO("Sherpa_i using the following settings in "+inputfile.first);
    ATH_MSG_INFO("\n"+inputfile.second+"\n");
  }

  int argc = 2;
  char** argv = new char*[2];
  argv[0] = new char[7];
  argv[1] = new char[34];
  strcpy(argv[0], "Sherpa");
  strcpy(argv[1], "RUNDATA: [Base.yaml, Sherpa.yaml]");
  p_sherpa = new SHERPA::Sherpa(argc, argv);
  delete [] argv;


  /***
      translate ATOOLS:SignalHandler
      TODO: is this even needed?
  ***/
  std::set_terminate(ATOOLS::Terminate);
  std::set_unexpected(ATOOLS::Terminate);
  signal(SIGSEGV,ATOOLS::HandleSignal);
  signal(SIGINT,ATOOLS::HandleSignal);
  signal(SIGPIPE,ATOOLS::HandleSignal);
  signal(SIGBUS,ATOOLS::HandleSignal);
  signal(SIGFPE,ATOOLS::HandleSignal);
  signal(SIGABRT,ATOOLS::HandleSignal);
  signal(SIGTERM,ATOOLS::HandleSignal);
  signal(SIGXCPU,ATOOLS::HandleSignal);
  signal(SIGUSR1,ATOOLS::HandleSignal);

  try {
    p_sherpa->InitializeTheRun();
    p_sherpa->InitializeTheEventHandler();
  }
  catch (const ATOOLS::normal_exit& exception) {
    ATH_MSG_ERROR("Normal exit caught, this probably means:");
    ATH_MSG_ERROR("Have to compile Amegic libraries");
    ATH_MSG_ERROR("You probably want to run ./makelibs");
    return StatusCode::FAILURE;
  }
  catch (const ATOOLS::Exception& exception) {
    ATH_MSG_ERROR("Unwanted ATOOLS::exception caught.");
    ATH_MSG_ERROR(exception);
    return StatusCode::FAILURE;
  }
  catch (std::exception exception) {
    ATH_MSG_ERROR("std::exception caught.");
    return StatusCode::FAILURE;
  }

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
    //double weight_normalisation = event->weights()[2];
    for (size_t i=0; i<event->weights().size(); ++i) {
      //if (i==0 || i>3) event->weights()[i] /= weight_normalisation;
      if (i>3) { // cap variation weights
        // cap variation weights at m_variation_weight_cap*nominal to avoid spikes from numerical instability
        if (fabs(event->weights()[i]) > m_variation_weight_cap*fabs(event->weights()[0])) {
          ATH_MSG_INFO("Capping variation" << i << " = " << event->weights()[i]/event->weights()[0] << "*nominal");
          event->weights()[i] *= m_variation_weight_cap*fabs(event->weights()[0])/fabs(event->weights()[i]);
        }
      }
      ATH_MSG_DEBUG("Sherpa WEIGHT " << i << " value="<< event->weights()[i]);
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

  if (m_cleanup) {
    ATH_MSG_INFO("Deleting left-over files from working directory.");
    system("rm -rf Results.db Process MIG_*.db MPI_*.dat libSherpa*.so libProc*.so");
  }
  
  return StatusCode::SUCCESS;
}




void Sherpa_i::compilePlugin(std::string pluginCode) {
  // TODO: not very pretty, should we eventually do this in Python instead (base fragment)
  FILE *file = fopen("Sherpa_iPlugin.C","w");
  fputs(pluginCode.c_str(),file);
  fclose(file);
  std::string command;
  // Python -> C++ string conversion seems to add quote character as first
  // and last line if the string contains quotes (like always in a plugin)
  // thus removing them here
  command += "tail -n +2 Sherpa_iPlugin.C | head -n -1 > Sherpa_iPlugin.C.tmp; mv Sherpa_iPlugin.C.tmp Sherpa_iPlugin.C; ";
  command += "g++ -shared -std=c++0x -g ";
  command += "`Sherpa-config --cppflags` ";
  command += "`Sherpa-config --ldflags` ";
  command += "-fPIC -o libSherpa_iPlugin.so Sherpa_iPlugin.C";
  ATH_MSG_INFO("Now compiling plugin library using: "+command);
  if (system(command.c_str())!=0) {
    ATH_MSG_ERROR("Error compiling plugin library.");
  }
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
