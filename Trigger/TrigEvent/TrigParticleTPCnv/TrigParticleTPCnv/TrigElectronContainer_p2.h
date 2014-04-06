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
 * @author Ricardo Goncalo  <r.goncalo@rhul.ac.uk>     - Royal Holloway 
 *
 * File and Version Information:
 * $Id: TrigElectronContainer_p2.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 *
 * Notes: TrigElectronCnv_p2 after evolving TrigElectron to use ElementLinks
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRONCONTAINER_P2_H
#define TRIGPARTICLETPCNV_TRIGELECTRONCONTAINER_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

class TrigElectronContainer_p2 : public std::vector<TPObjRef> 
{

 public:
  TrigElectronContainer_p2() {}

};

#endif
