/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           JEMEtSumsCnv_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef JEMEtSumsCnv_p1_H
#define JEMEtSumsCnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/JEMEtSums.h"

// Local include(s):
#include "TrigT1EventTPCnv/JEMEtSums_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo JEMEtSums class */

class JEMEtSumsCnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::JEMEtSums, JEMEtSums_p1 > {

public:
  JEMEtSumsCnv_p1() : m_JEMEtSumsCnv(0) {}

  virtual void persToTrans( const JEMEtSums_p1* persObj, LVL1::JEMEtSums* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::JEMEtSums* transObj, JEMEtSums_p1* persObj, MsgStream &log );

protected:
  JEMEtSumsCnv_p1 *m_JEMEtSumsCnv ;

}; // class TriggerTowerCnv_p1

#endif
