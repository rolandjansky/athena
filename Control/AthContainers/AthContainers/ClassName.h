// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ClassName.h 581165 2014-02-03 10:42:54Z krasznaa $
#ifndef ATHCONTAINERS_CLASSNAME_H
#define ATHCONTAINERS_CLASSNAME_H

#ifndef XAOD_STANDALONE

// Get the code from AthenaKernel:
#include "AthenaKernel/ClassName.h"

#else

// STL include(s):
#include <string>

/**
 *  @short A copy of the ClassName class for standalone compilation
 *
 *         Since the standalone compilation doesn't use SGTools, but
 *         the standalone code also needs to be able to get the names
 *         of DataVector types in a nicely formatted way, we have this
 *         copy of the SGTools class in here for standalone compilation.
 *
 * @author Scott Snyder         <Scott.Snyder@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 581165 $
 * $Date: 2014-02-03 11:42:54 +0100 (Mon, 03 Feb 2014) $
 */
template< class T >
class ClassName {

public:
   /// Returns the name of class @c T as a string
   static std::string name();

}; // class ClassName

// Include the implementation:
#include "ClassName.icc"

#endif // not XAOD_STANDALONE

#endif // not ATHCONTAINERS_CLASSNAME_H
