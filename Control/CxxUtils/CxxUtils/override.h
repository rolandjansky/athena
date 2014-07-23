// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/override.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2014
 * @brief Macro to mark a method as overriding one from the base class.
 *
 * Use like:
 *
 *   virtual void foo() ATH_OVERRIDE;
 */


#ifndef CXXUTILS_OVERRIDE_H
#define CXXUTILS_OVERRIDE_H


#ifndef HAVE_ATH_OVERRIDE
# if __cplusplus > 201100
#  define HAVE_ATH_OVERRIDE 1
# else
#  define HAVE_ATH_OVERRIDE 0
# endif
#endif

#if HAVE_ATH_OVERRIDE
# define ATH_OVERRIDE override
#else
# define ATH_OVERRIDE
#endif


#endif // not CXXUTILS_OVERRIDE_H
