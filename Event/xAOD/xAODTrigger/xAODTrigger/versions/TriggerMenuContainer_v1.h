// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuContainer_v1.h 631658 2014-11-27 18:53:39Z lheinric $
#ifndef XAODTRIGGER_VERSIONS_TRIGGERMENUCONTAINER_v1_H
#define XAODTRIGGER_VERSIONS_TRIGGERMENUCONTAINER_v1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/TriggerMenu_v1.h"

namespace xAOD {
   /// Define the menu container as a simple DataVector
   typedef DataVector< TriggerMenu_v1 > TriggerMenuContainer_v1;
}

#endif // XAODTRIGGER_VERSIONS_TRIGGERMENUCONTAINER_v1_H
