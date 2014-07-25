/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMAIDENTITY_H
#define CMAIDENTITY_H

#include <utility>
#include <iostream>
#include <string>
#include "MuonCablingTools/RPCdef.h"

enum CMAcoverage {EvenSectors, OddSectors, AllSectors};

class CMAidentity
{
    public:
    typedef std::pair < int,int > Offline_indexes;
    typedef std::string PhiCoverage;

    static const char CoverageTAG[3][5];   // Description of coverage type

    static bool coverage(std::string,CMAcoverage&);
    static const char* covtag (const CMAcoverage co) {return CoverageTAG[co];}
    //static const char* name(const ViewType,const CMAcoverage);
    static const std::string name(const ViewType,const CMAcoverage);

    private:
    ViewType m_type;        // Matrix type (0 = eta matrix, 1 = phi matrix)
    CMAcoverage m_coverage; // Sectors (even/odd/all) in which this CMA is used
    int m_number;           // Matrix sequential number (start from 1)
    int m_eta_index;        // Matrix eta address (start from 0)
    int m_phi_index;        // Matrix phi address (start from 0)
    int m_PAD_index;        // PAD index into the sector logic (start from 0)
    int m_Ixx_index;        // Matrix index into the PAD (start from 0)

    CMAcoverage which_sector(PhiCoverage);

    public:
    CMAidentity(ViewType,CMAcoverage,int,int,int,int,int);
    CMAidentity(ViewType,CMAcoverage,int);
    CMAidentity(ViewType,CMAcoverage,Offline_indexes&);
    CMAidentity(ViewType,CMAcoverage,int,int);
    
    ~CMAidentity() {}

    CMAidentity(const CMAidentity&);

    CMAidentity& operator=(const CMAidentity&);
    bool operator ==(const CMAidentity&) const;
    bool operator !=(const CMAidentity&) const;
    bool operator < (const CMAidentity&) const;

    ViewType type(void)        const {return m_type;}
    CMAcoverage coverage(void) const {return m_coverage;}
    int number(void)           const {return m_number;}
    int eta_index(void)        const {return m_eta_index;}
    int phi_index(void)        const {return m_phi_index;}
    int PAD_index(void)        const {return m_PAD_index;}
    int Ixx_index(void)        const {return m_Ixx_index;}

    void inversion(void);

    CMAidentity& operator+=(const CMAidentity&);

    friend std::ostream& operator<<(std::ostream&,const CMAidentity&);
};

#endif
