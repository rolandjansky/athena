// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTRegionMap.h
 * @author Riley Xu - riley.xu@cern.ch (rewrite from FTK)
 * @date Janurary 7th, 2020
 * @brief Maps ITK module indices to HTT regions.
 *
 * See header.
 */

#include "TrigHTTMaps/HTTRegionMap.h"
#include <AsgMessaging/MessageCheck.h>

#include <cstdlib>
#include <string>
#include <iostream>
#include <boost/foreach.hpp>
#include <vector>

using namespace std;
using namespace asg::msgUserCode;


///////////////////////////////////////////////////////////////////////////////
// Constructor/Desctructor
///////////////////////////////////////////////////////////////////////////////


HTTRegionMap::HTTRegionMap(const HTTPlaneMap *pmap, std::string const & filepath) :
    m_pmap(pmap)
{
    // Open the file
    ANA_MSG_INFO("Reading " << filepath);
    ifstream fin(filepath);
    if (!fin.is_open())
    {
        ANA_MSG_FATAL("Couldn't open " << filepath);
        throw ("HTTRegionMap Couldn't open " + filepath);
    }

    // Reads the header of the file to resize all the vector members
    allocateMap(fin);

    // Read all the region data
    for (int region = 0; region < m_nregions; region++) readRegion(fin, region);
}


// Reads the header of the file to resize all the vector members
void HTTRegionMap::allocateMap(ifstream & fin)
{
    string line, towerKey;
    bool ok = true;

    ok = ok && getline(fin, line);
    ANA_MSG_DEBUG(line);

    istringstream sline(line);
    ok = ok && (sline >> towerKey >> m_nregions);
    ok = ok && (towerKey == "towers");

    if (!ok) ANA_MSG_FATAL("Error reading header");

    m_map.resize(m_nregions);
    for (auto & vv : m_map)
    {
        vv.resize(m_pmap->getNLogiLayers());
        for (size_t l = 0; l < vv.size(); l++) vv[l].resize(m_pmap->getNSections(l));
    }
}


// Reads one region from file.
void HTTRegionMap::readRegion(ifstream & fin, int expected_region)
{
    string line, dummy;
    bool ok = true;
    int region = -1;
    uint32_t linesRead = 0; // detLayer lines read

    while (getline(fin, line))
    {
        if (line.empty() || line[0] == '#') continue;
        istringstream sline(line);

        if (region < 0) // Find the starting header of the next region
        {
            ok = ok && (sline >> region);
            ok = ok && !(sline >> dummy); // No keyword to check that we're not reading a detector line, so make sure rest of string is empty
            ok = ok && (region == expected_region);
            if (!ok) break;
        }
        else // Detector layer line
        {
            int isPix, BEC, physLayer, phi_min, phi_max, phi_tot, eta_min, eta_max, eta_tot;
            ok = ok && (sline >> isPix >> BEC >> physLayer >> phi_min >> phi_max >> phi_tot >> eta_min >> eta_max >> eta_tot);
            if (!ok) break;

            int logiLayer = m_pmap->getLayerSection(static_cast<SiliconTech>(isPix), static_cast<DetectorZone>(BEC), physLayer).layer;
            int section   = m_pmap->getLayerSection(static_cast<SiliconTech>(isPix), static_cast<DetectorZone>(BEC), physLayer).section;

            if (logiLayer > -1)
                m_map[region][logiLayer][section] = { phi_min, phi_max, eta_min, eta_max };

            if (++linesRead == m_pmap->getNDetLayers()) break;
        }
    }

    if (!ok)
    {
        ANA_MSG_FATAL("Found error reading file at line: " << line);
        throw "HTTRegionMap read error";
    }
}


// Read module id LUT (defining global -> tower-local module IDs)
void HTTRegionMap::loadModuleIDLUT(std::string const & filepath)
{
    ANA_MSG_INFO("Reading module LUT" << filepath);
    ifstream fin(filepath);
    if (!fin.is_open())
    {
        ANA_MSG_ERROR("Couldn't open " << filepath);
        throw ("HTTRegionMap Couldn't open " + filepath);
    }

    m_global_local_map.clear();
    m_global_local_map.resize(m_nregions, vector<map<uint32_t, uint32_t>>(m_pmap->getNLogiLayers()));

    string line;
    while (getline(fin, line))
    {
        uint32_t region, layer, globalID, localID;
        istringstream sline(line);

        if (!(sline >> region >> layer >> globalID >> localID))
            ANA_MSG_WARNING("Error reading module LUT");
        else if (region >= m_global_local_map.size() || layer >= m_pmap->getNLogiLayers())
            ANA_MSG_WARNING("loadModuleIDLUT() bad region=" << region << " or layer=" << layer);
        else
            m_global_local_map[region][layer][globalID] = localID;
    }
}



