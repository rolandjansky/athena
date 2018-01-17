/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/APTP
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: POSITION OF STATION

#ifndef DBLQ00_APTP_H
#define DBLQ00_APTP_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>

namespace MuonGM
{

class DblQ00Aptp {

public:
    DblQ00Aptp(){};
    ~DblQ00Aptp();
    DblQ00Aptp(std::unique_ptr<IRDBQuery>&&);

    // data members for DblQ00/APTP fields
    struct APTP {
        int   version;   // VERSION
        int   line;      // LINE NUMBER
        char  type[8];   // STATION TYPE
        int   i;         // STATION AMDB INDEX
        int   icut;      // CUT-OUT INDEX,ZERO IF MISSING
        int   iphi[8];   // PHI INDICATES OF OCTANTS
        int   iz;        // Z (FOR BARREL) OR R (FOR END-CAPS) POS.
        float dphi;      // RELATIVE PHI POSITION OF THE STATION IN
        float z;         // Z POSITION OF THE LOWEST Z EDGE OF THE S
        float r;         // RADIAL POSITION OF ITS INNERMOST EDGE
        float s;         // ORTHO-RADIAL POSITION OF THE CENTER OF T
        float alfa;       // ALFA ANGLE DEFINING THE DEVIATION [GRAD]
        float beta;       // BETA ANGLE DEFINING THE DEVIATION
        float gamma;      // GAMMA ANGLE DEFINING THE DEVIATION
    };

    const APTP* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "APTP"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "APTP"; };

private:
    APTP* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Aptp & operator=(const DblQ00Aptp &right);
    DblQ00Aptp(const DblQ00Aptp&);
};
}// end of MuonGM namespace

#endif // DBLQ00_APTP_H

