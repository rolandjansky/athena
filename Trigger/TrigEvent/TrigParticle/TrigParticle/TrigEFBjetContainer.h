/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigEFBjetContainer.h
// PACKAGE:  Trigger/TrigEvent/TrigParticle
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#ifndef TRIG_EF_BJET_CONTAINER
#define TRIG_EF_BJET_CONTAINER

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "TrigParticle/TrigEFBjet.h"
#include "AthenaKernel/BaseInfo.h"

/**
 * @brief Container of TrigEFBjet objects to be stored in POOL.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 */

//typedef DataVector<TrigEFBjet> TrigEFBjetContainer;
class TrigEFBjetContainer: public DataVector<TrigEFBjet> {
public:
  using DataVector::DataVector;  // inherit constructors
}; 


CLASS_DEF(TrigEFBjetContainer, 1142962358, 1)

SG_BASE(TrigEFBjetContainer, DataVector<TrigEFBjet>); 

#endif
