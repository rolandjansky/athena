/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/WMDT
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: MDT

#ifndef DBLQ00_WMDT_H
#define DBLQ00_WMDT_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>
namespace MuonGM
{
class DblQ00Wmdt {

public:
    DblQ00Wmdt(){};
    ~DblQ00Wmdt();
    DblQ00Wmdt(std::unique_ptr<IRDBQuery>&&);

    // data members for DblQ00/WMDT fields
    struct WMDT {
        int version; // VERSION
        char typ[8]; // NAME
        int iw; // INDEX
        float x0; // X0
        int laymdt; // MAXIMUM LAYER NUMBER
        float tubpit; // PITCH BETWEEN TUBE
        float tubrad; // RADIUS OF TUBE
        float tubsta; // THICKNESS OF TUBE
        float tubdea; // DEAD LENGTH IN TUBES
        float tubxco[4]; // Y TUBE POSITION
        float tubyco[4]; // X TUBE POSITION
        float tubwal; // TUBE WALL THICKNESS
    };
    
    const WMDT* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "WMDT"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "WMDT"; };
    
private:
    WMDT* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Wmdt & operator=(const DblQ00Wmdt &right);
    DblQ00Wmdt(const DblQ00Wmdt&);
};
} // end of MuonGM namespace

#endif // DBLQ00_WMDT_H

