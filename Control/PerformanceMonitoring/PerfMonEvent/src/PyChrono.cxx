///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyStore.cxx 
// Implementation file for class Py::Store
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// PerfMonEvent includes
#include "PerfMonEvent/PyChrono.h"

// Framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ChronoEntity.h"
#include "GaudiKernel/StatEntity.h"

// STL includes
#include <stdexcept>

using namespace PerfMon;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

PyChrono::PyChrono() :
  m_chronoSvc( "ChronoStatSvc", "PerfMon::PyChrono" )
{
  if ( ! m_chronoSvc.retrieve().isSuccess() ) {
    throw std::exception();
  }
}

// Destructor
///////////////
PyChrono::~PyChrono()
{}

/////////////////////////////////////////////////////////////////// 
// const methods: 
/////////////////////////////////////////////////////////////////// 


/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void  PyChrono::chronoStart( const std::string& t )
{ m_chronoSvc->chronoStart(t); }
  
void  PyChrono::chronoStop( const std::string& t )
{ m_chronoSvc->chronoStop(t); }

double
PyChrono::chronoDelta( const std::string& t , 
		       const std::string& f )
{
  double v = -999.;
  if ( f == "u" ) { 
    v = static_cast<double>(m_chronoSvc->chronoDelta(t,IChronoStatSvc::USER));
  } else if ( f == "s" ) { 
    v = static_cast<double>(m_chronoSvc->chronoDelta(t,IChronoStatSvc::KERNEL));
  } else if ( f == "r" ) { 
    v = static_cast<double>(m_chronoSvc->chronoDelta(t, IChronoStatSvc::ELAPSED));
  }
  return v;
}
