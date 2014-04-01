/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : RingerRingsContainer_p1
 *
 * @brief persistent partner for RingerRingsContainer
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id: RingerRingsContainer_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_RINGERRINGSCONTAINER_P1_H
#define TRIGCALOEVENTTPCNV_RINGERRINGSCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class RingerRingsContainer_p1 : public std::vector<TPObjRef> {
 public:    
  RingerRingsContainer_p1() { }
};

#endif

