///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAsciiFileHelper.cxx 
// Implementation file for McAsciiFileHelper
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <fstream>
#include <stdexcept>

// Framework includes
#include "GaudiKernel/MsgStream.h"

// Package includes
#include "McAsciiFileHelper.h"

namespace McAsciiFileHelper
{ 

std::size_t evtMax( const std::string& name, MsgStream& msg )
{
  std::ifstream asciiFile( name.c_str() );
  if ( !asciiFile.is_open() ||
       !asciiFile.good() ) {
    msg << MSG::ERROR
	<< "Could not open file [" << name << "] !!"
	<< endreq;
    throw std::runtime_error( "McAsciiFileHelper::evtMax: "
			      "Problem opening ascii file !!" );
  }


  std::string line;
  std::size_t evtMax = 0;

  /// Event lines, according to the HepMC::IO_Ascii format,
  /// begin with an 'E', so we look for them
  const std::string evtLine = "E";

  while (! asciiFile.eof() ) {

    getline( asciiFile, line );
    if ( !line.empty() && line[0] == evtLine[0] ) {
//       std::cout << ":::" << line << std::endl;
      ++evtMax;
    }
  }

  asciiFile.close();
  return evtMax;
}

} // namespace McAsciiFileHelper
