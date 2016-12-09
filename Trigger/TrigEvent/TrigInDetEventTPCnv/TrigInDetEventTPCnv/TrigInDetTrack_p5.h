/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigInDetTrack_p5
 *
 * @brief persistent partner for TrigInDetTrack
 *
 **********************************************************************************/

#ifndef TrigInDetEventTPCnv_TrigInDetTrack_p5_H
#define TrigInDetEventTPCnv_TrigInDetTrack_p5_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

class TrigInDetTrack_p5 
{
 public:

     TrigInDetTrack_p5() {}
     friend class TrigInDetTrackCnv_p5;

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
     std::vector< unsigned long long >  m_rdoList;
};

#endif

