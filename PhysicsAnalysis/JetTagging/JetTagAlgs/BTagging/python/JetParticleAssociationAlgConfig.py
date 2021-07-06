# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.JetParticleAssociationConfig import JetParticleAssociationCfg

def JetParticleAssociationAlgCfg(ConfigFlags, JetCollection="", InputParticleCollection="", OutputParticleDecoration="", **options):


    acc=ComponentAccumulator()
    jetcol = JetCollection.replace("Track", "PV0Track")

    # setup the associator
    options['JetContainer'] = jetcol + 'Jets'
    options['Decorators'] = [acc.popToolsAndMerge(JetParticleAssociationCfg(ConfigFlags, jetcol+'Jets', InputParticleCollection, OutputParticleDecoration))]
    options['name'] = (jetcol + "_" + OutputParticleDecoration + "_assoc").lower()
    
    # -- create the association algorithm
    acc.addEventAlgo(CompFactory.JetDecorationAlg(**options))
            
    return acc