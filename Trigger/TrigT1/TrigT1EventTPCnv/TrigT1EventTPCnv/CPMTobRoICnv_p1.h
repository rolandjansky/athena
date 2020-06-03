/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CPMTobRoICnv_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef CPMTobRoICnv_p1_H
#define CPMTobRoICnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CPMTobRoI.h"

// Local include(s):
#include "TrigT1EventTPCnv/CPMTobRoI_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo CPMTobRoI class */

class CPMTobRoICnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::CPMTobRoI, CPMTobRoI_p1 > {

public:
  CPMTobRoICnv_p1() : m_CPMTobRoICnv(0) {}

  virtual void persToTrans( const CPMTobRoI_p1* persObj, LVL1::CPMTobRoI* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::CPMTobRoI* transObj, CPMTobRoI_p1* persObj, MsgStream &log );

protected:
  CPMTobRoICnv_p1 *m_CPMTobRoICnv ;

}; // class TriggerTowerCnv_p1

#endif
