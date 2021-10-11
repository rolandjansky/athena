# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

JetParticleShrinkingConeAssociation=CompFactory.JetParticleShrinkingConeAssociation

# this function is only used below
def JetParticleAssociationCfg(ConfigFlags, jetCollName, partcollname, assocname, **options):

    acc=ComponentAccumulator()

    options["coneSizeFitPar1"] = +0.239
    options["coneSizeFitPar2"] = -1.220
    options["coneSizeFitPar3"] = -1.64e-5
    options["InputParticleContainer"] = partcollname
    options["OutputDecoration"] = assocname

    # -- create the association tool
    acc.setPrivateTools(JetParticleShrinkingConeAssociation(JetContainer=jetCollName, **options))

    return acc

def JetParticleAssociationAlgCfg(ConfigFlags, JetCollection="", InputParticleCollection="", OutputParticleDecoration="", **options):


    acc=ComponentAccumulator()
    jetcol = JetCollection.replace("Track", "PV0Track")

    # setup the associator
    if not jetcol.endswith('Jets'):
        jetcol += 'Jets'
    options['JetContainer'] = jetcol
    options['Decorators'] = [acc.popToolsAndMerge(JetParticleAssociationCfg(ConfigFlags, jetcol, InputParticleCollection, OutputParticleDecoration))]
    options['name'] = (jetcol + "_" + OutputParticleDecoration + "_assoc").lower()

    # -- create the association algorithm
    acc.addEventAlgo(CompFactory.JetDecorationAlg(**options))

    return acc
