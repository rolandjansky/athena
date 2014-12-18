/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/WSUP
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: RPC SUPPORT

#ifndef DBLQ00_WSUP_H
#define DBLQ00_WSUP_H

class IRDBQuery;
namespace MuonGM
{
class DblQ00Wsup {

public:
    DblQ00Wsup(){};
    ~DblQ00Wsup();
    DblQ00Wsup(IRDBQuery*);

    // data members for DblQ00/WSUP fields
    struct WSUP {
        int version; // VERSION
        int jsta; // INDEX
        int nxxsup; // MAX NB. FOR X FRAGMENTS
        int nzzsup; // MAX NB. FOR Z FRAGMENTS
        float x0; // X0
        float thickn; // THICKNESS
        float xxsup[4]; // X DIMENSION
        float zzsup[4]; // Z DIMENSION
    };
    
    const WSUP* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "WSUP"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "WSUP"; };

private:
    WSUP* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Wsup & operator=(const DblQ00Wsup &right);
    DblQ00Wsup(const DblQ00Wsup&);
};
} // end of MuonGM namespace

#endif // DBLQ00_WSUP_H

