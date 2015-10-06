/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <exception>
#include <iostream>
#include <unistd.h>

#include "GaudiKernel/ISvcLocator.h"

#include "MemoryRescueSvc.h"


//- data ------------------------------------------------------------------
char* MemoryRescueSvc::s_block = 0;

//=========================================================================
// Standard Constructor
//=========================================================================
MemoryRescueSvc::MemoryRescueSvc( const std::string& name, ISvcLocator* svc )
  : AthService( name, svc ), m_new( 0 )
{
  declareProperty("NumberOfPages", m_pages=12800); // 50MB on 32, 100MB on 64
  m_new = std::set_new_handler( &MemoryRescueSvc::newHandler );
}

//=========================================================================
// Standard Destructor
//=========================================================================
MemoryRescueSvc::~MemoryRescueSvc()
{
  delete[] s_block; s_block = 0;
  std::set_new_handler( m_new );
}

//=========================================================================
// Re-allocate the rescue block on Service::reinitialize
//=========================================================================
StatusCode MemoryRescueSvc::reinitialize()
{
  delete[] s_block; s_block = 0;

  if ( 0 < (int)m_pages ) {
    ATH_MSG_DEBUG ("allocating block of " << (int)m_pages << " pages");
    s_block = new char[ sysconf( _SC_PAGESIZE ) * m_pages ];
  } else {
    ATH_MSG_DEBUG ("NumberOfPages is zero; only using handler for finalize");
    s_block = new char[ 1 ];
  }

  return s_block ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=========================================================================
// Allocate the rescue block on Service::initialize
//=========================================================================
StatusCode MemoryRescueSvc::initialize()
{
  if ( 0 < (int)m_pages ) {
    ATH_MSG_DEBUG ("allocating block of " << (int)m_pages << " pages");
    s_block = new char[ sysconf( _SC_PAGESIZE ) * m_pages ];
  } else {
    ATH_MSG_DEBUG ("NumberOfPages is zero; only using handler for finalize");
    s_block = new char[ 1 ];
  }

  return s_block  ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=========================================================================
// Release the rescue block on Service::finalize
//=========================================================================
StatusCode MemoryRescueSvc::finalize()
{
  delete[] s_block; s_block = 0;

  return Service::finalize();
}

//=========================================================================
// Block-releasing implementation of the std::new_handler
//=========================================================================
void MemoryRescueSvc::newHandler()
{
  if ( ! s_block )
    throw std::bad_alloc();   // default behavior (no print-out)

// release our block to create working space for finalize()
  delete[] s_block; s_block = 0;

// print onto std::cerr rather than MsgStream, as it's more innocuous
  std::cerr << "MemoryRescueSvc     FATAL out of memory: taking the application down ..."
            << std::endl;

// in order to abort the job, throw a bad_alloc, which should make its way
// through to python, even if the ExceptionSvc is used, and that will shutdown
// (finalize/terminate) the application on exit
  throw std::bad_alloc();
}
