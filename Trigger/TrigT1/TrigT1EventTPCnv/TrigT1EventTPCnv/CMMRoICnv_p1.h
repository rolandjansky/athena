
 //***************************************************************************
 //                           CMMRoICnv_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     copyright            : (C) 2008 by Alan Watson
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
 //***************************************************************************
 //  *                                                                         *
 //  *   This program is free software; you can redistribute it and/or modify  *
  // *   it under the terms of the GNU General Public License as published by  *
//   *   the Free Software Foundation; either version 2 of the License, or     *
 //  *   (at your option) any later version.                                   *
 //  *                                                                         *
 //  ***************************************************************************/
  
#ifndef CMMRoICnv_p1_H
#define CMMRoICnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMMRoI.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMMRoI_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo CMMRoI class */

class CMMRoICnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::CMMRoI, CMMRoI_p1 > {

public:
  CMMRoICnv_p1() : m_CMMRoICnv(0) {}

  virtual void persToTrans( const CMMRoI_p1* persObj, LVL1::CMMRoI* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::CMMRoI* transObj, CMMRoI_p1* persObj, MsgStream &log );

protected:
  CMMRoICnv_p1 *m_CMMRoICnv ;

}; // class TriggerTowerCnv_p1

#endif
