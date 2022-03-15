/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTCluster.h"
#include <AsgMessaging/MessageCheck.h>
using namespace asg::msgUserCode;

void HTTCluster::Print(Option_t*) const
{
  ANA_MSG_INFO("Cluster formed from " << this->getHitList().size() << " hits");
}
