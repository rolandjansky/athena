/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Persistent representation of BCM_RDO_Container
// Author: Dominique Tardif <Dominique.Tardif@cern.ch>

#ifndef INDETBCM_EVENTATHENAPOOL_BCM_RDO_CONTAINER_P0_H
#define INDETBCM_EVENTATHENAPOOL_BCM_RDO_CONTAINER_P0_H

#include <vector>
#include "InDetBCM_EventAthenaPool/BCM_RDO_Collection_p0.h"

class BCM_RDO_Container_p0 : public std::vector<BCM_RDO_Collection_p0> {
 public:
  BCM_RDO_Container_p0() {}
};

#endif
