/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
 * HTTPattern.cxx: See HTTPattern.h
 *
 * Author: Riley Xu
 * Email: rixu@cern.ch
 * Creation Date: 2019-06-21
 */

#include "TrigHTTObjects/HTTPattern.h"
#include "TrigHTTUtils/HTTFunctions.h"

using std::vector;

void HTTDCPattern::setNLayers(size_t n)
{
    m_pattern.resize(n);
    m_dcBits.resize(n);
    m_stale.resize(n, true);
    m_cachedSSIDs.resize(n);
}


vector<ssid_t> const & HTTDCPattern::getMatchingSSIDs(size_t layer) const
{
    if (!m_stale[layer]) return m_cachedSSIDs[layer];

    m_cachedSSIDs[layer].clear();

    ssid_t base = m_pattern[layer] & ~m_dcBits[layer]; // base is the ssid with the dc bits zeroed
    vector<uint32_t> dcBits = splitSetBits32(m_dcBits[layer]);

    // Loop over combinations of dc bits. Note bitcomb serves as a bit mask
    // for which dc bits are set. For example, bitcomb=01b means set only the first dc bit.
    for (unsigned bitcomb = 0; bitcomb < (unsigned)(1 << dcBits.size()); bitcomb++) // 2^(N bits)
    {
        ssid_t new_ssid = base;
        // loop over bits to set
        for (unsigned ibit = 0; ibit < dcBits.size(); ibit++)
        {
            if ( bitcomb & (1 << ibit) ) // set this bit
                new_ssid = new_ssid | dcBits[ibit];
        }
        m_cachedSSIDs[layer].push_back(new_ssid);
    }

    m_stale[layer] = false;
    return m_cachedSSIDs[layer];
}
