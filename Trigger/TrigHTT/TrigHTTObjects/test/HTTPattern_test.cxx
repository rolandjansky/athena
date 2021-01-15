/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
 * HTTPattern_test.cxx: Unit tests for HTTPattern
 *
 * Author: Riley Xu
 * Email: rixu@cern.ch
 * Creation Date: 2019-06-21
 */

#include <cstdio>
#include <iostream>
#include <cassert>

#include "TrigHTTObjects/HTTPattern.h"

using namespace std;

void test_dc()
{
    HTTDCPattern patt(1);
    patt.setSSID(0, 0x92);
    patt.setDCBits(0, 0x11);
    vector<ssid_t> ssid_exp = {0x82, 0x83, 0x92, 0x93};
    assert(ssid_exp == patt.getMatchingSSIDs(0));
}

int main(int, char**)
{
    try
    {
        test_dc();
    }
    catch (std::runtime_error const & e)
    {
        cout << "TrigHTTPatternTest failed: " << e.what() << endl;
        return -1;
    }
    return 0;
}
