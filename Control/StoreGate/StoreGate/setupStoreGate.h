// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/setupStoreGate.h
 * @author scott snyder <snyder@bnl.gov>, after S. Binet's code from TestPolicy.
 * @date Aug, 2014
 * @brief Helper for setting up StoreGate for regression tests.
 */


#ifndef STOREGATESVC_SETUPSTOREGATE_H
#define STOREGATESVC_SETUPSTOREGATE_H


#include <string>


namespace Athena_test {


/**
 * @brief Helper for setting up StoreGate for regression tests.
 * @param progname Program name (argv[0]).
 * @param options_file Name of the JO text file to read.
 *                     If defaulted, one will be generated.
 */
bool setupStoreGate (std::string progname, 
                     std::string options_file = "");


} // namespace Athena_test


#endif // not STOREGATESVC_SETUPSTOREGATE_H
