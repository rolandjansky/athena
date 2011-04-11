/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file TriggerTowerCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef TriggerTowerCollectionCnv_H
#define TriggerTowerCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1EventTPCnv/TriggerTowerCollectionCnv_tlp1.h"

typedef TriggerTowerCollection_tlp1 TriggerTowerCollection_PERS;
typedef T_AthenaPoolCustomCnv<TriggerTowerCollection, TriggerTowerCollection_PERS> TriggerTowerCollectionCnvBase;

class TriggerTowerCollectionCnv: public TriggerTowerCollectionCnvBase {
friend class CnvFactory<TriggerTowerCollectionCnv>;

protected:

  TriggerTowerCollectionCnv (ISvcLocator* svcloc) :
    TriggerTowerCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "TriggerTowerCollectionCnv" )
     {}
  
  ~TriggerTowerCollectionCnv() {}
  
  virtual TriggerTowerCollection_PERS*   createPersistent (TriggerTowerCollection* transCont);
  virtual TriggerTowerCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  TriggerTowerCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


