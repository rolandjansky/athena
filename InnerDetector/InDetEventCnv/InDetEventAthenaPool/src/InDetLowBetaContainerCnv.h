/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_INDETLOWBETACONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_INDETLOWBETACONTAINER_CNV_H
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
 
#include "InDetLowBetaInfo/InDetLowBetaContainer.h"

class InDetLowBetaContainer_tlp1;
class InDetLowBetaContainerCnv_tlp1;
 
//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef InDetLowBetaContainer_tlp1 InDetLowBetaContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<InDet::InDetLowBetaContainer, InDetLowBetaContainer_PERS> InDetLowBetaContainerCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for InDetLowBetaContainer object
//-----------------------------------------------------------------------------
class InDetLowBetaContainerCnv : public InDetLowBetaContainerCnvBase {
  
  friend class CnvFactory<InDetLowBetaContainerCnv>;
 
  protected:
   
  InDetLowBetaContainerCnv(ISvcLocator *svcloc);
  ~InDetLowBetaContainerCnv();
 
  virtual InDetLowBetaContainer_PERS *createPersistent(InDet::InDetLowBetaContainer *transObj);
  virtual InDet::InDetLowBetaContainer *createTransient();
 
  virtual AthenaPoolTopLevelTPCnvBase *getTopLevelTPCnv();
 
  private:
 
  InDetLowBetaContainerCnv_tlp1* m_TPConverter_tlp1;
 
};
 
#endif
