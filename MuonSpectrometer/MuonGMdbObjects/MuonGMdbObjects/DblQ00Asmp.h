/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/ASMP
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: SINGLE MUON STATION

#ifndef DBLQ00_ASMP_H
#define DBLQ00_ASMP_H

class IRDBQuery;

namespace MuonGM
{
class DblQ00Asmp {

public:
    DblQ00Asmp(){};
    ~DblQ00Asmp();
    DblQ00Asmp(IRDBQuery*);
    // data members for DblQ00/ASMP fields

    struct ASMP {
        int version; // VERSION
        int indx; // STATION NUMBER (INSIDE TYPE)
        int n; // NUMBER OF ELEMENTS
        int jtyp; // AMDB STATION TYPE
    };
    
    const ASMP* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "ASMP"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "ASMP"; };

private:
  ASMP* m_d;
  unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
  DblQ00Asmp & operator=(const DblQ00Asmp &right);
  DblQ00Asmp(const DblQ00Asmp&);
};
} // end of MuonGM namespace

#endif // DBLQ00_ASMP_H

