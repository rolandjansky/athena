///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAVFOURMOM_IPARTICLECONTAINER_H
#define NAVFOURMOM_IPARTICLECONTAINER_H
/********************************************************************

NAME:     IParticleContainer.h
Package : offline/Event/EventKernel

AUTHORS:  K. Cranmer
CREATED:  June 2004

PURPOSE:  This is a data object, containing a collection of IParticle Objects.
********************************************************************/

// INCLUDE HEADER FILES:

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "EventKernel/IParticle.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

DATAVECTOR_VIRTBASES1 (IParticle, INavigable4Momentum);

typedef DataVector<IParticle>  IParticleContainer;

// re-use the 'old' way of declaring classes to the ClassIDSvc
CLASS_DEF(IParticleContainer, 1088157450, 1)

#endif // NAVFOURMOM_IPARTICLECONTAINER_H

