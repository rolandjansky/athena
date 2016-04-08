/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  virtual StatusCode initialize();

  /// Extend base-class conversion method to modify when reading in
  virtual StatusCode     PoolToDataObject(DataObject*& pObj, const Token* token);

  /// Extend base-class conversion method for writing
  virtual StatusCode     DataObjectToPool(DataObject*  pObj,const std::string &tname);

 private:
  /// For your private attributes

};

#endif