///////////////////////////////////////////////////////////////////////////////
// Interface Functions
///////////////////////////////////////////////////////////////////////////////

bool HTTRegionMap::isInRegion(uint32_t region, const HTTHit &hit) const
{
    return isInRegion(region, hit.getLayer(), hit.getSection(), hit.getHTTEtaModule(), hit.getPhiModule());
}


bool HTTRegionMap::isInRegion(uint32_t region, uint32_t layer, uint32_t section, int eta, int phi) const
{
    if (    region  >= m_map.size()
         || layer   >= m_map[region].size()
         || section >= m_map[region][layer].size() )
    {
        return false;
    }

    int eta_min = m_map[region][layer][section].eta_min;
    int eta_max = m_map[region][layer][section].eta_max;

    if (eta < eta_min || eta > eta_max) return false;

    int phi_min = m_map[region][layer][section].phi_min;
    int phi_max = m_map[region][layer][section].phi_max;

    // Need special cases for phi because it can go from 2pi to 0.
    if (phi_min <= phi_max) // Region does not cross phi = 0
    {
        if (phi < phi_min || phi > phi_max) return false;
    }
    else // Region crosses phi = 0
    {
        if (phi < phi_min && phi > phi_max) return false;
    }

    return true;
}

std::vector<uint32_t> HTTRegionMap::getRegions(const HTTHit &hit) const
{
    std::vector<uint32_t> regions;
    for (uint32_t region = 0; region < m_map.size(); region++)
        if (isInRegion(region, hit))
            regions.push_back(region);
    return regions;
}

uint32_t HTTRegionMap::getUnmappedID(uint32_t region, const HTTHit &hit) const
{
    /*
    Todo: Does this handle EC hits correctly?

    error code key:
    6 digit number. 1 = ok. 2 = not ok

    1st digit - Endcap Check
    2nd - region
    3rd - layer
    4th - section
    5th - eta
    6th - phi
    */

  uint32_t layer   = hit.getLayer();
  uint32_t section = hit.getSection();
  int eta          = hit.getHTTEtaModule();
  int phi          = hit.getPhiModule();

    int err[] = {1,1,1,1,1,1};

    if (region >= m_map.size()) err[1] = 2;

    if (layer >= m_map[region].size()) err[2] = 2;

    if (section >= m_map[region][layer].size()) err[3] = 2;

    int eta_min = m_map[region][layer][section].eta_min;
    int eta_max = m_map[region][layer][section].eta_max;

    if (eta < eta_min) err[4] = 3;
    if (eta > eta_max) err[4] = 2;

    int phi_min = m_map[region][layer][section].phi_min;
    int phi_max = m_map[region][layer][section].phi_max;

    // Need special cases for phi berrause it can go from 2pi to 0.
    if (phi_min <= phi_max) // Region does not cross phi = 0
    {
        if (phi < phi_min || phi > phi_max) err[5] = 2;
    }
    else // Region crosses phi = 0
    {
        if (phi < phi_min && phi > phi_max) err[5] = 3;
    }

    int error_code = 100000*err[0] + 10000*err[1] + 1000*err[2] + 100*err[3] + 10*err[4] + err[5];

    return error_code;
}


uint32_t HTTRegionMap::getLocalID(uint32_t region, uint32_t layer, uint32_t globalModuleID) const
{
    // TEMPORARY UNTIL WE HAVE A MODULE LUT
    (void) region;
    (void) layer;
    return globalModuleID & 0x3ff;
}


uint32_t HTTRegionMap::getGlobalID(uint32_t region, uint32_t layer, uint32_t localModuleID) const
{
    if (region >= m_global_local_map.size() || layer >= m_pmap->getNLogiLayers())
    {
        ANA_MSG_ERROR("getGlobalID() bad region=" << region << " or layer=" << layer);
        return -1;
    }

    for (auto const & g_l : m_global_local_map[region][layer])
        if (g_l.second == localModuleID) return g_l.first;

    ANA_MSG_ERROR("getGlobalID() Did not find global id for region " << region << ", layer " << layer << ", localID " << localModuleID);
    return -1;
}



