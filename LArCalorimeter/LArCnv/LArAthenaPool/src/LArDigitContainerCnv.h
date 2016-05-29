/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*- 
#ifndef LARATHENAPOOL_LARDIGITCONTAINERCNV_H
#define LARATHENAPOOL_LARDIGITCONTAINERCNV_H

#include "LArTPCnv/LArDigitContainerCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "LArRawEvent/LArDigitContainer.h"

//class LArDigitContainer;
//class LArDigitContainer_p1;

typedef LArDigitContainer_p1 LArDigitContainerPERS;

typedef T_AthenaPoolCustomCnv<LArDigitContainer,LArDigitContainerPERS> LArDigitContainerCnvBase;

class LArDigitContainerCnv : public LArDigitContainerCnvBase 
{
  friend class CnvFactory<LArDigitContainerCnv>;
 protected:
  LArDigitContainerCnv(ISvcLocator*);
  virtual LArDigitContainer* createTransient();
  virtual LArDigitContainerPERS* createPersistent(LArDigitContainer*);
 private:
  LArDigitContainerCnv_p1 m_converter;
  pool::Guid   m_p0_guid;
  pool::Guid   m_p1_guid;

};

#endif
