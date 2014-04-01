/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2MbtsBitsContainer_p1
 *
 * @brief persistent partner for TrigT2MbtsBitsContainer
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2MBTSBITSCONTAINER_P1_H
#define TRIGCALOEVENTTPCNV_TRIGT2MBTSBITSCONTAINER_P1_H


#include "AthenaPoolUtilities/TPObjRef.h"  
#include <vector>
  
class TrigT2MbtsBitsContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigT2MbtsBitsContainer_p1(){}
  friend class TrigT2MbtsBitsContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_t2MbtsBits;     

};// end of class definitions
 
 
#endif
