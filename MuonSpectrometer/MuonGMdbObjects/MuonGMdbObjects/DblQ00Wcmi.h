/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/WCMI
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: CMI SPACER

#ifndef DBLQ00_WCMI_H
#define DBLQ00_WCMI_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>
namespace MuonGM
{
class DblQ00Wcmi {

public:
    DblQ00Wcmi(){};
    ~DblQ00Wcmi();
    DblQ00Wcmi(std::unique_ptr<IRDBQuery>&&);

    // data members for DblQ00/WCMI fields
    struct WCMI {
        int version; // VERSION
        int jsta; // INDEX
        int num; // NUMBER OF OBJECTS
        float heightness; // HEIGHT
        float largeness; // T-SHAPE LARGENESS
        float thickness; // T-SHAPE THICKNESS
    };
    
    const WCMI* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "WCMI"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "WCMI"; };

private:
    WCMI* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Wcmi & operator=(const DblQ00Wcmi &right);
    DblQ00Wcmi(const DblQ00Wcmi&);
};
} // end of MuonGM namespace
    

#endif // DBLQ00_WCMI_H

