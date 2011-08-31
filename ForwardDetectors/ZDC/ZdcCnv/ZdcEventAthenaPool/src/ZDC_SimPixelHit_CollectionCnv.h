/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file ZDC_SimPixelHit_CollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef ZDC_SimPixelHit_CollectionCnv_H
#define ZDC_SimPixelHit_CollectionCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "ZDC_SimEvent/ZDC_SimPixelHit_Collection.h"
#include "ZdcEventTPCnv/ZDC_SimPixelHit_Collection_p1.h"

typedef ZDC_SimPixelHit_Collection_p1 ZDC_SimPixelHit_Collection_PERS;

typedef T_AthenaPoolCustomCnv<ZDC_SimPixelHit_Collection, ZDC_SimPixelHit_Collection_PERS> ZDC_SimPixelHit_CollectionCnvBase;

class ZDC_SimPixelHit_CollectionCnv  : public ZDC_SimPixelHit_CollectionCnvBase {

  friend class CnvFactory<ZDC_SimPixelHit_CollectionCnv>;

protected:

  ZDC_SimPixelHit_CollectionCnv(ISvcLocator* svcloc) : ZDC_SimPixelHit_CollectionCnvBase ( svcloc) {}
  virtual ZDC_SimPixelHit_Collection_PERS*  createPersistent(ZDC_SimPixelHit_Collection* transCont);
  virtual ZDC_SimPixelHit_Collection*       createTransient ();

};

#endif

