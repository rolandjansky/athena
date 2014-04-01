/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : RingerRingsContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for RingerRingsContainer
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * $Id: RingerRingsContainerCnv_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGCALOEVENTTPCNV_RINGERRINGSCONTAINER_CNV_TLP1_H
#define  TRIGCALOEVENTTPCNV_RINGERRINGSCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEventTPCnv/RingerRingsContainer_tlp1.h"

#include "TrigCaloEventTPCnv/RingerRingsContainerCnv_p1.h"
#include "TrigCaloEventTPCnv/RingerRingsCnv_p1.h" 
 
class RingerRingsContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter<RingerRingsContainerCnv_p1, RingerRingsContainer_tlp1> {

 public:

  RingerRingsContainerCnv_tlp1();
  virtual ~RingerRingsContainerCnv_tlp1() { }
 
  // default methods  
  virtual void setPStorage(RingerRingsContainer_tlp1 *storage);
  
 protected:
  RingerRingsContainerCnv_p1 m_ringerRingsContCnv;
  RingerRingsCnv_p1          m_ringerRingsCnv;
 
};

#endif

