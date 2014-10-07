// Dear emacs, this is -*- c++ -*-
// $Id: PropertyMgr.h 615760 2014-09-09 12:50:01Z krasznaa $
#ifndef ASGTOOLS_PROPERTYMGR_H
#define ASGTOOLS_PROPERTYMGR_H

// Local include(s):
#include "AsgTools/AsgToolsConf.h"

#ifdef ASGTOOL_ATHENA
#   include "GaudiKernel/PropertyMgr.h"
#elif defined(ASGTOOL_STANDALONE)

// System include(s):
#include <string>
#include <map>

// Local include(s):
#include "AsgTools/MsgStream.h"
#include "AsgTools/Property.h"
#include "AsgTools/StatusCode.h"

/// Replacement of Gaudi's property manager in standalone usage
///
/// In order to set the properties of dual-use tools outside of Athena,
/// we provide this ligh-weight replacement for Gaudi's propert manager
/// code.
///
/// @author David Adams <dladams@bnl.gov>
///
/// $Revision: 615760 $
/// $Date: 2014-09-09 14:50:01 +0200 (Tue, 09 Sep 2014) $
///
class PropertyMgr {

public:
   /// Convenience type declaration
   typedef Property::Type Type;

   /// Map of properties, indexed by property name
   typedef std::map< std::string, Property* > PropMap_t;

   /// Default constructor
   PropertyMgr();
   /// Destructor
   ~PropertyMgr();

   /// @name Property management functions
   /// @{

   /// Declare a property, i.e. assign address and type to name.
   template< typename T >
   Property* declareProperty( const std::string& name, T& loc );

   /// Set the value for a string property
   StatusCode setProperty( const std::string& name, const char* value );
   /// Set the value for a property
   template< typename T >
   StatusCode setProperty( const std::string& name, const T& val );

   /// Retrieve a property
   template< typename T >
   StatusCode getProperty( const std::string& name, T& val ) const;

   /// Retrieve a typeless property
   const Property* getProperty( const std::string& name ) const;

   /// Return the map of properties
   const PropMap_t& getProperties() const;

   /// @}

   /// @name Message stream handling functions
   /// @{

   /// Return the message stream
   MsgStream& msg() const;

   /// Return the message stream, set to a certain level
   MsgStream& msg( MSG::Level level ) const;

   /// @}

private:
   /// Internap map of properties
   PropMap_t m_props;
   /// Message logger object
   mutable MsgStream m_msg;

}; // class PropertyMgr

// Include the template implementations:
#include "AsgTools/PropertyMgr.icc"

#else
#   error "What environment are we in?!?"
#endif // Environment selection
#endif // ASGTOOLS_PROPERTYMGR_H
