/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBBPCRawContCnv_H
#define TBBPCRawContCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "TBEvent/TBBPCRawCont.h"

typedef T_AthenaPoolCnv<TBBPCRawCont> TBBPCRawContCnvBase;

class TBBPCRawContCnv : public TBBPCRawContCnvBase
{
  friend class CnvFactory<TBBPCRawContCnv >;
 public:
  TBBPCRawContCnv(ISvcLocator* svcloc);
  virtual ~TBBPCRawContCnv();

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


