/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhotonContainer_p3
 *
 * @brief persistent partner for TrigPhotonContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhotonContainer_p3.h 359048 2011-04-15 12:07:04Z salvator $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_P3_H
#define TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_P3_H

#include "TrigParticleTPCnv/TrigPhoton_p3.h"

class TrigPhotonContainer_p3 : public std::vector<TrigPhoton_p3> {};
// end of class definitions
 
 
#endif
