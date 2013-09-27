/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigHisto1DContainerCnv_p1
 *
 * @brief transient persistent converter for TrigHisto1DContainer
 *
 * @author W.Bell@cern.ch  <w.bell@physics.gla.ac.uk>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGHISTO1DCONTAINER_CNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGHISTO1DCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigInDetEvent/TrigHisto1DContainer.h"
#include "TrigInDetEventTPCnv/TrigHisto1DContainer_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto1DCnv_p1.h"
 
class TrigHisto1DContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigHisto1DContainer, TrigHisto1DContainer_p1, TrigHisto1DCnv_p1 >
{
 
 public:
  TrigHisto1DContainerCnv_p1() {}
 
}; //end of class definitions
 


#endif
