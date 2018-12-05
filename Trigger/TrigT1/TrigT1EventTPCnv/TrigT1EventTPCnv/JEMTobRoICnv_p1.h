/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
  
  
#ifndef JEMTobRoICnv_p1_H
#define JEMTobRoICnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/JEMTobRoI.h"

// Local include(s):
#include "TrigT1EventTPCnv/JEMTobRoI_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo JEMTobRoI class */

class JEMTobRoICnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::JEMTobRoI, JEMTobRoI_p1 > {

public:
  JEMTobRoICnv_p1() : m_JEMTobRoICnv(0) {}

  virtual void persToTrans( const JEMTobRoI_p1* persObj, LVL1::JEMTobRoI* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::JEMTobRoI* transObj, JEMTobRoI_p1* persObj, MsgStream &log );

protected:
  JEMTobRoICnv_p1 *m_JEMTobRoICnv ;

}; // class JEMTobRoICnv_p1

#endif
