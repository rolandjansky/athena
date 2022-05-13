// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTRegionMap.h
 * @author Riley Xu - riley.xu@cern.ch (rewrite from FTK)
 * @date Janurary 7th, 2020
 * @brief Maps ITK module indices to HTT regions.
 *
 * The region map is a text configuration file that defines the regions in HTT.
 * A region is roughly an eta/phi slice that handled by the same TP board (??? not quite sure if this is right).
 * The region map delineates the modules in each region based on InDetIdentifier ids.
 *
 * Region map format:
 *      1. The first line should read
 *              towers <NTOWERS> phi <NPHI>
 *         where NTOWERS is the number of regions, and NPHI is ???. TODO rename tower to region
 *      2. A list of the region definitions with the below format, seperated by empty lines, in order.
 *
 * Region definition format:
 *      1. A line with the region number.
 *      2. A line for each detector layer (see HTTPlaneMap), which is a space-separated list
 *         with the below values.
 *
 * Line values (i.e. columns):
 *      1. isPixel (1/0)                        |
 *      2. isEndcap (1/0)                       } -- These identify the detector layer
 *      3. physical layer (see HTTPlaneMap)     |
 *      4. minimum phi id   ] -- in this detector layer in this region
 *      5. maximum phi id   ]
 *      6. total phi modules  -- in this detector layer among all regions
 *      7. minimum eta id
 *      8. maximum eta id
 *      9. total eta modules
 *
 *  The min and max ids are the important fields that actually define the boundary
 *  of the region, and are inclusive.
 *  TODO there is no reason to store total phi/eta module in each region.
 */

#ifndef HTTREGIONMAP_H
#define HTTREGIONMAP_H

#include "TrigHTTMaps/HTTPlaneMap.h"

#include <vector>
#include <map>
#include <fstream>

class HTTHit;


struct HTTRegionBoundaries
{
    int phi_min; // minimum phi ID
    int phi_max; // maximum phi ID
    int eta_min; // minimum eta ID
    int eta_max; // minimum eta ID
};


class HTTRegionMap 
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // Constructors/Initialization

        HTTRegionMap(HTTPlaneMap const * pmap, std::string const & filepath);

        void loadModuleIDLUT(std::string const & filepath);

        ///////////////////////////////////////////////////////////////////////
        // Simple Getters/Setters

        const HTTPlaneMap* getPlaneMap() const { return m_pmap; }

        int getNRegions() const { return m_nregions; }

        bool isModuleIDLUTloaded() const { return !m_global_local_map.empty(); }

        ///////////////////////////////////////////////////////////////////////
        // Main Interface Functions

        const HTTRegionBoundaries & getRegionBoundaries(int region, int layer, int section) const
            { return m_map.at(region).at(layer).at(section); }

        bool isInRegion(uint32_t region, const HTTHit &hit) const;
        bool isInRegion(uint32_t region, uint32_t plane, uint32_t section, int eta, int phi) const;

        // Returns a list of regions that hit belongs to
        std::vector<uint32_t> getRegions(const HTTHit &hit) const;

        uint32_t getUnmappedID(uint32_t region, const HTTHit &hit) const;

        // These require the module LUT file to be loaded
        uint32_t getLocalID(uint32_t region, uint32_t layer, uint32_t globalModuleID) const;
        uint32_t getGlobalID(uint32_t region, uint32_t layer, uint32_t localModuleID) const;

    private:

        const HTTPlaneMap *m_pmap = nullptr;
        int m_nregions = 0;

        std::vector<std::vector<std::vector<HTTRegionBoundaries>>> m_map;
            // Index by region, logical layer, section

        std::vector<std::vector<std::map<uint32_t, uint32_t>>> m_global_local_map;
            // Index by region, logical layer, globalID. Returns a local id.

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        void allocateMap(std::ifstream & fin);
        void readRegion(std::ifstream & fin, int expected_region);
};

#endif // HTTREGIONMAP_H
