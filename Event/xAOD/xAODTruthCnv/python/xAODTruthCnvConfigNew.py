# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def GEN_EVNT2xAODCfg(flags, name="GEN_EVNT2xAOD", **kwargs):
    """Configure the EVNT->xAOD converter alg"""
    # Note that since this is usually used by the derivation framework
    # which handles the output stream itself (to allow the expansion to AuxDyn and slimming)
    # nothing is done with the stream here - this is done in the DF job options
    acc = ComponentAccumulator()
    xAODTruthCnvAlg = CompFactory.xAODMaker.xAODTruthCnvAlg
    acc.addEventAlgo(xAODTruthCnvAlg(name,**kwargs))
    return acc

def GEN_AOD2xAODCfg(flags, name="GEN_AOD2xAOD", **kwargs):
    acc = ComponentAccumulator()

    writeInTimePileUpTruth=False

    # Use digiSteeringConf from metadata to check whether full-PU
    # truth should be written.
    if flags.Digitization.DigiSteeringConf in ['StandardPileUpToolsAlg', 'StandardInTimeOnlyTruthPileUpToolsAlg', 'StandardInTimeOnlyGeantinoTruthPileUpToolsAlg']:
        writeInTimePileUpTruth = True

    kwargs.setdefault('WriteInTimePileUpTruth',  writeInTimePileUpTruth)
    kwargs.setdefault('AODContainerName', 'TruthEvent')

    algo = CompFactory.xAODMaker.xAODTruthCnvAlg(name, **kwargs)
    acc.addEventAlgo(algo, primary = True)

    from OutputStreamAthenaPool.OutputStreamConfig import addToESD,addToAOD
    toAOD = ["xAOD::TruthEventContainer#TruthEvents", "xAOD::TruthEventAuxContainer#TruthEventsAux.",
             "xAOD::TruthVertexContainer#TruthVertices", "xAOD::TruthVertexAuxContainer#TruthVerticesAux.",
             "xAOD::TruthParticleContainer#TruthParticles", "xAOD::TruthParticleAuxContainer#TruthParticlesAux.",
    ]

    if writeInTimePileUpTruth:
        toAOD.append("xAOD::TruthPileupEventContainer#TruthPileupEvents")
        toAOD.append("xAOD::TruthPileupEventAuxContainer#TruthPileupEventsAux.")
    toESD = []

    if flags.Output.doWriteESD:
        acc.merge(addToESD(flags, toAOD+toESD))
    if flags.Output.doWriteAOD:
        acc.merge(addToAOD(flags, toAOD))

    return acc
