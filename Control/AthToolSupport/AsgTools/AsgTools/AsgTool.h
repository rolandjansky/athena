// Dear emacs, this is -*- c++ -*-
// $Id: AsgTool.h 619923 2014-10-03 18:49:49Z dadams $
#ifndef ASGTOOLS_ASGTOOL_H
#define ASGTOOLS_ASGTOOL_H

// Local include(s):
#include "AsgTools/AsgToolsConf.h"
#include "AsgTools/IAsgTool.h"
#include "AsgTools/MsgLevel.h"

// Environment specific include(s):
#ifdef ASGTOOL_STANDALONE
#   include "AsgTools/AsgMessaging.h"
#   include "AsgTools/SgTEvent.h"
#elif defined(ASGTOOL_ATHENA)
#   include "AthenaBaseComps/AthAlgTool.h"
#else
#   error "What environment are we in?!?"
#endif // Environment selection

// Forward declaration(s):
class Property;
class PropertyMgr;

namespace asg {

   // Declare the type name of AsgTool's base class
#ifdef ASGTOOL_ATHENA
   typedef ::AthAlgTool AsgToolBase;
#elif defined(ASGTOOL_STANDALONE)
   typedef AsgMessaging AsgToolBase;
#else
#  error "What environment are we in?!?"
#endif // Environment selection

   /// Base class for the dual-use tool implementation classes
   ///
   /// This class can be used like AthAlgTool can be used for Athena-only
   /// tools.
   ///
   /// @author David Adams <dladams@bnl.gov>
   ///
   /// $Revision: 619923 $
   /// $Date: 2014-10-03 20:49:49 +0200 (Fri, 03 Oct 2014) $
   ///
   class AsgTool : public virtual IAsgTool,
                   public AsgToolBase {

   public:
      /// Constructor specifying the tool instance's name
      AsgTool( const std::string& name );
      /// Destructor
      ~AsgTool();

#ifdef ASGTOOL_STANDALONE

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

#endif // ASGTOOL_STANDALONE

#ifdef ASGTOOL_ATHENA

      /// Method to set the type for Gaudi AlgTools.
      /// For expert use only.
      /// The ctor macros call this to set the type which is needed
      /// by the trigger DB
      void setAlgToolType( const std::string& type );

      /// Pull in the usage of the base class's getProperty function
      using ::AthAlgTool::getProperty;

#endif // ASGTOOL_ATHENA

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
#ifdef ASGTOOL_STANDALONE
      std::string m_name; ///< The name of the tool instance
      PropertyMgr* m_ppropmgr; ///< Standalone property manager
      mutable SgTEvent m_event; ///< Wrapper around TEvent/TStore
#endif // ASGTOOL_STANDALONE

   }; // class AsgTool

} // namespace asg

// Include the implementation of the template functions:
#include "AsgTools/AsgTool.icc"

#endif // ASGTOOLS_ASGTOOL_H
