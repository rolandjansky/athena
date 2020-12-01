/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/getAlfaMagnetConfigFiles.h"
#include "FPTracker/getConfigFile.h"
#include "FPTracker/SimpleLogger.h"
#include "../src/openFile.tpl"
#include <sstream>
#include <stdexcept>

namespace FPTracker{


  boost::shared_ptr<std::ifstream> getAlfaMagnetConfigFiles(const std::string& dir, const Side& side)
  {
    std::string fn = (side == beam1) ? "alfaTwiss1.txt":"alfaTwiss2.txt";
    std::ostringstream ost;
    ost<<"Using ALFA twiss file "<<fn<<'\n';
    SimpleLogger* logger = SimpleLogger::getLogger();
    logger->info( ost );

    return  getConfigFile( dir, fn );
    
  }
  
}

