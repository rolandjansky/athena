// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/excepts.h
 * @author David Quarrie <David.Quarrie@cern.ch>
 * @date Mar 2010
 * @brief Declarations of feenableexcept()/fedisableexcept() functions for MacOSX.
 */


#ifndef CXXUTILS_EXCEPTS_H
#define CXXUTILS_EXCEPTS_H

#ifdef __APPLE__
int
feenableexcept (unsigned int excepts);
int
fedisableexcept (unsigned int excepts);
#endif

#endif // not CXXUTILS_EXCEPTS_H
