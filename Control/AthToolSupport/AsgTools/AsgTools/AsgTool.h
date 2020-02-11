/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASGTOOLS_ASGTOOL_H
#define ASGTOOLS_ASGTOOL_H

// Local include(s):
#include "AsgTools/IAsgTool.h"
#include "AsgMessaging/MsgLevel.h"
#include "AsgMessaging/MessageCheck.h"

// Environment specific include(s):
#ifdef XAOD_STANDALONE
#   include "AsgMessaging/AsgMessaging.h"
#   include "AsgTools/SgTEvent.h"
   // Forward declaration(s):
   class Property;
   class PropertyMgr;
#else // XAOD_STANDALONE
#   include "AthenaBaseComps/AthAlgTool.h"
#endif // XAOD_STANDALONE


namespace asg {

   // Declare the type name of AsgTool's base class
#ifndef XAOD_STANDALONE
   typedef ::AthAlgTool AsgToolBase;
#else // not XAOD_STANDALONE
   typedef AsgMessaging AsgToolBase;
#endif // not XAOD_STANDALONE

   /// Base class for the dual-use tool implementation classes
   ///
   /// This class can be used like AthAlgTool can be used for Athena-only
   /// tools.
   ///
   /// @author David Adams <dladams@bnl.gov>
   ///
   /// $Revision: 790657 $
   /// $Date: 2016-12-20 21:54:06 +0100 (Tue, 20 Dec 2016) $
   ///
   class AsgTool : public virtual IAsgTool,
                   public AsgToolBase {

   public:
      /// Constructor specifying the tool instance's name
      AsgTool( const std::string& name );
      /// Destructor
      ~AsgTool();

      AsgTool (const AsgTool&) = delete;
      AsgTool& operator= (const AsgTool&) = delete;
     

#ifdef XAOD_STANDALONE

      /// Stand-alone, StoreGate-like accessor to the event store
      SgTEvent* evtStore() const;

      /// @name Property management functions
      /// @{

      /// Declare a tool property in standalone mode
      template< class T >
      Property* declareProperty( const std::string& name, T& loc,
                                 const std::string& doc = "" );

      /// Set a string property in standalone mode
      StatusCode setProperty( const std::string& name,
                              const char* value );
      /// Set a property in standalone mode
      template< class T >
      StatusCode setProperty( const std::string& name, const T& val );

      /// Get a non-constant pointer to the property manager
      PropertyMgr* getPropertyMgr();
      /// Get a constant pointer to the property manager
      const PropertyMgr* getPropertyMgr() const;

      /// @}

      /// @name Tool name handling functions
      /// @{

      /// Return the name of the tool
      virtual const std::string& name() const;
      /// Set the name of the tool
      virtual void setName( const std::string& name );

      /// @}

#endif // XAOD_STANDALONE

#ifndef XAOD_STANDALONE
   public:
      /// Pull in the usage of the base class's getProperty function
      using ::AthAlgTool::getProperty;

#endif // not XAOD_STANDALONE

      /// @name Additional helper functions, not directly mimicking Athena
      /// @{

      /// Get one of the tool's properties
      template< class T >
      const T* getProperty( const std::string& name ) const;

       /// A deprecated function for getting the message level's name
      const std::string& msg_level_name() const __attribute__ ((deprecated));

      /// @}

      /// Dummy implementation of the initialisation function
      ///
      /// It's here to allow the dual-use tools to skip defining an
      /// initialisation function. Since many are doing so...
      ///
      virtual StatusCode initialize() { return StatusCode::SUCCESS; }

      /// Print the state of the tool
      virtual void print() const;

   private:
#ifdef XAOD_STANDALONE
      std::string m_name; ///< The name of the tool instance
      PropertyMgr* m_ppropmgr; ///< Standalone property manager
      mutable SgTEvent m_event; ///< Wrapper around TEvent/TStore
#endif // XAOD_STANDALONE

   }; // class AsgTool

} // namespace asg

// Include the implementation of the template functions:
#include "AsgTools/AsgTool.icc"

// Include static methods for working with AsgTools
#include "AsgTools/SetProperty.h"

#endif // ASGTOOLS_ASGTOOL_H
