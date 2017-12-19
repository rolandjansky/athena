/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/ALIN
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: LINE OF CUT

#ifndef DBLQ00_ALIN_H
#define DBLQ00_ALIN_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>
namespace MuonGM
{
class DblQ00Alin {

public:
    DblQ00Alin(){};
    ~DblQ00Alin();
    DblQ00Alin(std::unique_ptr<IRDBQuery>&&);

    // data members for DblQ00/ALIN fields
    struct ALIN {
        int      version;             // VERSION
        float    dx;                   // X RELATIVE POSITION OF THE SUB-CUT
        float    dy;                 // Y RELATIVE POSITION OF THE SUB-CUT
        int      i;                 // SERIAL NB. OF THE OBJECT IN WHICH THE SU
        float    width_xs;         // S DIMENSIONS OF THE SUB-CUT
        float    width_xl;         // L DIMENSIONS OF THE SUB-CUT
        float    length_y;         // Y DIMENSIONS OF THE SUB-CUT
        float    excent;          // EXC DIMENSIONS OF THE SUB-CUT
        float    dead1;           // D1 DIMENSIONS OF THE SUB-CUT
        int      jtyp;                  // STATION TYPE
        int      indx;                // INDEX
        int      icut;                // CUT-OUT INDEX
    };
    
    const ALIN* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "ALIN"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "ALIN"; };

private:
    ALIN* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Alin & operator=(const DblQ00Alin &right);
    DblQ00Alin(const DblQ00Alin&);
};
} // end of MuonGM namespace

#endif // DBLQ00_ALIN_H

