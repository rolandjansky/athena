// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef HTTPATTERN_H
#define HTTPATTERN_H

/*
 * HTTPattern.h: This file declares classes to represent patterns.
 * Note that the basic object is pattern_t, a typedef in HTTUtils/HTTTypes.h.
 *
 * Declarations in this file:
 *      struct HTTPattern
 *      class HTTDCPattern
 *
 *
 * Author: Riley Xu
 * Email: rixu@cern.ch
 * Creation Date: 2019-05-29
 */


#include <cstddef>
#include <vector>
#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTUtils/HTTMacros.h"


/*
 * HTTPattern
 *
 * This struct extends pattern_t with metadata: coverage, sector
 */
struct HTTPattern
{
    sector_t sector;
    size_t coverage;
    pattern_t ssids;

    inline size_t nWildcards() const
    {
        size_t n = 0;
        for (auto ssid : ssids) if (ssid == SSID_WILDCARD) n++;
        return n;
    }

    layer_bitmask_t wcLayers() const
    {
        layer_bitmask_t bm = 0;
        for (size_t layer = 0; layer < ssids.size(); layer++)
            if (ssids[layer] == SSID_WILDCARD) bm |= 1 << layer;
        return bm;
    }
};


/*
 * HTTDCPattern
 *
 * Represents a pattern containing don't care bits. For example:
 *      pattern[0] = 0101b = 5
 *      dcBits[0]  = 0011b
 * encodes ssids:
 *      0100 = 4
 *      0101 = 5
 *      0110 = 6
 *      0111 = 7
 */
class HTTDCPattern
{
    public:

        // Constructors
        HTTDCPattern() {}
        HTTDCPattern(size_t nLayers) { setNLayers(nLayers); }
        HTTDCPattern(pattern_t const & pattern, std::vector<ssid_t> const & dcBits, size_t coverage = 0)
            : m_pattern(pattern), m_dcBits(dcBits), m_coverage(coverage) { setNLayers(m_pattern.size()); }

        // Getters
        inline size_t getNLayers() const { return m_pattern.size(); }
        inline ssid_t getSSID(size_t layer) const { return m_pattern[layer]; }
        inline ssid_t getDCBits(size_t layer) const { return m_dcBits[layer]; }
        pattern_t const & getSSIDs() const { return m_pattern; }
        pattern_t const & getDCBits() const { return m_dcBits; }
        inline size_t getCoverage() const { return m_coverage; }

        std::vector<ssid_t> const & getMatchingSSIDs(size_t layer) const;
            // This function applies the dcBits in layer and returns all SSIDs that match this pattern.
            // Use getSSID(layer) to get the underlying, stored ssid, and getSSIDs(void) to get a list
            // of all of the underlying ssids.

        // Setters
        void setNLayers(size_t n);
        inline void setSSID(size_t layer, ssid_t ssid)
            { m_pattern[layer] = ssid; m_stale[layer] = true; }
        inline void setPattern(pattern_t const & pattern)
            { m_pattern = pattern; m_stale.clear(); setNLayers(m_pattern.size()); }
        inline void setDCBits(size_t layer, ssid_t dcBits)
            { m_dcBits[layer] = dcBits; m_stale[layer] = true; }
        inline void setDCBits(std::vector<ssid_t> const & dcBits)
            { m_dcBits = dcBits; m_stale.clear(); setNLayers(m_pattern.size()); }
        inline void setCoverage(size_t coverage) { m_coverage = coverage; }

        // Other
        inline bool match(size_t layer, ssid_t ssid) const // returns true if the ssid matches the pattern, with DC bits
            { return ((m_pattern[layer] | m_dcBits[layer]) ^ (ssid | m_dcBits[layer])) == 0; }

    private:

        // Data
        pattern_t m_pattern; // TSP ssids
        pattern_t m_dcBits; // bitmask for each layer, with 1 = dc bit. type pattern_t to match bitwidth of ssids.
        size_t m_coverage = 0;

        // Cache
        mutable std::vector<bool> m_stale; // must recalculate cache
        mutable std::vector<std::vector<ssid_t>> m_cachedSSIDs;
            // cache the TSP ssids from getSSIDs() to not have to recalculate everytime
};




#endif // HTTPATTERN_H
