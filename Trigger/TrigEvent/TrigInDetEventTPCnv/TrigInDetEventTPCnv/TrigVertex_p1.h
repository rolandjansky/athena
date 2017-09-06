/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @class  : TrigVertex_p1
 *
 * @brief persistent partner for TrigVertex
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigVertex_p1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGVERTEX_P1_H
#define TRIGINDETEVENTTPCNV_TRIGVERTEX_P1_H

#include <stdint.h>
#include <vector>


#include "AthenaPoolUtilities/TPObjRef.h"

class TrigVertex_p1 
{
  friend class TrigVertexCnv_p1;

 public:
  
  TrigVertex_p1() {}
  virtual ~TrigVertex_p1(){}

  enum AlgoId_p1{NULLID=0, SITRACKID=1, IDSCANID=2, HISTOPRMVTXSITRACKID=3, VKALSECVTXID=4, BPHYSSECID=5,
                 BSFULLSITRACKID=6, BSFULLIDSCANID=7, BSSPLITSITRACKID=8, BSSPLITIDSCANID=9, HISTOPRMVTXIDSCANID=10,
                 HISTOPRMVTXEFID=11, HISTOPRMVTXSITRACKIDTAU=12, HISTOPRMVTXEFIDTAU=13, BJETSECVTXID=14, 
                 BSFULL_STRATEGY_B_ID=15, BSFULL_STRATEGY_A_ID=16, BSSPLIT_STRATEGY_B_ID=17, BSSPLIT_STRATEGY_A_ID=18,
                 HISTOPRMVTXFTKID=19, HISTOPRMVTXFTKIDTAU=20, BSFULL_STRATEGY_F_ID=21, BSSPLIT_STRATEGY_F_ID = 22,
                 HISTOPRMVTXFTKID_REFIT = 23};

 private:

  float                  m_x                ;
  float                  m_y                ;
  float                  m_z                ;
  float                  m_mass             ;
  float                  m_massVar          ;
  double                 m_cov[6]           ;
  float                  m_energyFraction   ;
  int                    m_nTwoTracksSecVtx ;
  float                  m_chiSquared       ;
  int                    m_nDOF             ;
  std::vector<TPObjRef>  m_tracks           ; 
  AlgoId_p1              m_algId            ;  

};

#endif
