// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: egammaRecContainer.h 586152 2014-03-04 14:12:57Z christos $
#ifndef EGAMMARECCONTAINER
#define EGAMMARECCONTAINER

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "egammaRec.h"
#include "SGTools/CLASS_DEF.h"

/// The container is a simple typedef for now

typedef DataVector< egammaRec > EgammaRecContainer;
 
CLASS_DEF( EgammaRecContainer , 1265838377 , 1 )
#endif 
