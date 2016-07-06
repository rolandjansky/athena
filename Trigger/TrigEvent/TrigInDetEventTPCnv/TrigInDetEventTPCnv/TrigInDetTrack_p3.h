/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigInDetTrack_p2
 *
 * @brief persistent partner for TrigInDetTrack
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrack_p2.h,v 1.2 2009-04-01 22:08:44 ilija@vukotic.me Exp $
 **********************************************************************************/

#ifndef TrigInDetEventTPCnv_TrigInDetTrack_p3_H
#define TrigInDetEventTPCnv_TrigInDetTrack_p3_H

#include "AthenaPoolUtilities/TPObjRef.h"

class TrigInDetTrack_p3 
{

 public:

     TrigInDetTrack_p3() {}
     friend class TrigInDetTrackCnv_p3;

     //private:

     TPObjRef m_param;    //actual type is TrigInDetTrackFitPar*
     TPObjRef m_endParam; //actual type is TrigInDetTrackFitPar*
     double  m_chi2;
     
     // TrigInDetTrack::AlgoId m_algId;
     // int     m_NStrawHits;
     // int     m_NStraw;
     // int     m_NStrawTime;
     // int     m_NTRHits;  
     // int     m_NPixelSpacePoints;
     // int     m_NSCT_SpacePoints;
     // long    m_HitPattern;
     int    m_allIntegers[8]; // all the integers above are saved in this one array.
};

#endif

