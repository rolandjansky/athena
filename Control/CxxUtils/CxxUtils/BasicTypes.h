// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/BasicTypes.h
 * @author David Quarrie <David.Quarrie@cern.ch>
 * @date Jan 2010, from earlier code.
 * @brief Provide simplified clock_gettime() function for MacOSX.
 */


#ifndef CXXUTILS_BASICTYPES_H
#define CXXUTILS_BASICTYPES_H

#ifdef __APPLE__
typedef unsigned int uint;
#endif

#endif // not CXXUTILS_BASICTYPES_H
