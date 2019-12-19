# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# import the JetParticleAssociation configurable (first try - only
JetParticleShrinkingConeAssociation=CompFactory.JetParticleShrinkingConeAssociation

def JetParticleAssociationCfg(ConfigFlags, **options):

    acc=ComponentAccumulator()

    options["coneSizeFitPar1"] = +0.239
    options["coneSizeFitPar2"] = -1.220
    options["coneSizeFitPar3"] = -1.64e-5

    # -- create the association tool
    acc.setPrivateTools(JetParticleShrinkingConeAssociation(**options))
            
    return acc
