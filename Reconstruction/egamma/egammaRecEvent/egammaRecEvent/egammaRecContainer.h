// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: egammaRecContainer.h 666999 2015-05-13 18:20:00Z christos $
#ifndef EGAMMARECCONTAINER
#define EGAMMARECCONTAINER

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "egammaRec.h"

/// The container is a simple typedef for now

typedef DataVector< egammaRec > EgammaRecContainer;

#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( EgammaRecContainer , 1265838377 , 1 )
#endif // XAOD_STANDALONE
 
#endif 
