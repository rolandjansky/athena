/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/ATLN
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: MATERIAL OF TGC

#ifndef DBLQ00_ATLN_H
#define DBLQ00_ATLN_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>

namespace MuonGM
{
    
class DblQ00Atln {

public:
    DblQ00Atln(){};
    ~DblQ00Atln();
    DblQ00Atln(std::unique_ptr<IRDBQuery>&&);

    // data members for DblQ00/ATLN fields
    struct ATLN {
        int version; // VERSION
        int i; // INDEX
        int icovol; // MATERIAL CODE
        float zpovol; // RELATIVE Z POSITION
        float widvol; // Z WIDTH
        char namvol[8]; // MATERIAL NAME
        int jsta; // 
    };

    const ATLN* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "ATLN"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "ATLN"; };

private:
    ATLN* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Atln & operator=(const DblQ00Atln &right);
    DblQ00Atln(const DblQ00Atln&);
};
} // end of MuonGM namespace

#endif // DBLQ00_ATLN_H

