// Dear emacs, this is -*- c++ -*-
// $Id: IAsgTool.h 615760 2014-09-09 12:50:01Z krasznaa $
#ifndef ASGTOOLS_IASGTOOL_H
#define ASGTOOLS_IASGTOOL_H

// System include(s):
#include <string>

// Local include(s):
#include "AsgTools/AsgToolsConf.h"
#include "AsgTools/AsgToolMacros.h"

// Environment specific include(s):
#ifdef ASGTOOL_ATHENA
#   include "GaudiKernel/IAlgTool.h"
#elif defined(ASGTOOL_STANDALONE)
#   include "AsgTools/StatusCode.h"
#else
#   error "What environment are we in?!?"
#endif // Environment selection

namespace asg {

   /// Base class for the dual-use tool interface classes
   ///
   /// This class is used like IAlgTool is used for Athena-only
   /// interfaces. It is used as the base class for the pure virtual
   /// tool interface classes.
   ///
   /// @author David Adams <dladams@bnl.gov>
   ///
   /// $Revision: 615760 $
   /// $Date: 2014-09-09 14:50:01 +0200 (Tue, 09 Sep 2014) $
   ///
   class IAsgTool
#ifdef ASGTOOL_ATHENA
      : virtual public ::IAlgTool
#endif // ASGTOOL_ATHENA
   {

   public:
      /// Virtual destructor, to make vtable happy...
      virtual ~IAsgTool() {}

#ifdef ASGTOOL_STANDALONE

      /// @name Functions coming from IAlgTool in Athena
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize() = 0;

      /// Return the name of the tool
      virtual const std::string& name() const = 0;
      /// Set the name of the tool
      virtual void setName( const std::string& name ) = 0;

      /// @}

#endif // not ASGTOOL_ATHENA

      /// Print the state of the tool
      virtual void print() const = 0;

   }; // class IAsgTool

} // namespace asg

#endif // ASGTOOLS_IASGTOOL_H
