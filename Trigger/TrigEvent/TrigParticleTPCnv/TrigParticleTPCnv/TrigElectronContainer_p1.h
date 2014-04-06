/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigElectronContainer_p1
 *
 * @brief persistent partner for TrigElectronContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigElectronContainer_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRONCONTAINER_P1_H
#define TRIGPARTICLETPCNV_TRIGELECTRONCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

class TrigElectronContainer_p1 : public std::vector<TPObjRef> 
{

 public:
  TrigElectronContainer_p1() {}

};

#endif
