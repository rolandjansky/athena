// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTMATRIXREADER_H
#define HTTMATRIXREADER_H

/**
 * @file HTTMatrixIO.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date May 29th, 2020
 * @brief Classes to read/write matrix files event by event.
 *
 * See below for batch read and write functions.
 */

#include "HTTMatrixAccumulator.h"
#include "TrigHTTObjects/HTTTypes.h"

#include <vector>

#include "TTree.h"


///////////////////////////////////////////////////////////////////////////////
// Reader
///////////////////////////////////////////////////////////////////////////////


/*
 * Use nextEntry() or readEntry() to read the entry in the tree,
 * then use getModules() or getAccumulator() to get a reference to the data.
 *
 * Example:
 *      HTTMatrixReader reader(tree, nLayers, nCoords);
 *      while (reader.nextEntry())
 *      {
 *          std::vector<module_t> & modules = reader.getModules();
 *          HTTMatrixAccumulator & acc = reader.getAccumulator();
 *      }
 */
class HTTMatrixReader
{
    public:

        HTTMatrixReader(TTree *tree, size_t nLayers, size_t nCoords);
        ~HTTMatrixReader() { m_tree->ResetBranchAddresses(); }

        void setEntry(size_t entry) { m_entry = entry; }
        size_t getEntry() const { return m_entry - 1; } // last read entry number

        // Read the next entry. Returns true on success.
        bool nextEntry();
        // Read a specific entry.
        void readEntry(size_t entry);

        std::vector<module_t> & getModules() { return m_modules; }
        HTTMatrixAccumulator & getAccumulator() { return m_acc; }

    private:

        TTree* m_tree;

        size_t m_entry;
        size_t m_nEntries;

        std::vector<module_t> m_modules;
        HTTMatrixAccumulator m_acc;

        std::vector<short> m_bins_QoP;
        std::vector<short> m_bins_phi;
        std::vector<short> m_bins_d0;
        std::vector<short> m_bins_z0;
        std::vector<short> m_bins_eta;

        // These point to the variables above because ROOT expects a vector**.
        std::vector<short>* m_pQoP;
        std::vector<short>* m_pphi;
        std::vector<short>* m_pd0;
        std::vector<short>* m_pz0;
        std::vector<short>* m_peta;
};

///////////////////////////////////////////////////////////////////////////////
// Writer
///////////////////////////////////////////////////////////////////////////////

class HTTMatrixWriter
{
    public:

        HTTMatrixWriter(TTree *tree, int nLayers, int nCoords);

        // Returns entry filled
        size_t fill(std::vector<module_t> modules, HTTMatrixAccumulator & acc); // copying, necessary since root writing is not const but map keys are

    private:

        TTree* m_tree;

        size_t m_nEntries;
        int m_nLayers;
        int m_nCoords;
        int m_nCoords2;
        float m_coverage = 0.0F;

        std::vector<short> m_bins_QoP;
        std::vector<short> m_bins_phi;
        std::vector<short> m_bins_d0;
        std::vector<short> m_bins_z0;
        std::vector<short> m_bins_eta;

        // These point to the variables above because ROOT expects a vector**.
        std::vector<short>* m_pQoP;
        std::vector<short>* m_pphi;
        std::vector<short>* m_pd0;
        std::vector<short>* m_pz0;
        std::vector<short>* m_peta;
};


///////////////////////////////////////////////////////////////////////////////
// Utility functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Reads a matrix tree, accumulating its entries into a map.
 * Note this functions sets and resets branch addresses.
 *
 * @param map - map to be added to
 * @param tree - tree to be read
 * @param nLayers - number of logical layers
 * @param nCoords - number of hit coordinates in a full (13/13) track
 */
void readTree(AccumulateMap & map, TTree *tree, size_t nLayers, size_t nCoords);


/**
 * Reads a matrix tree, accumulating its entries into a vector, in the order they
 * are read. Note this functions sets and resets branch addresses.
 *
 * @param tree - tree to be read
 * @param nLayers - number of logical layers
 * @param nCoords - number of hit coordinates in a full (13/13) track
 */
std::vector<std::pair<std::vector<module_t>, HTTMatrixAccumulator>> readTree(TTree *tree, size_t nLayers, size_t nCoords);


/**
 * Writes the contents of an AccumulateMap into the supplied tree (one entry per sector).
 *
 * @param begin - iterator to a std::pair<std::vector<module_t>, HTTMatrixAccumulator>> to begin filling
 * @param end - end iterator to stop filling
 * @param tree - tree to be written to; this function will create the branches
 * @param nLayers - number of logical layers
 * @param nCoords - number of hit coordinates in a full (13/13) track
 */
template<typename Iter>
void fillTree(Iter begin, Iter end, TTree *tree, int nLayers, int nCoords)
{
    // Sort sectors by decreasing coverage
    std::vector<Iter> sorted_sectors;
    sorted_sectors.reserve(std::distance(begin, end));
    for (Iter i = begin; i != end; i++) sorted_sectors.push_back(i);

    std::sort(sorted_sectors.begin(), sorted_sectors.end(), [](Iter const & a, Iter const & b)
        { return a->second.track_bins.size() > b->second.track_bins.size(); }
    );

    // Create the writer
    HTTMatrixWriter writer(tree, nLayers, nCoords);

    // Fill the tree
    for (Iter & sector_info : sorted_sectors)
        writer.fill(sector_info->first, sector_info->second);
}


// Specialization to AccumulateMap
void fillTree(AccumulateMap & map, TTree *tree, int nLayers, int nCoords);



#endif
