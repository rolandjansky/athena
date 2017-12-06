/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/WCSC
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: CSC

#ifndef DBLQ00_WCSC_H
#define DBLQ00_WCSC_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>
namespace MuonGM
{
class DblQ00Wcsc {

public:
    DblQ00Wcsc(){};
    ~DblQ00Wcsc();
    DblQ00Wcsc(std::unique_ptr<IRDBQuery>&&);

    // data members for DblQ00/WCSC fields
    struct WCSC {
        int version; // VERSION
        int jsta; // JSTA TYPE NUMBER
        int laycsc; // NBER OF CSCS LAYERS
        float ttotal; // TOTAL THICKNESS
        float tnomex; // NOMEX HONEYCOMB THICKNESS
        float tlag10; // G10 LAMINATES THICKNESS
        float wispa; // WIRE SPACING
        float dancat; // ANODE-CATHODE DISTANCE
        float pcatre; // CATHODE READOUT PITCH
        float gstrip; // GAP BETWEEN CATHODE STRIPS
        float wrestr; // WIDTH OF READOUT STRIPS
        float wflstr; // WIDTH OF FLOATING STRIPS
        float trrwas; // RIGIT RECTANGULAR WASHER THICKNES
        float wroxa; // ROXACELL WIDTH
        float groxwi; // ROXACELL AND WIRE BAR GAP
        float wgasba; // FULL GAS GAP BAR WIDTH
        float tgasba; // FULL GAS GAP BAR THICK.
        float wgascu; // CUTS GAS GAP BAR WIDTH
        float tgascu; // CUTS GAS GAP BAR THICK.
        float wfixwi; // FULL WIRE FIX. BAR WID.
        float tfixwi; // FULL WIRE FIX. BAR THICK.
        float pba1wi; // WIRE BAR POSITION
        float pba2wi; // WIRE BAR POSITION
        float pba3wi; // WIRE BAR POSITION
        float psndco; // 2ND COORDINATE PITCH
        float azcat;  // this actually is the 2ND COORDINATE PITCH
    };
    
    const WCSC* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "WCSC"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "WCSC"; };

private:
    WCSC* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Wcsc & operator=(const DblQ00Wcsc &right);
    DblQ00Wcsc(const DblQ00Wcsc&);
};
} // end of MuonGM namespace

#endif // DBLQ00_WCSC_H

