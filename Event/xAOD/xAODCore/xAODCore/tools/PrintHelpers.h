// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PrintHelpers.h 646509 2015-02-12 16:43:18Z krasznaa $
#ifndef XAODCORE_TOOLS_PRINTHELPERS_H
#define XAODCORE_TOOLS_PRINTHELPERS_H

// System include(s):
#include <iosfwd>

// Forward declaration(s):
namespace SG {
   class AuxElement;
}

/// Operator printing the payload of an xAOD object
///
/// This function can be used to quickly get an idea about the payload held
/// by any type of xAOD object. It prints the type and name of each auxiliary
/// variable associated with the object. And for some simple types (primitives
/// and some vectors of primitives) it even prints the values of these
/// variables.
///
/// @param out The stream to write the information to
/// @param obj The object to write the information about
/// @returns The same stream object that was given to the function
///
std::ostream& operator<< ( std::ostream& out, const SG::AuxElement& obj );

namespace xAOD {

   /// Helper function for dumping xAOD objects on the screen in PyROOT
   ///
   /// Regular C++ code should just use the operator declared in this header.
   /// But in interactive ROOT and PyROOT this simple function, which always
   /// prints the information to std::cout, should come in handy.
   ///
   /// @param obj The object to dump information about to std::cout
   ///
   void dump( const SG::AuxElement& obj );

} // namespace xAOD

#endif // XAODCORE_TOOLS_PRINTHELPERS_H
