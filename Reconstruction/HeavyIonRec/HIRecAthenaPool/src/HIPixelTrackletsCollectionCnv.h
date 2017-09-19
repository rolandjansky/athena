/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file HIPixelTrackletsCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef HIPixelTrackletsCollectionCnv_H
#define HIPixelTrackletsCollectionCnv_H

#define protected public
#include "GaudiKernel/MsgStream.h"
#undef protected

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "HIGlobal/HIPixelTrackletsCollection.h"
#include "HIRecTPCnv/HIPixelTrackletsCollectionCnv_tlp1.h"

typedef HIPixelTrackletsCollection_tlp1 HIPixelTrackletsCollection_PERS;
typedef T_AthenaPoolCustomCnv<HIPixelTrackletsCollection, HIPixelTrackletsCollection_PERS> HIPixelTrackletsCollectionCnvBase;

class HIPixelTrackletsCollectionCnv : public HIPixelTrackletsCollectionCnvBase
{
  friend class CnvFactory<HIPixelTrackletsCollectionCnv>;
  
 protected:
  HIPixelTrackletsCollectionCnv(ISvcLocator *svcloc );
  ~HIPixelTrackletsCollectionCnv();
  virtual StatusCode initialize();
  virtual HIPixelTrackletsCollection_PERS *createPersistent(HIPixelTrackletsCollection* transCont);
  virtual HIPixelTrackletsCollection *createTransient();
  virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_TPConverter; }
  
 private:
  void updateLog();
  
  IMessageSvc *m_msgSvc;
  MsgStream m_log;
  HIPixelTrackletsCollectionCnv_tlp1 m_TPConverter;

};

#endif

