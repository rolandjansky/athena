/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloTTOnAttrIdMapCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CaloTTOnAttrIdMapCnv_H
#define CaloTTOnAttrIdMapCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "CaloTriggerTool/CaloTTOnAttrIdMap.h"

typedef T_AthenaPoolCustCnv<CaloTTOnAttrIdMap, CaloTTOnAttrId_P> CaloTTOnAttrIdMapCnvBase;

/**
 ** Create derived converter to customize the saving of MyClass
 **/
class CaloTTOnAttrIdMapCnv : public CaloTTOnAttrIdMapCnvBase
{
   friend class CnvFactory<CaloTTOnAttrIdMapCnv>;
public:
   CaloTTOnAttrIdMapCnv(ISvcLocator* svcloc);
   virtual ~CaloTTOnAttrIdMapCnv();

   /// initialization
   virtual StatusCode initialize();

   /// Extend base-class conversion method
   virtual StatusCode transToPers(CaloTTOnAttrIdMap* obj, CaloTTOnAttrId_P*& persObj);
   virtual StatusCode persToTrans(CaloTTOnAttrIdMap*& transObj, CaloTTOnAttrId_P* obj);

};

#endif

