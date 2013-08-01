/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECATHENAPOOL_MISSINGETCOMPOSITIONCNV_H
#define RECATHENAPOOL_MISSINGETCOMPOSITIONCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// the converter and the persistent class
#include "RecTPCnv/MissingETCompositionCnv_p2.h"
#include "RecTPCnv/MissingETComposition_p2.h"

// the transient class
#include "MissingETEvent/MissingETComposition.h"

// the latest persistent representation type of MissingETComposition_p2
typedef  MissingETComposition_p2  MissingETComposition_PERS;
typedef  T_AthenaPoolCustomCnv<MissingETComposition, MissingETComposition_PERS>  MissingETCompositionCnvBase;

class MissingETCompositionCnv : public MissingETCompositionCnvBase
{
  friend class CnvFactory<MissingETCompositionCnv>;
  
 protected:

  MissingETCompositionCnv (ISvcLocator* svcloc) : MissingETCompositionCnvBase(svcloc)
    , m_TPconverter_p2(0)
    { };

  virtual ~MissingETCompositionCnv() { delete m_TPconverter_p2; };

  virtual MissingETComposition_PERS*   createPersistent (MissingETComposition* transObj);
  virtual MissingETComposition*        createTransient ();
  
 private:
  MissingETCompositionCnv_p2*   m_TPconverter_p2;
};
#endif
