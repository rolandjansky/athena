/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/ALMN
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: MUON STATION ELEMENT

#ifndef DBLQ00_ALMN_H
#define DBLQ00_ALMN_H

class IRDBQuery;

namespace MuonGM
{
    

class DblQ00Almn {

public:
    DblQ00Almn(){};
    ~DblQ00Almn();
    DblQ00Almn(IRDBQuery*);
    
    // data members for DblQ00/ALMN fields
    struct ALMN {
        int version; // VERSION
        int i; // ELEMENT NUMBER
        float dx; // X RELATIVE POSITION OF THE OBJECT
        float dy; // Y RELATIVE POSITION OF THE OBJECT
        float dz; // Z RELATIVE POSITION OF THE OBJECT
        int job; // OBJECT SERIAL NUMBER IN THE STATION
        char tec[8]; // TYPE OF TECHNOLOGY USED FOR THE OBJECT
        int iw; // INNER STRUCTURE TYPE INDEX, ISTA
        int isplit_x; // NUMBER OF SECTIONS IN X
        int isplit_y; // NUMBER OF SECTIONS IN Y
        int ishape; // TYPE OF GEOMETRICAL SHAPE 0-TRAPEZOIDAL,
        float width_xs; // S WIDTH, WS
        float width_xl; // L WIDTH, WL
        float length_y; // Y LENGTH, LE
        float excent; // ADDITIONAL INFORMATION, EX
        float dead1; // FIRST DEAD MATERIAL, D1
        float dead2; // SECOND DEAD MATERIAL, D2
        float dead3; // STEPS, D3
        int jtyp; // STATION TYPE
        int indx; // ELEMENT NUMBER
    };

    const ALMN* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "ALMN"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "ALMN"; };

private:
    ALMN* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Almn & operator=(const DblQ00Almn &right);
    DblQ00Almn(const DblQ00Almn&);
};


} // end of MuonGM namespace

#endif // DBLQ00_ALMN_H

