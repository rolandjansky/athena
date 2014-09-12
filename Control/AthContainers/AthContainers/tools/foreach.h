// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/foreach.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief foreach helper for standalone build.
 *
 * Define @c ATHCONTAINERS_FOREACH to use either @c BOOST_FOREACH
 * for C++ directly, depending on whether or not we're using c++11.
 */


#ifndef ATHCONTAINERS_FOREACH_H
#define ATHCONTAINERS_FOREACH_H


#if __cplusplus < 201100
# include "boost/foreach.hpp"
# define ATHCONTAINERS_FOREACH(A, B) BOOST_FOREACH(A, B)
#else
# define ATHCONTAINERS_FOREACH(A, B) for (A : B)
#endif


#endif // not ATHCONTAINERS_FOREACH_H
