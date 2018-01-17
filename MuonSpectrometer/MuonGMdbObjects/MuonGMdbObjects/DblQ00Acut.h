/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/ACUT
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: CUT OF STATION

#ifndef DBLQ00_ACUT_H
#define DBLQ00_ACUT_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>
namespace MuonGM
{
class DblQ00Acut {

public:
    DblQ00Acut(){};
    ~DblQ00Acut();
    DblQ00Acut(std::unique_ptr<IRDBQuery>&&);

    // data members for DblQ00/ACUT fields
    struct ACUT {
        int version; // VERSION
        int i; // INDEX
        int icut; // CUT-OUT INDEX
        int n; // NUMBER OF SUB-CUTS IN THE STATION
    };
    
    const ACUT* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "ACUT"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "ACUT"; };
    
private:
    ACUT* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Acut & operator=(const DblQ00Acut &right);
    DblQ00Acut(const DblQ00Acut&);
};
} // end of MuonGM namespace

#endif // DBLQ00_ACUT_H

