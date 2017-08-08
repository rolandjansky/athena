///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/** @brief helper method to decide if an index value is thinned or not
 */
bool 
IThinningSvc::isThinned ( const std::size_t idx )
{
  return idx == IThinningSvc::RemovedIdx;
}

template <>
std::size_t 
IThinningSvc::generateRemovedIdx<std::size_t>()
{
  return IThinningSvc::RemovedIdx;
}

template <>
std::string 
IThinningSvc::generateRemovedIdx<std::string>()
{
  return "--RemovedIdx--";
}

IThinningSvc* 
IThinningSvc::instance( IThinningSvc* activeSvc, bool overrideSvc )
{
  static thread_local IThinningSvc* svc = 0;
  if ( overrideSvc ) {
    svc = activeSvc;
  }
  return svc;
}

StatusCode
IThinningSvc::typelessFilter( const void* in,
                              const IThinningSvc::VecFilter_t& filter,
                              const IThinningSvc::Operator::Type op ) {

   // Translate the filter:
   IThinningSvc::Filter_t sparse_filter;
   const std::size_t imax = filter.size();
   for ( std::size_t i = 0; i != imax; ++i ) {
      sparse_filter[ i ] = filter[ i ];
   }

   // Call the other function:
   return this->typelessFilter( in, sparse_filter, op );
}

StatusCode
IThinningSvc::typelessFilter( const void* in,
                              const IThinningSvc::Filter_t& filter,
                              const IThinningSvc::Operator::Type op ) {

   // Create a dummy thinning handler object:
   ::DummyThinningHdlr* dummyHandler = new ::DummyThinningHdlr();

   // Get the DataProxy of the object:
   SG::DataProxy* proxy = this->proxy( in );

   // Leave it to the service implementation to do the rest:
   return this->filter_impl( dummyHandler, proxy, filter, op );
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
