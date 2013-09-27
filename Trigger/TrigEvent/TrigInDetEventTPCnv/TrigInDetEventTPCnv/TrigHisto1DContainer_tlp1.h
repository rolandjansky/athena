/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigHisto1DContainer_tlp1
 *
 * @brief "top-level" persistent partner for TrigHisto1DContainer
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGHISTO1DCONTAINER_TLP1_H
#define TRIGINDETEVENTTPCNV_TRIGHISTO1DCONTAINER_TLP1_H

#include "TrigInDetEventTPCnv/TrigHisto1DContainer_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto1D_p1.h"

class TrigHisto1DContainer_tlp1 
{
   
 public:
   
  TrigHisto1DContainer_tlp1(){}
 
  std::vector<TrigHisto1DContainer_p1>       m_trigHisto1DContainerVec;
  std::vector<TrigHisto1D_p1>                m_trigHisto1DVec;
  
};//end of class definitions
 
#endif
