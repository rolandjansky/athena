/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBScintillatorRawContCnv_H
#define TBScintillatorRawContCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "TBEvent/TBScintillatorRawCont.h"

typedef T_AthenaPoolCnv<TBScintillatorRawCont> TBScintillatorRawContCnvBase;

class TBScintillatorRawContCnv : public TBScintillatorRawContCnvBase
{
  friend class CnvFactory<TBScintillatorRawContCnv >;
 public:
  TBScintillatorRawContCnv(ISvcLocator* svcloc);
  virtual ~TBScintillatorRawContCnv();

  /// initialization
  virtual StatusCode initialize() override;

  /// Extend base-class conversion method to modify when reading in
  virtual StatusCode     PoolToDataObject(DataObject*& pObj,
                                          const Token* token,
                                          const std::string& key) override;
};

#endif


