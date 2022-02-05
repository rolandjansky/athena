/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_REGIONDATAFROMFILE_H
#define MUONCALIB_REGIONDATAFROMFILE_H

#include <iostream>
#include <vector>

namespace MuonCalib {

    /**
     Holds the station, eta, phi and multilayer Ids for a given multilayer.
    */
    struct MultilayerId {
        MultilayerId() = default;
        MultilayerId(int st, int e, int p, int m) : stName(st), eta(e), phi(p), ml(m) {}
        int stName{-1};
        int eta{-1};
        int phi{-1};
        int ml{-1};
    };

    /**
     Manages the I/O of the list of multilayers belonging to a calibration region.
    */
    class RegionDataFromFile {
    public:
        typedef MultilayerId id_type;
        typedef std::vector<id_type> id_vec;

    public:
        /** returns a vector containing all identifiers for the given regionId,
            returns an empty vector if the regionId is out of range or the region is empty */
        id_vec getIdentifiers(int regionId) const;

        /** returns the number of regions */
        unsigned int nregions() const;

        void setNRegions(unsigned int nreg) {
            m_nregions = nreg;
            m_identifiersPerRegions.resize(nreg);
        }

        /** returns false if the region was already present,
            nothing is added in that case */
        bool addRegion(int regionIndex, const id_vec& ids);

        /** clear entries */
        void clear();

        std::istream& read(std::istream& is);
        std::ostream& write(std::ostream& os) const;

    private:
        typedef std::vector<id_vec> id_vec_vec;

        /** map containing all identifiers for a given region id */
        unsigned int m_nregions;
        id_vec_vec m_identifiersPerRegions;
    };

}  // namespace MuonCalib

std::istream& operator>>(std::istream& is, MuonCalib::RegionDataFromFile& data);
std::ostream& operator<<(std::ostream& os, const MuonCalib::RegionDataFromFile& data);

#endif
