/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigInDetTrack_p4
 *
 * @brief persistent partner for TrigInDetTrack
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrack_p4.h,v 1.4 2010-12-04 demelian Exp $
 **********************************************************************************/

#ifndef TrigInDetEventTPCnv_TrigInDetTrack_p4_H
#define TrigInDetEventTPCnv_TrigInDetTrack_p4_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

class TrigInDetTrack_p4 
{

 public:

     TrigInDetTrack_p4() {}
     friend class TrigInDetTrackCnv_p4;

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
     std::vector< unsigned int >  m_rdoList;
};

#endif

