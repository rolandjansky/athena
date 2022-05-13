// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTSectorBank.cxx
 * @author Riley Xu - rixu@cern.ch
 * @date April 2nd, 2020
 * @brief This file declares a class that stores the module IDs of the sectors.
 */


#include "TrigHTTBanks/HTTSectorBank.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTypes.h"

#include <AsgMessaging/MessageCheck.h>
#include <set>
#include <cassert>
#include <unordered_map>
#include <algorithm>

using namespace asg::msgUserCode;

HTTSectorBank::HTTSectorBank(std::string const & filepath) 
{
    // Open the file for reading
  ANA_MSG_INFO("Reading " << filepath);
    std::ifstream fin(filepath);
    if (!fin.is_open())
    {
      ANA_MSG_ERROR("Couldn't open " << filepath);
      throw ("HTTSectorBank couldn't open " + filepath);
    }
    
    // Retrieve the number of sectors and layers, resize m_s2mMap
    readHeader(fin);

    // Read the modules from each sector
    readSectors(fin);

    // Make m2sMap
    makeInverseMap();
}


// Retrieve the number of sectors and layers, resize m_s2mMap
void HTTSectorBank::readHeader(std::ifstream & fin)
{
    bool ok = true;
    std::string line;
    int nSectors;

    ok = ok && std::getline(fin, line);
    ANA_MSG_DEBUG(line);

    std::istringstream sline(line);
    ok = ok && (sline >> nSectors >> m_nLayers);

    if (!ok) throw "Error reading header";

    m_s2mMap.resize(nSectors, std::vector<module_t>(m_nLayers));
}


void HTTSectorBank::readSectors(std::ifstream & fin)
{
    std::string line;
    bool ok = true;
    int sector = 0; // make sure sectors are numbered in order
    int sector_file, UNUSED, coverage;

    while (std::getline(fin, line))
    {
        if (line.empty() || line[0] == '!') continue;
        ANA_MSG_DEBUG(line);
        std::istringstream sline(line);

        ok = ok && (sline >> sector_file);
        ok = ok && (sector_file == sector);

        for (unsigned i = 0; i < m_nLayers; i++)
            ok = ok && (sline >> m_s2mMap[sector][i]);

        ok = ok && (sline >> UNUSED >> coverage); // both these are not stored

        if (!ok) break;
        sector++;
    }

    if (!ok) ANA_MSG_ERROR("Error reading file on line: " << line);
}



// Inverts m_s2mMap, a list of module ids for each sector, into a mapping
// from module ids to sectors, m_m2sMap.
void HTTSectorBank::makeInverseMap()
{
    m_m2sMap.resize(m_nLayers);
    for (size_t layer = 0; layer < m_nLayers; layer++)
    {
        for (sector_t sector = 0; (size_t)sector < m_s2mMap.size(); sector++)
        {
            module_t moduleID = m_s2mMap[sector][layer];
            m_m2sMap[layer].insert(std::make_pair(moduleID, sector));
        }
    }
}


std::vector<sector_t> HTTSectorBank::getSectors(unsigned layer, module_t module) const
{
    std::vector<sector_t> out;
    auto range = m_m2sMap[layer].equal_range(module);
    for (auto it_m2s = range.first; it_m2s != range.second; it_m2s++)
        out.push_back(it_m2s->second);

    return out;
}


sector_t HTTSectorBank::findSector(std::vector<module_t> const & modules) const
{
    assert(modules.size() == m_nLayers);

    std::set<sector_t> sectors_good; // List of matching sectors so far.
    for (size_t layer = 0; layer < m_nLayers; layer++)
    {
        std::set<sector_t> sectors_new;
        auto range = m_m2sMap[layer].equal_range(modules[layer]);

        for (auto it_m2s = range.first; it_m2s != range.second; it_m2s++)
            if (layer == 0 || sectors_good.count(it_m2s->second))
                sectors_new.insert(it_m2s->second);

        if (sectors_new.empty()) return -1;
        sectors_good = sectors_new;
    }
    
    assert(sectors_good.size() == 1);
    return *(sectors_good.begin());
}


// Returns the sector matching 'hits' exactly (WC must be matched with WC), or -1 if none.
// If multiple hits have different hashIDs, will prefer most popular, then random.
sector_t HTTSectorBank::findSector(std::vector<std::vector<const HTTHit*>> const & hits) const
{
    std::vector<module_t> modules(hits.size());

    for (size_t i = 0; i < hits.size(); i++)
    {
        if (hits[i].empty())
        {
            modules[i] = MODULE_BADMODULE;
        }
        else if (hits[i].size() == 1)
        {
            modules[i] = hits[i][0]->getHTTIdentifierHash();
	}
        else
        {
            std::unordered_map<unsigned, unsigned> hashCount;
            for (const HTTHit* h : hits[i]) hashCount[h->getHTTIdentifierHash()]++;
            modules[i] = std::max_element(hashCount.begin(), hashCount.end())->first;
        }
    }

    return findSector(modules);
}
