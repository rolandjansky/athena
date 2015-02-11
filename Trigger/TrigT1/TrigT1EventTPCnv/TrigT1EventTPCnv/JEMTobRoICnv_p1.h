  
 //***************************************************************************
 //  *                                                                         *
 //  *   This program is free software; you can redistribute it and/or modify  *
  // *   it under the terms of the GNU General Public License as published by  *
//   *   the Free Software Foundation; either version 2 of the License, or     *
 //  *   (at your option) any later version.                                   *
 //  *                                                                         *
 //  ***************************************************************************/
  
#ifndef JEMTobRoICnv_p1_H
#define JEMTobRoICnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Local include(s):
#include "TrigT1EventTPCnv/JEMTobRoI_p1.h"

using namespace LVL1;

/** T/P convertor for TrigT1Calo JEMTobRoI class */

class JEMTobRoICnv_p1 : public T_AthenaPoolTPCnvBase< JEMTobRoI, JEMTobRoI_p1 > {

public:
  JEMTobRoICnv_p1() : m_JEMTobRoICnv(0) {}

  virtual void persToTrans( const JEMTobRoI_p1* persObj, JEMTobRoI* transObj, MsgStream &log );
  virtual void transToPers( const JEMTobRoI* transObj, JEMTobRoI_p1* persObj, MsgStream &log );

protected:
  JEMTobRoICnv_p1 *m_JEMTobRoICnv ;

}; // class JEMTobRoICnv_p1

#endif
