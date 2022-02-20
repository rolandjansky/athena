/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "TrigParticle/TrigL2Bjet.h"
#include "AthenaKernel/BaseInfo.h"

/**
 * @brief Container of TrigEFBjet objects to be stored in POOL.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 */


//typedef DataVector<TrigL2Bjet> TrigL2BjetContainer;
class TrigL2BjetContainer : public DataVector<TrigL2Bjet> {
public:
  using DataVector::DataVector;  // inherit constructors
};


CLASS_DEF(TrigL2BjetContainer, 1272271823, 1)

SG_BASE(TrigL2BjetContainer, DataVector<TrigL2Bjet>); 

#endif
