/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBMWPCRawContCnv_H
#define TBMWPCRawContCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "TBEvent/TBMWPCRawCont.h"

typedef T_AthenaPoolCnv<TBMWPCRawCont> TBMWPCRawContCnvBase;

class TBMWPCRawContCnv : public TBMWPCRawContCnvBase
{
  friend class CnvFactory<TBMWPCRawContCnv >;
 public:
  TBMWPCRawContCnv(ISvcLocator* svcloc);
  virtual ~TBMWPCRawContCnv();

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


