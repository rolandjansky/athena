/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           JEMRoICnv_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef JEMRoICnv_p1_H
#define JEMRoICnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/JEMRoI.h"

// Local include(s):
#include "TrigT1EventTPCnv/JEMRoI_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo JEMRoI class */

class JEMRoICnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::JEMRoI, JEMRoI_p1 > {

public:
  JEMRoICnv_p1() : m_JEMRoICnv(0) {}

  virtual void persToTrans( const JEMRoI_p1* persObj, LVL1::JEMRoI* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::JEMRoI* transObj, JEMRoI_p1* persObj, MsgStream &log );

protected:
  JEMRoICnv_p1 *m_JEMRoICnv ;

}; // class TriggerTowerCnv_p1

#endif
