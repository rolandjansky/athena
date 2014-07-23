/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/no_conversion_warning.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2009
 * @brief Disable conversion warnings.
 *
 * Include this header first in a compilation unit to disable -Wconversion
 * warnings.
 */


#if __GNUC__ >= 4
# pragma GCC diagnostic ignored "-Wconversion"
#endif
