/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CMMEtSumsCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CMMEtSumsCollectionCnv_H
#define CMMEtSumsCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CMMEtSumsCollection.h"
#include "TrigT1EventTPCnv/CMMEtSumsCollectionCnv_tlp1.h"

typedef CMMEtSumsCollection_tlp1 CMMEtSumsCollection_PERS;
typedef T_AthenaPoolCustomCnv<CMMEtSumsCollection, CMMEtSumsCollection_PERS> CMMEtSumsCollectionCnvBase;

class CMMEtSumsCollectionCnv: public CMMEtSumsCollectionCnvBase {
friend class CnvFactory<CMMEtSumsCollectionCnv>;

public:
  CMMEtSumsCollectionCnv (ISvcLocator* svcloc) :
    CMMEtSumsCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CMMEtSumsCollectionCnv" )
     {}
  
protected:
  
  virtual CMMEtSumsCollection_PERS*   createPersistent (CMMEtSumsCollection* transCont);
  virtual CMMEtSumsCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CMMEtSumsCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


