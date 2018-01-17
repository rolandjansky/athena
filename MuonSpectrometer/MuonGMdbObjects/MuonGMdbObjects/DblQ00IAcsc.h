/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/IACSC
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: CSC internal alignment parameters - class to read from DB

#include <string>

#ifndef DBLQ00_IACSC_H
#define DBLQ00_IACSC_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>

namespace MuonGM
{
    

class DblQ00IAcsc {

public:
    DblQ00IAcsc();
    ~DblQ00IAcsc();
    DblQ00IAcsc(std::unique_ptr<IRDBQuery>&&);
    DblQ00IAcsc(std::string asciiFileName);
    
    void WriteIAcscToAsciiFile(std::string filename);

    // data members for DblQ00/IACSC fields
    struct IACSC {
        int version; // VERSION
        int line; // LINE NUMBER
        char type[8]; // STATION TYPE
        int jff; // PHI POSITION
        int jzz; // Z POSITION
        int job; // JOB POSITION
        int wireLayer; // JOB POSITION
        float tras; // S TRANSLATION [MM]
        float traz; // Z TRANSLATION
        float trat; // T TRANSLATION
        float rots; // S ROTATION [RAD]
        float rotz; // Z ROTATION
        float rott; // T ROTATION
        int i; // STATION AMDB INDEX
    };

    const IACSC* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "IACSC"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "IACSC"; };

private:
    IACSC* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00IAcsc & operator=(const DblQ00IAcsc &right);
    DblQ00IAcsc(const DblQ00IAcsc&);
};


} // end of MuonGM namespace

#endif // DBLQ00_ASZT_H

