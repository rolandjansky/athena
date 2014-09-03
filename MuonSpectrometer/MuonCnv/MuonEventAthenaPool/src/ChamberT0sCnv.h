/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_CHAMBERT0SCNV_H
#define MUONEVENTATHENAPOOL_CHAMBERT0SCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonChamberT0s/ChamberT0s.h"
#include "MuonEventTPCnv/MuonChamberT0s/ChamberT0s_p1.h"

// the latest persistent representation type of ChamberT0s
typedef  Muon::ChamberT0s_p1  ChamberT0s_PERS;
typedef  T_AthenaPoolCustomCnv<Muon::ChamberT0s, ChamberT0s_PERS >   ChamberT0sCnvBase;

class ChamberT0sCnv : public ChamberT0sCnvBase {
friend class CnvFactory<ChamberT0sCnv >;
protected:
  ChamberT0sCnv (ISvcLocator* svcloc) : ChamberT0sCnvBase(svcloc) {}
  virtual ChamberT0s_PERS*   createPersistent (Muon::ChamberT0s* transObj);
  virtual Muon::ChamberT0s*  createTransient ();
};

#endif
