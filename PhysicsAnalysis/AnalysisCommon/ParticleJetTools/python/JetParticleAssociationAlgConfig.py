# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# this function is only used below
def JetParticleAssociationCfg(ConfigFlags, jetCollName, partcollname, assocname, **options):

    acc=ComponentAccumulator()

    options["coneSizeFitPar1"] = +0.239
    options["coneSizeFitPar2"] = -1.220
    options["coneSizeFitPar3"] = -1.64e-5
    options["InputParticleContainer"] = partcollname
    options["OutputDecoration"] = assocname

    # -- create the association tool
    acc.setPrivateTools(
        CompFactory.JetParticleShrinkingConeAssociation(
            JetContainer=jetCollName, **options))

    return acc


def JetParticleAssociationAlgCfg(ConfigFlags, JetCollection="", InputParticleCollection="", OutputParticleDecoration="", MinimumJetPt=0.0):

    acc=ComponentAccumulator()
    jetcol = JetCollection.replace("Track", "PV0Track")
    name=(jetcol + "_" + OutputParticleDecoration + "_assoc").lower()
    if MinimumJetPt > 0.0:
        ptflag = f'{OutputParticleDecoration}OverPtThreshold'
    else:
        ptflag = ''

    # -- create the association algorithm
    acc.addEventAlgo(CompFactory.JetDecorationAlg(
        name=name,
        JetContainer=jetcol,
        Decorators=[
            acc.popToolsAndMerge(
                JetParticleAssociationCfg(
                    ConfigFlags,
                    jetcol,
                    InputParticleCollection,
                    OutputParticleDecoration,
                    MinimumJetPt=MinimumJetPt,
                    PassPtFlag=ptflag,
                ))
        ]
    ))

    return acc
