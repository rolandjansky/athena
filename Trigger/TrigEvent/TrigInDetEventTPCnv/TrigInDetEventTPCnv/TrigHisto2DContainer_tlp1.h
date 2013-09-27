/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigHisto2DContainer_tlp1
 *
 * @brief "top-level" persistent partner for TrigHisto2DContainer
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGHISTO2DCONTAINER_TLP1_H
#define TRIGINDETEVENTTPCNV_TRIGHISTO2DCONTAINER_TLP1_H

#include "TrigInDetEventTPCnv/TrigHisto2DContainer_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto2D_p1.h"

class TrigHisto2DContainer_tlp1 
{
   
 public:
   
  TrigHisto2DContainer_tlp1(){}
 
  std::vector<TrigHisto2DContainer_p1>       m_trigHisto2DContainerVec;
  std::vector<TrigHisto2D_p1>                m_trigHisto2DVec;
  
};//end of class definitions
 
#endif
