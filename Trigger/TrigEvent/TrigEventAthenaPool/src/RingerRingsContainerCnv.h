/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_RINGERRINGSCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_RINGERRINGSCONTAINER_CNV_H
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
 
#include "TrigCaloEvent/RingerRingsContainer.h"
#include "TrigCaloEventTPCnv/RingerRingsContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/RingerRingsContainerCnv_p2.h"


typedef RingerRingsContainer_p2 RingerRingsContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<RingerRingsContainer, RingerRingsContainer_PERS> RingerRingsContainerCnvBase;
 
class RingerRingsContainerCnv : public RingerRingsContainerCnvBase {
  
  friend class CnvFactory<RingerRingsContainerCnv>;
 
  protected:
   
public:
  RingerRingsContainerCnv(ISvcLocator *svcloc) : RingerRingsContainerCnvBase(svcloc){}
protected:
 
  virtual RingerRingsContainer_PERS *createPersistent(RingerRingsContainer *transObj);
  virtual RingerRingsContainer *createTransient();

private:
  RingerRingsContainerCnv_tlp1 m_converter_tlp1;
  RingerRingsContainerCnv_p2   m_converter;

};
 
#endif

