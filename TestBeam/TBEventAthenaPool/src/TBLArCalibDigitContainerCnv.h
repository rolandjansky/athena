/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBLArCalibDigitContainerCnv_H
#define TBLArCalibDigitContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "TBEvent/TBLArCalibDigitContainer.h"

typedef T_AthenaPoolCnv<TBLArCalibDigitContainer> TBLArCalibDigitContainerCnvBase;

class TBLArCalibDigitContainerCnv : public TBLArCalibDigitContainerCnvBase
{
  friend class CnvFactory<TBLArCalibDigitContainerCnv >;
 public:
  TBLArCalibDigitContainerCnv(ISvcLocator* svcloc);
  virtual ~TBLArCalibDigitContainerCnv();

  /// initialization
  virtual StatusCode initialize();

  /// Extend base-class conversion method to modify when reading in
  virtual StatusCode     PoolToDataObject(DataObject*& pObj, const Token* token);

  /// Extend base-class conversion method for writing
  virtual StatusCode     DataObjectToPool(DataObject*  pObj,const std::string &tname);

 private:
  /// For your private attributes

};

#endif


