/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASGTOOLS_IASGTOOL_H
#define ASGTOOLS_IASGTOOL_H

// System include(s):
#include <string>

// Local include(s):
#include "AsgTools/AsgToolMacros.h"

// Environment specific include(s):
#ifndef XAOD_STANDALONE
#   include "GaudiKernel/IAlgTool.h"
#else
#   include "AsgMessaging/StatusCode.h"
#   include "AsgMessaging/INamedInterface.h"
#endif

namespace asg {

   /// Base class for the dual-use tool interface classes
   ///
   /// This class is used like IAlgTool is used for Athena-only
   /// interfaces. It is used as the base class for the pure virtual
   /// tool interface classes.
   ///
   /// @author David Adams <dladams@bnl.gov>
   ///
   /// $Revision: 804869 $
   /// $Date: 2017-05-15 22:14:34 +0200 (Mon, 15 May 2017) $
   ///
   class IAsgTool
#ifndef XAOD_STANDALONE
      : virtual public ::IAlgTool
#else
      : virtual public INamedInterface
#endif // not XAOD_STANDALONE
   {

   public:
      /// Virtual destructor, to make vtable happy...
      virtual ~IAsgTool() {}

#ifdef XAOD_STANDALONE

      /// @name Functions coming from IAlgTool in Athena
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize() = 0;

      /// @}

#endif // XAOD_STANDALONE

      /// Print the state of the tool
      virtual void print() const = 0;

   }; // class IAsgTool

} // namespace asg

#endif // ASGTOOLS_IASGTOOL_H
