/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbProtoTrack_H
#define MbProtoTrack_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbDRAT.h"

  /**
   @class MbProtoTrack

  @author samusog@cern.ch
  
  */

class MbProtoTrack {
public:
    MbProtoTrack();
    virtual ~MbProtoTrack();
    MbProtoTrack(const MbProtoTrack& ToBeCopied);
    MbProtoTrack& operator=( const MbProtoTrack& ToBeCopied );

//m_MboyStatus:
//-1: unknown
// 0: never reconstructed
// 1: produced as primary fit
// 2: produced as secondary fit
// 3: produced as main fit
    int    m_MboyStatus ;

    double m_Xpoint ;
    double m_Ypoint ;
    double m_Zpoint ;

    double m_Xdir ;
    double m_Ydir ;
    double m_Zdir ;

    double m_Mome ;
    double m_Charge ;

    int    m_ISC0 ;

    double m_DZT1 ;
    double m_DS1  ;
    double m_DZT2 ;
    double m_DS2  ;
    
    double m_SMU  ;

    double m_CFI  ;
    double m_SFI  ;
    double m_CFA  ;
    double m_SFA  ;

    double m_Z1   ;
    double m_T1   ;
    double m_S1   ;

    double m_Z2   ;
    double m_T2   ;
    double m_S2   ;

    int   m_Own_m_VectorOfMbDRAT   ;

//  MbProtoTrack does NOT own these MbDRAT objects but if m_Own_m_VectorOfMbDRAT is 1
    std::vector<MbDRAT*> m_VectorOfMbDRAT ; 
 
//  MbProtoTrack does own these MbDRAT objects
    std::vector<MbDRAT*> m_VectorOfMbDRATTRAKDO ; 
 
};

#endif
