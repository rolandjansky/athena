/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CMMCPHitsCnv_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef CMMCPHitsCnv_p1_H
#define CMMCPHitsCnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMMCPHits.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMMCPHits_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo CMMCPHits class */

class CMMCPHitsCnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::CMMCPHits, CMMCPHits_p1 > {

public:
  CMMCPHitsCnv_p1() : m_CMMCPHitsCnv(0) {}

  virtual void persToTrans( const CMMCPHits_p1* persObj, LVL1::CMMCPHits* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::CMMCPHits* transObj, CMMCPHits_p1* persObj, MsgStream &log );

protected:
  CMMCPHitsCnv_p1 *m_CMMCPHitsCnv ;

}; // class TriggerTowerCnv_p1

#endif
