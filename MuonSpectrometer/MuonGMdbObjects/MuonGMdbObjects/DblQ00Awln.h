/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/AWLN
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: RPC DETAILS

#ifndef DBLQ00_AWLN_H
#define DBLQ00_AWLN_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>
namespace MuonGM
{
class DblQ00Awln {

public:
    DblQ00Awln(){};
    ~DblQ00Awln();
    DblQ00Awln(std::unique_ptr<IRDBQuery>&&);
    
    // data members for DblQ00/AWLN fields
    struct AWLN {
        int version; // VERSION
        int jsta; // JSTA TYPE NUMBER
        float spitch; // S-STRIPS PITCH
        float zpitch; // Z-STRIPS PITCH
        float dedstr; // DEAD REAGION BETWEEN STRIP
        int nsrest; // NBER OF S STRIPS READOUTS
        int nzrest; // NBER OF S GAS GAPS
        float sfirst; // S-PHI STRIP OFFSET
        float zfirst; // Z-ETA STRIP OFFSET
        float dedsep; // DEAD SEPARATION
        int nsrost; // NUMBER OF S-PHI READOUT STRIPS
        int nzrost; // NUMBER OF Z-ETA READOUT STRIPS
    };
    
    const AWLN* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "AWLN"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "AWLN"; };

private:
    AWLN* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Awln & operator=(const DblQ00Awln &right);
    DblQ00Awln(const DblQ00Awln&);
};
} // end of MuonGM namespace

#endif // DBLQ00_AWLN_H

