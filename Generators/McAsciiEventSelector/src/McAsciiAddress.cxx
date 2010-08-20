///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAsciiAddress.cxx 
// Implementation file for class McAsciiAddress
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes

// Package includes
#include "McAsciiAddress.h"
#include "McAsciiCnvSvc.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

McAsciiAddress::McAsciiAddress( const CLID& clid,
				const std::string& fname,
				const std::string& cname ) :
  GenericAddress( McAsciiCnvSvc::storageType(), clid, fname, cname ),
  m_runNbr   ( 0 ),
  m_eventNbr ( 0 ),
  m_timeStamp( 0 )
{}

McAsciiAddress::McAsciiAddress( const CLID& clid,
				const std::string& fname,
				const unsigned int runNbr,
				const unsigned int evtNbr,
				const unsigned int timeStamp ) :
  GenericAddress( McAsciiCnvSvc::storageType(), clid, fname, "" ),
  m_runNbr   ( runNbr ),
  m_eventNbr ( evtNbr ),
  m_timeStamp( timeStamp )
{}

// Destructor
///////////////
McAsciiAddress::~McAsciiAddress()
{}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 
