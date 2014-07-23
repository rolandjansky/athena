// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/final.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2012
 * @brief Macro to mark a method as final.
 *
 * Use like:
 *
 *   virtual void foo() ATH_FINAL;
 */


#ifndef CXXUTILS_FINAL_H
#define CXXUTILS_FINAL_H


#ifndef HAVE_ATH_FINAL
# if __cplusplus > 201100
#  define HAVE_ATH_FINAL 1
# else
#  define HAVE_ATH_FINAL 0
# endif
#endif

#if HAVE_ATH_FINAL
# define ATH_FINAL final
#else
# define ATH_FINAL
#endif


#endif // not CXXUTILS_FINAL_H
