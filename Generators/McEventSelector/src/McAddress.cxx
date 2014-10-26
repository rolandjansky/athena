/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	McAddress implementation
//--------------------------------------------------------------------
//
//	Package    : McEventSelector
//
//  Description: Event TDR address implementation
//
//	Author     : M.Shapiro, copied from P. Calafiura
//  History    : storegate changes Nov 01 I. Hinchliffe
//====================================================================

//own 
#include "McEventSelector/McAddress.h"
#include "McEventSelector/McCnvSvc.h"

// Framework include files
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/MsgStream.h"

// Event model


#include <iostream>

// Externals

/// Standard Constructor
McAddress::McAddress(const CLID& clid, const std::string& fname, const std::string& cname )
: GenericAddress(McCnvSvc::storageType(), clid, fname, cname),
  m_runNo(0), m_eventNo(0), m_LBNo(0), m_timeStamp(0)
{ }

McAddress::McAddress(const CLID& clid, const std::string& fname, 
	   const unsigned int& run, const uint64_t& event, const unsigned int& LB, const unsigned int& time)
: GenericAddress(McCnvSvc::storageType(), clid, fname, ""),
  m_runNo(run), m_eventNo(event), m_LBNo(LB), m_timeStamp(time)
{ }

