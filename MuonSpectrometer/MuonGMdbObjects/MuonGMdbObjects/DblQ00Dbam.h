/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/DBAM
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: MUON GEOMETRY FROM AMDB

#ifndef DBLQ00_DBAM_H
#define DBLQ00_DBAM_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>
namespace MuonGM
{
class DblQ00Dbam {

public:
    DblQ00Dbam(){};
    ~DblQ00Dbam();
    DblQ00Dbam(std::unique_ptr<IRDBQuery>&&);

    // data members for DblQ00/DBAM fields
    struct DBAM {
        int version; // VERSION
        int nvrs; // VERSION OF READING
        char amdb[8]; // AMDB NAME
        char test[8]; // TEST NAME
        int mtyp; // MAXIMUM STATION NUMBER
        char name[53][8]; // STATION NAME
        int numbox; // FILE INDEX
    };
    
    const DBAM* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "DBAM"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "DBAM"; };
    
private:
    DBAM* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Dbam & operator=(const DblQ00Dbam &right);
    DblQ00Dbam(const DblQ00Dbam&);
};
} // end of MuonGM namespace

#endif // DBLQ00_DBAM_H

