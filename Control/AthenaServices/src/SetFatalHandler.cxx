/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//===================================================================
//      SetFatalHandler.cxx
//--------------------------------------------------------------------
//
//      Package    : AthenaServices
//
//  Description: use the SEAL fatal handler to fatalize SIG_INT.
//
//      Author     : Wim Lavrijsen
//  History    :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who
// +---------+----------------------------------------------+---------
// | 02/10/05| Initial version                              | WL
// +---------+----------------------------------------------+---------
//
//====================================================================


// External
#include "CxxUtils/SealCommon.h"
#include "CxxUtils/SealSignal.h"

// Standard
#include <exception>
#include <iostream>

#include "SetFatalHandler.h"

//- helper -------------------------------------------------------------------
namespace {

   void ExitOnInt( int sig, siginfo_t*, void* ) {
      if ( sig == SIGINT ) {
      // called on user ^C
	 std::cout << std::endl;
         std::cerr << "Athena           CRITICAL stopped by user interrupt\n";
	 raise(SIGKILL);
      }
   }

} // unnamed namespace


//- public members -----------------------------------------------------------
void AthenaServices::SetFatalHandler( int flags ) {

  if ( flags < 0 ) {
    Athena::Signal::handle( SIGINT, ExitOnInt );
  }
  else {
    Athena::Signal::handleFatal(nullptr, IOFD_INVALID, nullptr, nullptr, flags );
    Athena::Signal::handle( SIGINT, ExitOnInt );
  }

}
