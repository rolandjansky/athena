/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2MbtsBitsContainerCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TrigT2MbtsBitsContainer_tlp1
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef  TRIGCALOEVENTTPCNV_TRIGT2MBTSBITSCONTAINER_CNV_TLP1_H
#define  TRIGCALOEVENTTPCNV_TRIGT2MBTSBITSCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainer_tlp1.h"
 
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainerCnv_p1.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsCnv_p1.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsCnv_p2.h"
  
 
class TrigT2MbtsBitsContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigT2MbtsBitsContainerCnv_p1, TrigT2MbtsBitsContainer_tlp1 >
{
 
 public:
 
  TrigT2MbtsBitsContainerCnv_tlp1();
  virtual ~TrigT2MbtsBitsContainerCnv_tlp1() {}
  
  //default methods  
  virtual void  setPStorage( TrigT2MbtsBitsContainer_tlp1 *storage );
  
 protected: 
  TrigT2MbtsBitsContainerCnv_p1    m_trigT2MbtsBitsContainerCnv;
  TrigT2MbtsBitsCnv_p1             m_trigT2MbtsBitsCnv_p1;
  TrigT2MbtsBitsCnv_p2             m_trigT2MbtsBitsCnv_p2;
 
}; //end of class definitions

#endif
