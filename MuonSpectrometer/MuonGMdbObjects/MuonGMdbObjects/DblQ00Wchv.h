/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/WCHV
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: CHV SPACER

#ifndef DBLQ00_WCHV_H
#define DBLQ00_WCHV_H

class IRDBQuery;
namespace MuonGM
{
class DblQ00Wchv {

public:
    DblQ00Wchv(){};
    ~DblQ00Wchv();
    DblQ00Wchv(IRDBQuery*);
    
    // data members for DblQ00/WCHV fields
    struct WCHV {
        int version; // VERSION
        int jsta; // INDEX
        int num; // NUMBER OF OBJECTS
        float heightness; // HEIGHT
        float largeness; // T-SHAPE LARGENESS
        float thickness; // T-SHAPE THICKNESS
    };
    
    const WCHV* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "WCHV"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "WCHV"; };

private:
    WCHV* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Wchv & operator=(const DblQ00Wchv &right);
    DblQ00Wchv(const DblQ00Wchv&);
};
} // end of MuonGM namespace

#endif // DBLQ00_WCHV_H

