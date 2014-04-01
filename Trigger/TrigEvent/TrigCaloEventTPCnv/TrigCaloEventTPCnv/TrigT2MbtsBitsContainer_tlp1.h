/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2MbtsBitsContainer_tlp1
 *
 * @brief "top-level" persistent partner for TrigT2MbtsBitsContainer
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2MBTSBITSCONTAINER_TLP1_H
#define TRIGCALOEVENTTPCNV_TRIGT2MBTSBITSCONTAINER_TLP1_H

#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p1.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p2.h"

class TrigT2MbtsBitsContainer_tlp1 
{
   
 public:
   
  TrigT2MbtsBitsContainer_tlp1(){}
 
  std::vector<TrigT2MbtsBitsContainer_p1>       m_trigT2MbtsBitsContainerVec;
  std::vector<TrigT2MbtsBits_p1>                m_trigT2MbtsBitsVec;
  std::vector<TrigT2MbtsBits_p2>                m_trigT2MbtsBitsVec_p2;
  
};//end of class definitions
 
#endif
