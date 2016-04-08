/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_PARTICLECLUSTERASSOCIATIONCOLLECTION_H
#define REC_PARTICLECLUSTERASSOCIATIONCOLLECTION_H

#include "ParticleCaloExtension/ParticleClusterAssociation.h"
#include "xAODCore/CLASS_DEF.h"
#include "AthContainers/DataVector.h"

typedef DataVector<Rec::ParticleClusterAssociation> ParticleClusterAssociationCollection;
CLASS_DEF(ParticleClusterAssociationCollection, 1234352061 , 1 )

#endif
