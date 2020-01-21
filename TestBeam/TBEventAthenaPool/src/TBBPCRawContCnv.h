/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  virtual StatusCode initialize() override;

  /// Extend base-class conversion method to modify when reading in
  virtual StatusCode     PoolToDataObject(DataObject*& pObj,
                                          const Token* token,
                                          const std::string& key) override;
 };

#endif


