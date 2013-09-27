/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigHisto2DContainerCnv_p1
 *
 * @brief transient persistent converter for TrigHisto2DContainer
 *
 * @author W.Bell@cern.ch  <w.bell@physics.gla.ac.uk>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGHISTO2DCONTAINER_CNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGHISTO2DCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigInDetEvent/TrigHisto2DContainer.h"
#include "TrigInDetEventTPCnv/TrigHisto2DContainer_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto2DCnv_p1.h"
 
class TrigHisto2DContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigHisto2DContainer, TrigHisto2DContainer_p1, TrigHisto2DCnv_p1 >
{
 
 public:
  TrigHisto2DContainerCnv_p1() {}
 
}; //end of class definitions
 


#endif
