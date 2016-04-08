/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArCellContainerCnv_H
#define LArCellContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "LArRecEvent/LArCellContainer.h"

// We rename generated typedef to <MyClass>CnvBase
typedef T_AthenaPoolCnv<LArCellContainer> LArCellContainerCnvBase;

class CaloDetDescrManager;

/** 
 ** Create derived converter to customize the saving of MyClass
 **/
class LArCellContainerCnv : public LArCellContainerCnvBase
{
   friend class CnvFactory<LArCellContainerCnv >;
public:
    LArCellContainerCnv(ISvcLocator* svcloc);
    virtual ~LArCellContainerCnv();

    /// initialization
    virtual StatusCode initialize();

    /// Extend base-class conversion method to modify when reading in
    virtual StatusCode     PoolToDataObject(DataObject*& pObj, const Token* token);

private:
    /// Save CaloDDMgr for initializing LArCells
    const CaloDetDescrManager* m_detMgr;
};

#endif


