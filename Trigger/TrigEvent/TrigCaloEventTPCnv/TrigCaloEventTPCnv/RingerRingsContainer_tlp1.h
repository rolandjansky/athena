/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : RingerRingsContainer_tlp1
 *
 * @brief "top level" persistent partner for RingerRingsContainer
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * $Id: RingerRingsContainer_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_RINGERRINGSCONTAINER_TLP1_H
#define TRIGCALOEVENTTPCNV_RINGERRINGSCONTAINER_TLP1_H

#include "TrigCaloEventTPCnv/RingerRingsContainer_p1.h"
#include "TrigCaloEventTPCnv/RingerRings_p1.h"

class RingerRingsContainer_tlp1 {

 public:
  RingerRingsContainer_tlp1() { }
  friend class RingerRingsContainerCnv_tlp1;

 private:

  std::vector<RingerRingsContainer_p1> m_RingerRingsContainers;
  std::vector<RingerRings_p1> m_RingerRings;

};
 
#endif

