
 //***************************************************************************
 //                           CMMEtSumsCnv_p1.h  -  description
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
  
#ifndef CMMEtSumsCnv_p1_H
#define CMMEtSumsCnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMMEtSums.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMMEtSums_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo CMMEtSums class */

class CMMEtSumsCnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::CMMEtSums, CMMEtSums_p1 > {

public:
  CMMEtSumsCnv_p1() : m_CMMEtSumsCnv(0) {}

  virtual void persToTrans( const CMMEtSums_p1* persObj, LVL1::CMMEtSums* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::CMMEtSums* transObj, CMMEtSums_p1* persObj, MsgStream &log );

protected:
  CMMEtSumsCnv_p1 *m_CMMEtSumsCnv ;

}; // class TriggerTowerCnv_p1

#endif
