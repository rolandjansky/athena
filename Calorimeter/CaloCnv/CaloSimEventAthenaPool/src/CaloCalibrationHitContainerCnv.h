/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCALIBRATIONHITCONTAINERCNV
#define CALOCALIBRATIONHITCONTAINERCNV

#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloSimEventTPCnv/CaloCalibrationHitContainer_p1.h"
#include "CaloSimEventTPCnv/CaloCalibrationHitContainer_p2.h"
#include "CaloSimEventTPCnv/CaloCalibrationHitContainer_p3.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent version
typedef CaloCalibrationHitContainer_p3  CaloCalibrationHitContainer_PERS;

class CaloCalibrationHitContainerCnv  : public T_AthenaPoolCustomCnv<CaloCalibrationHitContainer, CaloCalibrationHitContainer_PERS > {
  friend class CnvFactory<CaloCalibrationHitContainerCnv>;
protected:
  CaloCalibrationHitContainerCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<CaloCalibrationHitContainer, CaloCalibrationHitContainer_PERS >( svcloc) {}
  CaloCalibrationHitContainer_PERS*  createPersistent(CaloCalibrationHitContainer* transCont);
  CaloCalibrationHitContainer*       createTransient ();
};

#endif
