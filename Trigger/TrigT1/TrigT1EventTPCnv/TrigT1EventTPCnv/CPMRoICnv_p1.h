
 //***************************************************************************
 //                           CPMRoICnv_p1.h  -  description
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
  
#ifndef CPMRoICnv_p1_H
#define CPMRoICnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CPMRoI.h"

// Local include(s):
#include "TrigT1EventTPCnv/CPMRoI_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo CPMRoI class */

class CPMRoICnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::CPMRoI, CPMRoI_p1 > {

public:
  CPMRoICnv_p1() : m_CPMRoICnv(0) {}

  virtual void persToTrans( const CPMRoI_p1* persObj, LVL1::CPMRoI* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::CPMRoI* transObj, CPMRoI_p1* persObj, MsgStream &log );

protected:
  CPMRoICnv_p1 *m_CPMRoICnv ;

}; // class TriggerTowerCnv_p1

#endif
