/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETEVENT_MISSINGETCONTAINER_H
#define MISSINGETEVENT_MISSINGETCONTAINER_H

/********************************************************************

NAME:     MissingETContainer.cxx
PACKAGE:  offline/Reconstruction/MissingETEvent

AUTHORS:  P. Loch, S. Resconi, RD Schaffer
CREATED:  Oct 2004

PURPOSE:  Container class for MissingET objects
********************************************************************/
#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "MissingETEvent/MissingET.h"

class MissingETContainer : public DataVector<MissingET> {
public:
    /// Main constructor
    MissingETContainer();

};

CLASS_DEF(MissingETContainer, 1138023102, 1)

// inlines defs

inline
MissingETContainer::MissingETContainer() {}

#endif
