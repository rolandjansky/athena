/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file ZDC_SimStripHit_CollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef ZDC_SimStripHit_CollectionCnv_H
#define ZDC_SimStripHit_CollectionCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "ZDC_SimEvent/ZDC_SimStripHit_Collection.h"
#include "ZdcEventTPCnv/ZDC_SimStripHit_Collection_p1.h"

typedef ZDC_SimStripHit_Collection_p1 ZDC_SimStripHit_Collection_PERS;

typedef T_AthenaPoolCustomCnv<ZDC_SimStripHit_Collection, ZDC_SimStripHit_Collection_PERS> ZDC_SimStripHit_CollectionCnvBase;

class ZDC_SimStripHit_CollectionCnv  : public ZDC_SimStripHit_CollectionCnvBase {

  friend class CnvFactory<ZDC_SimStripHit_CollectionCnv>;

protected:

  ZDC_SimStripHit_CollectionCnv(ISvcLocator* svcloc) : ZDC_SimStripHit_CollectionCnvBase ( svcloc) {}
  virtual ZDC_SimStripHit_Collection_PERS*  createPersistent(ZDC_SimStripHit_Collection* transCont);
  virtual ZDC_SimStripHit_Collection*       createTransient ();

};

#endif

