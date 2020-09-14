/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASGTOOLS_TOOLHANDLEARRAY_H
#define ASGTOOLS_TOOLHANDLEARRAY_H

// Local include(s):
#include "AsgTools/AsgToolsConf.h"

#ifndef XAOD_STANDALONE
#   include "GaudiKernel/ToolHandle.h"
#else // not XAOD_STANDALONE

// System include(s):
#include <vector>
#if __cplusplus >= 201100
#   include <initializer_list>
#endif // C++11

// Local include(s):
#include "AsgTools/ToolHandle.h"

/// Partial re-implementation of Gaudi's ToolHandleArray
///
/// This class makes it possible to use ToolHandleArrays in dual-use tools
/// in a uniform way between Athena and ROOT.
///
/// @author David Adams <dladams@bnl.gov>
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 613788 $
/// $Date: 2014-08-28 11:22:42 +0200 (Thu, 28 Aug 2014) $
///
template< class T >
class ToolHandleArray : public std::vector< ToolHandle< T > > {

public:
   /// Default constructor
   ToolHandleArray();
   /// Constructor with an initialiser list
   ToolHandleArray( const std::initializer_list< ToolHandle< T > >& l );
   /// Constructor with a string initialiser list
   ToolHandleArray( const std::initializer_list< std::string >& l );
   /// Constructor declaring a property
   template<typename Parent>
   ToolHandleArray( Parent *parent, const std::string& name, const std::initializer_list< std::string >& l, const std::string& description = "" );

   /// Retrieve all tools in the array
   StatusCode retrieve() const;

}; // class ToolHandleArray

// Include the implementation of the code:
#include "AsgTools/ToolHandleArray.icc"

#endif // not XAOD_STANDALONE
#endif // ASGTOOLS_TOOLHANDLEARRAY_H
