/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBHitPlaneContCnv_H
#define TBHitPlaneContCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "TBEvent/TBHitPlaneCont.h"

typedef T_AthenaPoolCnv<TBHitPlaneCont> TBHitPlaneContCnvBase;

class TBHitPlaneContCnv : public TBHitPlaneContCnvBase
{
  friend class CnvFactory<TBHitPlaneContCnv >;
 public:
  TBHitPlaneContCnv(ISvcLocator* svcloc);
  virtual ~TBHitPlaneContCnv();

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


