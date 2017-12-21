/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/WCRO
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: CRO SPACER

#ifndef DBLQ00_WCRO_H
#define DBLQ00_WCRO_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>
namespace MuonGM
{
class DblQ00Wcro {

public:
    DblQ00Wcro(){};
    ~DblQ00Wcro();
    DblQ00Wcro(std::unique_ptr<IRDBQuery>&&);
    
    // data members for DblQ00/WCRO fields
    struct WCRO {
        int version; // VERSION
        int jsta; // INDEX
        int num; // NUMBER OF OBJECTS
        float heightness; // HEIGHT
        float largeness; // T-SHAPE LARGENESS
        float thickness; // T-SHAPE THICKNESS
    };
    
    const WCRO* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "WCRO"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "WCRO"; };

private:
    WCRO* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Wcro & operator=(const DblQ00Wcro &right);
    DblQ00Wcro(const DblQ00Wcro&);
};
} // end of MuonGM namespace

#endif // DBLQ00_WCRO_H

