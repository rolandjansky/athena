/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCOND_ALIGNTRKCONTAINER_H
#define DETDESCRCOND_ALIGNTRKCONTAINER_H
// AlignableTransformContainer.h - a container for AlignableTransform objects
// using the CondMultChanCollection to allow storage as a set of POOLrefs
// in conditions database
// Richard Hawkings, 31/8/05

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaPoolUtilities/CondMultChanCollection.h"
#include "DetDescrConditions/AlignableTransform.h"

typedef CondMultChanCollection<AlignableTransform> AlignableTransformContainer;

CLASS_DEF( AlignableTransformContainer , 1170039409 , 1 )

#endif // DETDESCRCOND_ALIGNTRKCONTAINER_H
