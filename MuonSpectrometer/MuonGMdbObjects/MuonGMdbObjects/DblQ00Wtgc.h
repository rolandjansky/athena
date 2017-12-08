/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/WTGC
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: TGC GENERAL

#ifndef DBLQ00_WTGC_H
#define DBLQ00_WTGC_H

class IRDBQuery;
namespace MuonGM
{
class DblQ00Wtgc {

public:
    DblQ00Wtgc(){};
    ~DblQ00Wtgc();
    DblQ00Wtgc(IRDBQuery*);

    // data members for DblQ00/WTGC fields
    struct WTGC {
        int version; // VERSION
        int jsta; // JSTA INDEX
        int nbevol; // NUMBER OF DETAILS
        float x0; // X0
        float widchb; // WIDTH OF THE CHBER ALONG Z
        float fwirch; // FRAME WIDTH IN R
        float fwixch; // FRAME WIDTH IN X
        char allname[9][8]; // MATERIAL
    };
    
    const WTGC* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "WTGC"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "WTGC"; };

private:
    WTGC* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Wtgc & operator=(const DblQ00Wtgc &right);
    DblQ00Wtgc(const DblQ00Wtgc&);
};
} // end of MuonGM namespace

#endif // DBLQ00_WTGC_H

