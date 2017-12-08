/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/WDED
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: RPC PANEL

#ifndef DBLQ00_WDED_H
#define DBLQ00_WDED_H

class IRDBQuery;
namespace MuonGM
{
class DblQ00Wded {

public:
    DblQ00Wded(){};
    ~DblQ00Wded();
    DblQ00Wded(IRDBQuery*);

    // data members for DblQ00/WDED fields
    struct WDED {
        int version; // VERSION
        int jsta; // INDEX
        int nb; // NUMBER OF DETAILS
        float x0; // X0
        float auphcb; // HONEYCOMB THICKNESS
        float tckded; // ALUMINIUM THICKNESS
    };
    
    const WDED* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "WDED"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "WDED"; };

private:
    WDED* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Wded & operator=(const DblQ00Wded &right);
    DblQ00Wded(const DblQ00Wded&);
};
} // end of MuonGM namespace


#endif // DBLQ00_WDED_H

