/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArTTCellMapCnv_H
#define LArTTCellMapCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "CaloTriggerTool/LArTTCellMap.h"

#include <string>

typedef T_AthenaPoolCustCnv<LArTTCellMap, LArTTCell_P> LArTTCellMapCnvBase;

/**
 ** Create derived converter to customize the saving of MyClass
 **/
class LArTTCellMapCnv : public LArTTCellMapCnvBase
{
   friend class CnvFactory<LArTTCellMapCnv>;
public:
   LArTTCellMapCnv(ISvcLocator* svcloc);
   virtual ~LArTTCellMapCnv();

   /// initialization
   virtual StatusCode initialize();

   /// Extend base-class conversion method
   virtual StatusCode transToPers(LArTTCellMap* obj, LArTTCell_P*& persObj);
   virtual StatusCode persToTrans(LArTTCellMap*& transObj, LArTTCell_P* obj);

};

#endif
