/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
 * HTT_SSID_test.cxx: Unit tests for HTT_SSID
 *
 * Author: Riley Xu
 * Email: riley.xu@cern.ch
 * Creation Date: 2019-12-18
 */

#include <iostream>
#include <cassert>

#include "TrigHTTObjects/HTT_SSID.h"

using namespace std;

void test_sizes()
{
    assert(sizeof(HTT_SSID) == sizeof(uint32_t));
    assert(NBITS_MODULE > 0 && NBITS_MODULE < NBITS_SSID); // check overflow
    assert(NBITS_PHI > 0 && NBITS_PHI < NBITS_SSID);
}

void test_mirror()
{
    HTT_SSID s;
    s.setStripX(0xA);
    s.setModule(0xFF);

    assert(s.getStripX() == 0xA);
    assert(s.getModule() == 0xFF);

    s.setPixelX(0x2);
    s.setPixelY(0x1);
    assert(s.getPixelX() == 0x2);
    assert(s.getPixelY() == 0x1);
    assert(s.getModule() == 0xFF);
}

// These tests need to change if the bit widths change
void test_union()
{
    HTT_SSID s;
    s.setStripX(0xB); // 0b001110 Gray-coded
    s.setModule(0x39C);

    assert(s.getSSID() == 0xE70E);
    assert(s.getPixelX() == 0x4); // 0b110 decoded
    assert(s.getPixelY() == 0x1); // 0b001 decoded

    s.setPixelX(0x1); // 0b001 Gray-coded
    s.setPixelY(0x3); // 0b010 Gray-coded
    s.setModule(0xB9C);

    assert(s.getStripX() == 0x1E); // 0b010001 decoded
    assert(s.getSSID() == 0x2E71E);
    assert(s.getHWSSID() == 0xE71E);
}

void test_constructor()
{
    HTT_SSID s(0x2E71E);
    assert(s.getPixelX() == 0x1);
    assert(s.getPixelY() == 0x3);
    assert(s.getModule() == 0xB9C);
}

int main(int, char**)
{
    try
    {
        test_sizes();
        test_mirror();
        test_union();
        test_constructor();
    }
    catch (std::runtime_error const & e)
    {
        cout << "HTT_SSID_test failed: " << e.what() << endl;
        return -1;
    }
    return 0;
}
