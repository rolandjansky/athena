///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// IThinningSvc.cxx 
// Implementation file for class IThinningSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// Gaudi includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

// AthenaKernel includes
#include "AthenaKernel/IThinningSvc.h"

// STL includes

namespace {

   /// Dummy implementation for the IThinningHldr interface
   ///
   /// This dummy implementation is used when we want to record information
   /// into a ThinningSvc instance about containers that the service should
   /// not thin itself, but leave it up to the POOL converters to take care
   /// of doing this. Like for thinning xAOD containers.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision$
   /// $Date$
   ///
   class DummyThinningHdlr : public ::Athena::IThinningHdlr {

   public:
      /// Dummy function
      virtual void remove( const std::size_t /*idx*/ ) { return; }
      /// Dummy function
      virtual void commit() { return; }
      /// Dummy function
      virtual void rollback() { return; }

   }; // class DummyThinningHldr

} // private namespace

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////
IThinningSvc::~IThinningSvc()
{}

IThinningSvc* 
IThinningSvc::instance( IThinningSvc* activeSvc, bool overrideSvc )
{
  static thread_local IThinningSvc* svc = 0;
  if ( overrideSvc ) {
    svc = activeSvc;
  }
  return svc;
}

