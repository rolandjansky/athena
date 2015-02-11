/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 macro to assert an error condition
 ----------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: SGassert.h,v 1.2 2005-11-29 00:51:33 calaf Exp $


#ifndef TEST_SGASSERT_H
# define TEST_SGASSERT_H

#include <cassert>
#include <iostream>

#undef NDEBUG

#define SGASSERT( TRUEEXPR ) assert(TRUEEXPR)
#define SGASSERTERROR( FALSEEXPR )   \
    std::cerr << "Now we expect to see an error message:" << std::endl \
              << "----Error Message Starts--->>" << std::endl; \
    assert(!(FALSEEXPR));                                      \
    std::cerr<< "<<---Error Message Ends-------" << std::endl


#endif // TEST_SGASSERT_H
