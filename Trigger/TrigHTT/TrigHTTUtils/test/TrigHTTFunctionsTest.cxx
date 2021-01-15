/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
 * TrigHTTFunctionsTest.cxx: Unit tests for HTTFunctions
 *
 * Author: Riley Xu
 * Email: rixu@cern.ch
 * Creation Date: 2019-07-01
 */


#include <string>
#include <iostream>

#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTUtils/HTTFunctions.h"

using std::cout;
using std::vector;

void test_splitSetBits32()
{
    vector<uint32_t> exp;

    exp = {};
    HTT_ASSERT_THROW(splitSetBits32(0x0) == exp);

    exp = {0x1};
    HTT_ASSERT_THROW(splitSetBits32(0x1) == exp);

    exp = {0x2, 0x8};
    HTT_ASSERT_THROW(splitSetBits32(0xA) == exp);

    exp = {0x1, 0x2, 0x4, 0x8};
    HTT_ASSERT_THROW(splitSetBits32(0xF) == exp);

    exp = {0x10};
    HTT_ASSERT_THROW(splitSetBits32(0x10) == exp);

    exp = {0x1, 0x4000};
    HTT_ASSERT_THROW(splitSetBits32(0x4001) == exp);
}

int main(int, char**)
{
    try {
        test_splitSetBits32();
    } catch (std::runtime_error const & e) {
        cout << "TrigHTTFunctionsTest failed: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
