/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* $Id$ */
/**
 * @file CxxUtils/unused.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Macro to mark a member as unused.
 *
 * Use like:
 *
 *   int ATH_UNUSED_MEMBER(m_v);
 *
 * Of course, if a member is really unused, it's better to simply
 * remove it rather than trying to paper over the warning like this.
 *
 * For non-members, just use [[maybe_unused]] (except for parameters,
 * which should usually just be left unnamed).
 */

#ifndef CXXUTILS_UNUSED_H
#define CXXUTILS_UNUSED_H


// Declare that a member is unused, to suppress warnings.
// Only clang recognizes an unused attribute for a member.
#ifdef __clang__
# define ATH_UNUSED_MEMBER(v) v __attribute__((unused))
#else
# define ATH_UNUSED_MEMBER(v) v
#endif


#endif /* not CXXUTILS_UNUSED_H */
