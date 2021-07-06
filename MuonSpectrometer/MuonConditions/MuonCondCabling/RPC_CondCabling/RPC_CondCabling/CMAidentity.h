/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMAIDENTITY_H
#define CMAIDENTITY_H

#include <iostream>
#include <string>
#include <utility>

#include "MuonCablingTools/RPCdef.h"

enum CMAcoverage { EvenSectors, OddSectors, AllSectors };

class CMAidentity {
public:
    typedef std::pair<int, int> Offline_indexes;
    typedef std::string PhiCoverage;

    static const char CoverageTAG[3][5];  // Description of coverage type

    static bool coverage(const std::string&, CMAcoverage&);
    static const char* covtag(const CMAcoverage co) { return CoverageTAG[co]; }
    static const std::string name(const ViewType, const CMAcoverage);

    struct defineParams {
        defineParams() = default;
        defineParams(const defineParams&) = default;
        defineParams& operator=(const defineParams&) = default;
        defineParams(defineParams&&) = default;
        /// CMA identity params
        CMAcoverage coverage{CMAcoverage::AllSectors};
        ViewType view{ViewType::NoView};  /// Matrix type (0 = eta matrix, 1 = phi matrix)
        int etaIndex{-1};                 /// Matrix eta address (start from 0)
        int phiIndex{-1};                 /// Matrix chi address (start from 0)
        int padIndex{-1};                 /// PAD index into the sector logic (start from 0)
        int IxxIndex{-1};                 /// Matrix index into the PAD (start from 0)
        int seqNumber{-1};                /// Matrix sequential number (start from 1)
    };

private:
    defineParams m_params{};
    CMAcoverage which_sector(PhiCoverage);

public:
    CMAidentity(defineParams pars);

    CMAidentity(ViewType, CMAcoverage, int, int, int, int, int);
    CMAidentity(ViewType, CMAcoverage, int);
    CMAidentity(ViewType, CMAcoverage, Offline_indexes&);
    CMAidentity(ViewType, CMAcoverage, int, int);

    ~CMAidentity() = default;

    CMAidentity(const CMAidentity&) = default;
    CMAidentity& operator=(const CMAidentity&) = default;

    bool operator==(const CMAidentity&) const;
    bool operator!=(const CMAidentity&) const;
    bool operator<(const CMAidentity&) const;

    ViewType type() const;
    CMAcoverage coverage() const;
    int number() const;
    int eta_index() const;
    int phi_index() const;
    int PAD_index() const;
    int Ixx_index() const;
    void inversion();

    CMAidentity& operator+=(const CMAidentity&);

    friend std::ostream& operator<<(std::ostream&, const CMAidentity&);
};

#endif
