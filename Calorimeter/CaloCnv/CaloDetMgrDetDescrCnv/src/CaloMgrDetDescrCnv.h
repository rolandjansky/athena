/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALODETMGRDETDESCRCNV_CALODETMGRDETDESCRCNV_H
#define CALODETMGRDETDESCRCNV_CALODETMGRDETDESCRCNV_H

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "AthenaBaseComps/AthMessaging.h"

class CaloMgrDetDescrCnv: public DetDescrConverter , public AthMessaging 
{
 public:
  CaloMgrDetDescrCnv(ISvcLocator* svcloc);

  virtual long int   repSvcType() const override;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;
  
  // Storage type and class ID (used by CnvFactory)
  static long storageType();
  static const CLID& classID();

 private:

};

#endif
