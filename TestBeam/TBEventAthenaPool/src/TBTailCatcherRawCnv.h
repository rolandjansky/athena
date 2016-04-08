/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBTailCatcherRawCnv_H
#define TBTailCatcherRawCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "TBEvent/TBTailCatcherRaw.h"

typedef T_AthenaPoolCnv<TBTailCatcherRaw> TBTailCatcherRawCnvBase;

class TBTailCatcherRawCnv : public TBTailCatcherRawCnvBase
{
  friend class CnvFactory<TBTailCatcherRawCnv >;
 public:
  TBTailCatcherRawCnv(ISvcLocator* svcloc);
  virtual ~TBTailCatcherRawCnv();

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


