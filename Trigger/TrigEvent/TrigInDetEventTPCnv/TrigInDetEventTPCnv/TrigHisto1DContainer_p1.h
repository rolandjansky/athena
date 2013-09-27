/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigHisto1DContainer_p1
 *
 * @brief persistent partner for TrigHisto1DContainer
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGHISTO1DCONTAINER_P1_H
#define TRIGINDETEVENTTPCNV_TRIGHISTO1DCONTAINER_P1_H


#include "AthenaPoolUtilities/TPObjRef.h"  
#include <vector>
  
class TrigHisto1DContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigHisto1DContainer_p1(){}
  friend class TrigHisto1DContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_t2MbtsBits;     

};// end of class definitions
 
 
#endif
