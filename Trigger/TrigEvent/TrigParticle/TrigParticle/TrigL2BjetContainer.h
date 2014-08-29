/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigL2BjetContainer.h
// PACKAGE:  Trigger/TrigEvent/TrigParticle
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#ifndef TRIG_L2_BJET_CONTAINER
#define TRIG_L2_BJET_CONTAINER

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "TrigParticle/TrigL2Bjet.h"
#include "SGTools/BaseInfo.h"

/**
 * @brief Container of TrigEFBjet objects to be stored in POOL.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 */


//typedef DataVector<TrigL2Bjet> TrigL2BjetContainer;
class TrigL2BjetContainer : public DataVector<TrigL2Bjet> { }; 


CLASS_DEF(TrigL2BjetContainer, 1272271823, 1)

SG_BASE(TrigL2BjetContainer, DataVector<TrigL2Bjet>); 

#endif
