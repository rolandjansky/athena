/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloTTPpmRxIdMapCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CaloTTPpmRxIdMapCnv_H
#define CaloTTPpmRxIdMapCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "CaloTriggerTool/CaloTTPpmRxIdMap.h"

typedef T_AthenaPoolCustCnv<CaloTTPpmRxIdMap, CaloTTPpmRxId_P> CaloTTPpmRxIdMapCnvBase;

/**
 ** Create derived converter to customize the saving of MyClass
 **/
class CaloTTPpmRxIdMapCnv : public CaloTTPpmRxIdMapCnvBase
{
   friend class CnvFactory<CaloTTPpmRxIdMapCnv>;
public:
   CaloTTPpmRxIdMapCnv(ISvcLocator* svcloc);
   virtual ~CaloTTPpmRxIdMapCnv();

   /// initialization
   virtual StatusCode initialize();

   /// Extend base-class conversion method
   virtual StatusCode transToPers(CaloTTPpmRxIdMap* obj, CaloTTPpmRxId_P*& persObj);
   virtual StatusCode persToTrans(CaloTTPpmRxIdMap*& transObj, CaloTTPpmRxId_P* obj);

};

#endif

