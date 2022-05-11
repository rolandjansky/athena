/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <string>
#include <cstdlib>

#include "DataQualityInterfaces/HanConfig.h"
#include "DataQualityInterfaces/HanOutput.h"
#include "DataQualityInterfaces/HanInputRootFile.h"
#include "DataQualityInterfaces/ConditionsSingleton.h"
#include "dqm_core/LibraryManager.h"


int main( )
{
  const auto* chconfig = std::getenv("HANCONFIGFILE");
  const auto* cdata = std::getenv("HISTFILE");
  if (!chconfig || !cdata) { 
    std::cerr << "Need to specify at least HANCONFIGFILE and HISTFILE environment variables" << std::endl;
    return 2;
  }
  auto* cpath = std::getenv("HISTFILEPATH");
  auto* coutput = std::getenv("OUTFILE");
  auto* c_cmdlineConditions = std::getenv("CMDLINECONDITIONS");  

  std::string inputName( cdata );
  std::string configName( chconfig );
  std::string pathName( cpath ? cpath : "" );
  std::string outputName;
  if( coutput ) {
    outputName = coutput;
  }
  else {
    std::string::size_type s = inputName.size();
    std::string::size_type p = inputName.rfind( '.', s );
    outputName = inputName.substr( 0, p );
    outputName += "_han.root";
  }
  
  std::string cmdlineConditions = c_cmdlineConditions ? c_cmdlineConditions : "";
  dqi::ConditionsSingleton::getInstance().setCondition(cmdlineConditions);
  if (cmdlineConditions != "") {
    std::cout<<"Input Conditions="<<dqi::ConditionsSingleton::getInstance().getCondition()
	     <<std::endl;
  }

  std::shared_ptr<dqi::HanOutput::DQOutputMap_t> outputMap(new dqi::HanOutput::DQOutputMap_t());
  TSeqCollection *outputList = new TList();
  
  dqi::HanInputRootFile input( inputName, pathName );
  dqi::HanOutput output( outputName, outputMap.get(), outputList );
  output.setInput(const_cast<TDirectory*>(input.getBasedir()));
  
  dqm_core::LibraryManager::instance().loadLibrary( "libdqm_algorithms.so" );
  dqm_core::LibraryManager::instance().loadLibrary( "libdqm_summaries.so" );
  
  dqi::HanConfig config;
  std::cout << "Attempting to build configuration...\n" << std::flush;
  try {
    config.BuildMonitors( configName, input, output );
  } catch (const std::exception& e) {
    std::cerr << "Exception caught, test has failed:\n" << e.what() << std::endl;
    return 1;
  }

  return 0;
}

