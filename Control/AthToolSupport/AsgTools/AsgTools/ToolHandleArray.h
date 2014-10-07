// Dear emacs, this is -*- c++ -*-
// $Id: ToolHandleArray.h 613788 2014-08-28 09:22:42Z krasznaa $
#ifndef ASGTOOLS_TOOLHANDLEARRAY_H
#define ASGTOOLS_TOOLHANDLEARRAY_H

// Local include(s):
#include "AsgTools/AsgToolsConf.h"

#ifdef ASGTOOL_ATHENA
#   include "GaudiKernel/ToolHandle.h"
#elif defined(ASGTOOL_STANDALONE)

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
#if __cplusplus >= 201100
   /// Constructor with an initialiser list
   ToolHandleArray( const std::initializer_list< ToolHandle< T > >& l );
   /// Constructor with a string initialiser list
   ToolHandleArray( const std::initializer_list< std::string >& l );
#endif // C++11

   /// Retrieve all tools in the array
   StatusCode retrieve() const;

}; // class ToolHandleArray

// Include the implementation of the code:
#include "AsgTools/ToolHandleArray.icc"

#endif // Environment selection
#endif // ASGTOOLS_TOOLHANDLEARRAY_H
