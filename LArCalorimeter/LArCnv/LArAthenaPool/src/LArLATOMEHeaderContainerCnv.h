/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*- 
#ifndef LARATHENAPOOL_LARLATOMEHEADERCONTAINERCNV_H
#define LARATHENAPOOL_LARLATOMEHEADERCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "LArRawEvent/LArLATOMEHeaderContainer.h"
#include "LArTPCnv/LArLATOMEHeaderContainer_p1.h"

typedef LArLATOMEHeaderContainer_p1 LArLATOMEHeaderContainerPERS;

typedef T_AthenaPoolCustomCnv<LArLATOMEHeaderContainer,LArLATOMEHeaderContainerPERS> LArLATOMEHeaderContainerCnvBase;

class LArLATOMEHeaderContainerCnv : public LArLATOMEHeaderContainerCnvBase 
{
public:
  LArLATOMEHeaderContainerCnv(ISvcLocator*);
  virtual StatusCode initialize() override;
protected:
  virtual LArLATOMEHeaderContainer* createTransient() override final;
  virtual LArLATOMEHeaderContainerPERS* createPersistent(LArLATOMEHeaderContainer*) override;
private:
  pool::Guid   m_p1_guid;
  
};

#endif
