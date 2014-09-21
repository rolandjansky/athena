/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CPMTobRoICollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CPMTobRoICollectionCnv_H
#define CPMTobRoICollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CPMTobRoICollection.h"
#include "TrigT1EventTPCnv/CPMTobRoICollectionCnv_tlp1.h"

typedef CPMTobRoICollection_tlp1 CPMTobRoICollection_PERS;
typedef T_AthenaPoolCustomCnv<CPMTobRoICollection, CPMTobRoICollection_PERS> CPMTobRoICollectionCnvBase;

class CPMTobRoICollectionCnv: public CPMTobRoICollectionCnvBase {
friend class CnvFactory<CPMTobRoICollectionCnv>;

protected:

  CPMTobRoICollectionCnv (ISvcLocator* svcloc) :
    CPMTobRoICollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CPMTobRoICollectionCnv" )
     {}
  
  ~CPMTobRoICollectionCnv() {}
  
  virtual CPMTobRoICollection_PERS*   createPersistent (CPMTobRoICollection* transCont);
  virtual CPMTobRoICollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CPMTobRoICollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


