/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhotonContainer_p1
 *
 * @brief persistent partner for TrigPhotonContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhotonContainer_p1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_P1_H
#define TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigPhotonContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigPhotonContainer_p1(){}
     
};// end of class definitions
 
 
#endif
