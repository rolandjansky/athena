/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/ATYP
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: STATION TYPE

#ifndef DBLQ00_ATYP_H
#define DBLQ00_ATYP_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>

namespace MuonGM
{
    
class DblQ00Atyp {

public:
    DblQ00Atyp(){};
    ~DblQ00Atyp();
    DblQ00Atyp(std::unique_ptr<IRDBQuery>&&);
    
    // data members for DblQ00/ATYP fields
    struct ATYP {
        int version; // VERSION
        int jtyp; // AMDB STATION TYPE
        char type[8]; // AMDB STATION NAME
        int nsta; // NUMBER OF STATIONS OF THIS TYPE
    };
    
    const ATYP* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "ATYP"; };
    unsigned int getNObj() const { return m_nObj; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "ATYP"; };

private:
    ATYP* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Atyp & operator=(const DblQ00Atyp &right);
    DblQ00Atyp(const DblQ00Atyp&);
};
    
} // end of MuonGM namespace

#endif // DBLQ00_ATYP_H

