
 //***************************************************************************
 //                           JEMHitsCnv_p1.h  -  description
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
  
#ifndef JEMHitsCnv_p1_H
#define JEMHitsCnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/JEMHits.h"

// Local include(s):
#include "TrigT1EventTPCnv/JEMHits_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo JEMHits class */

class JEMHitsCnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::JEMHits, JEMHits_p1 > {

public:
  JEMHitsCnv_p1() : m_JEMHitsCnv(0) {}

  virtual void persToTrans( const JEMHits_p1* persObj, LVL1::JEMHits* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::JEMHits* transObj, JEMHits_p1* persObj, MsgStream &log );

protected:
  JEMHitsCnv_p1 *m_JEMHitsCnv ;

}; // class TriggerTowerCnv_p1

#endif
