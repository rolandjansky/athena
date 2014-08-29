/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "TrigParticle/TrigEFBjet.h"
#include "SGTools/BaseInfo.h"

/**
 * @brief Container of TrigEFBjet objects to be stored in POOL.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 */

class TrigEFBjetContainer: public DataVector<TrigEFBjet> { }; 
//typedef DataVector<TrigEFBjet> TrigEFBjetContainer;

CLASS_DEF(TrigEFBjetContainer, 1142962358, 1)

SG_BASE(TrigEFBjetContainer, DataVector<TrigEFBjet>); 

#endif
