/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/WRPC
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: RPC GENERAL

#ifndef DBLQ00_WRPC_H
#define DBLQ00_WRPC_H

class IRDBQuery;
namespace MuonGM
{
class DblQ00Wrpc {

public:
    DblQ00Wrpc(){};
    ~DblQ00Wrpc();
    DblQ00Wrpc(IRDBQuery*);
    
    // data members for DblQ00/WRPC fields
    struct WRPC {
        int version; // VERSION
        int nvrs; // VERSION OF RPC TECHNOLOGY
        int layrpc; // LAYERS NUMBER
        float tckrla; // THICK. OF AN RPC LAYER
        float tottck; // TOTAL THICKNESS
        float tckfsp; // THICK. OF FOAM SPACER
        float ackfsp; // THICK. OF AL PLATE OF FOAM SPACER
        float tlohcb; // THICK. OF LOWER HONEYCOMB
        float alohcb; // THICK. OF AL PLATE OF LOWER HONEYCOMB
        float tckbak; // THICK. OF BAKELITE
        float tckgas; // THICK. OF GAS GAP
        float tckssu; // THICK. OF STRIPS SUPPORT
        float tckstr; // THICK. OF STRIPS
        float sdedmi; // S INTERNAL MID-CHBER DEAD REGION
        float zdedmi; // Z INTERNAL MID-CHBER DEAD REGION
        float spdiam; // SPACER DIAMETER
        float sppitc; // SPACER PITCH
        float stroff[3]; // STRIP OFFSET S, FIRST Z, SECOND Z
    };
    
    const WRPC* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "WRPC"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "WRPC"; };

private:
    WRPC* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Wrpc & operator=(const DblQ00Wrpc &right);
    DblQ00Wrpc(const DblQ00Wrpc&);
};
} // end of MuonGM namespace

#endif // DBLQ00_WRPC_H

