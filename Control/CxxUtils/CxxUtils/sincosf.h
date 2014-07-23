// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/sincosf.h
 * @author David Quarrie <David.Quarrie@cern.ch>
 * @date Jan 2010
 * @brief Provide sincosf function for MacOSX.
 */


#ifndef CXXUTILS_SINCOSF_H
#define CXXUTILS_SINCOSF_H

#ifdef __APPLE__
void sincosf(float x, float* sn, float* cs);
#endif

#endif // not CXXUTILS_SINCOSF_H
