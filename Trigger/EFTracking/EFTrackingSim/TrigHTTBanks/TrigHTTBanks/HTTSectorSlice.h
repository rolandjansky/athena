// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTSectorSlice_h
#define HTTSectorSlice_h

/**
 * @file HTTSectorSlice.h
 * @author major update Riley Xu - rixu@cern.ch
 * @date March 20, 2020
 * @brief Stores the range of eta/phi/etc. of each sector.
 *
 * Sector slices store the range of eta/phi/etc. of each sector. This is done for
 * all 5 track parameters by binning each parameter, with one bitvector per bin.
 * The size of the bitvectors is the number of sectors, so that bv[sector] represents
 * whether that sector goes through that parameter value.
 *
 * Sector slices are created by ConstGenAlgo and stored in a ROOT file.
 * Sector slices are used by track inversion to select the sectors that it uses to do
 * the inverse fit.
 */

#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTObjects/HTTTypes.h"

#include <TROOT.h>
#include <TClonesArray.h>
#include <TMath.h>


class HTTSectorSlice
{
    public:

        // Create blank bitvectors for writing new slice file
        HTTSectorSlice(size_t nSectors,
                HTTTrackParsI const & nBins, HTTTrackPars const & min, HTTTrackPars const & max);
        // Read from file
        HTTSectorSlice(const std::string & filepath);


        // Slice writing functions
        void addSectorToSlice(sector_t sector, HTTTrackParsI const & bins);
        void saveSlices(const std::string & filepath);


        // Finds the parameter boundaries for where bitmasks are non-empty.
        // Returns (min, max).
        std::pair<HTTTrackPars, HTTTrackPars> getBoundaries() const;

        // Returns a list of sectors that contain pars.
        std::vector<sector_t> searchSectors(HTTTrackPars const & pars) const;

    private:

        // TBits bitmask arrays. The use of TClonesArray is probably overkill but whatever, kept from FTK.
        // I wish ROOT would update these classes to use templating X(
        TClonesArray *m_bits_phi    = nullptr;
        TClonesArray *m_bits_c      = nullptr;
        TClonesArray *m_bits_d0     = nullptr;
        TClonesArray *m_bits_z0     = nullptr;
        TClonesArray *m_bits_eta = nullptr;

        // Number of bins for each parameter, size of arrays above
        HTTTrackParsI m_nBins;

        // Min and max parameters found out in the slice file
        HTTTrackPars m_min;
        HTTTrackPars m_max;

        // Step sizes, (max - min) / nbins
        HTTTrackPars m_step;

        // Helper functions
        void calcDependentVals();
        void getBoundary(const TClonesArray *bitmasks, double x_min, double x_max,
                double &autoMin, double &autoMax, bool wraps, const char *debug) const;
        bool checkTrackPars(HTTTrackPars const & pars) const;
};

#endif // HTTSectorSlice_h

