/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArCalibParamsCnv.h
 * @brief Customized conversion for LArCalibParams
 * @author Walter Lampl <walter.lampl@cern.ch>
 */

#ifndef LArCalibParamsCnv_H
#define LArCalibParamsCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "LArRawConditions/LArCalibParams.h"

typedef T_AthenaPoolCnv<LArCalibParams> LArCalibParamsCnvBase;

//class LArCalibParamsCnv;

class LArCalibParamsCnv : public LArCalibParamsCnvBase
{
  friend class CnvFactory<LArCalibParamsCnv>;
 public:
  LArCalibParamsCnv(ISvcLocator* svcloc);
  virtual ~LArCalibParamsCnv();

  //Overload PoolToDataObject with custom implementation
  virtual StatusCode PoolToDataObject(DataObject*& pObj, const Token* token);
};


#endif

