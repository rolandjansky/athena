/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/getMagnetConfigFiles.h"
#include "FPTracker/getConfigFile.h"
#include "FPTracker/SimpleLogger.h"
#include "../src/openFile.tpl"
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace FPTracker{


  std::string determineMagnetConfigFileName(int IP, const Side& side, int magver)
  {
    //  Find twiss magnet configuration files used by P Bussey up until 12/08

    bool argsOK = (IP == 1 || IP == 5) && (side == beam1 || side == beam2); 
    if (!argsOK)
      {
	std::stringstream ss;
	ss<<"Legal side arg values = beam1 or beam2, is "<<side<<" Legal IP arg values = 1 or 5, is "<<IP<<'\n';
	
	throw std::runtime_error(ss.str().c_str());
      }

    
    if(magver == 1)
      {
	if (side  == beam2){return "twiss_65_b2_old.txt";}
	return "twiss_65_b1_old.txt";
      }
    else
      if ( IP==1 ) 
	{
	  if( side==beam2 ) { return "LHCB2IR1_6500";}
	  return "LHCB1IR1_6500";
	}
      else
	{
	  if( side==beam2 ) { return "LHCB2IR5_6500";}
	  return "LHCB1IR5_6500";
	}
    
  }




  boost::shared_ptr<std::ifstream> getMagnetConfigFiles(const std::string& dir, int IP, int magVer, const Side& side)
  {
    
    std::ostringstream ost;
    ost<<determineMagnetConfigFileName(IP, side, magVer)<<'\n';
    SimpleLogger::getLogger()->info(ost);
    return  getConfigFile( dir, determineMagnetConfigFileName(IP, side, magVer) );
    
  }
  
}

