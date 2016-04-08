/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloTTOnOffIdMapCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CaloTTOnOffIdMapCnv_H
#define CaloTTOnOffIdMapCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "CaloTriggerTool/CaloTTOnOffIdMap.h"

typedef T_AthenaPoolCustCnv<CaloTTOnOffIdMap, CaloTTOnOffId_P> CaloTTOnOffIdMapCnvBase;

/**
 ** Create derived converter to customize the saving of MyClass
 **/
class CaloTTOnOffIdMapCnv : public CaloTTOnOffIdMapCnvBase
{
   friend class CnvFactory<CaloTTOnOffIdMapCnv>;
public:
   CaloTTOnOffIdMapCnv(ISvcLocator* svcloc);
   virtual ~CaloTTOnOffIdMapCnv();

   /// initialization
   virtual StatusCode initialize();

   /// Extend base-class conversion method
   virtual StatusCode transToPers(CaloTTOnOffIdMap* obj, CaloTTOnOffId_P*& persObj);
   virtual StatusCode persToTrans(CaloTTOnOffIdMap*& transObj, CaloTTOnOffId_P* obj);

};

#endif

