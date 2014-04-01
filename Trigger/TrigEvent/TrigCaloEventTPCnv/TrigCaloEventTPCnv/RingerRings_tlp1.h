/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : RingerRings_tlp1
 *
 * @brief "top level" persistent partner for RingerRings
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id: RingerRings_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_RINGERRINGS_TLP1_H
#define TRIGCALOEVENTTPCNV_RINGERRINGS_TLP1_H

#include "TrigCaloEventTPCnv/RingerRings_p1.h"

class RingerRings_tlp1
{
 public:
  
  RingerRings_tlp1() {}
  friend class RingerRingsCnv_tlp1; 
  
 private:
  
  std::vector<RingerRings_p1> m_ringerRings;

};

#endif
