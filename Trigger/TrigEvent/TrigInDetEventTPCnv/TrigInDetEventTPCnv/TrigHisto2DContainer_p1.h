/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigHisto2DContainer_p1
 *
 * @brief persistent partner for TrigHisto2DContainer
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGHISTO2DCONTAINER_P1_H
#define TRIGINDETEVENTTPCNV_TRIGHISTO2DCONTAINER_P1_H


#include "AthenaPoolUtilities/TPObjRef.h"  
#include <vector>
  
class TrigHisto2DContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigHisto2DContainer_p1(){}
  friend class TrigHisto2DContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_t2MbtsBits;     

};// end of class definitions
 
 
#endif
