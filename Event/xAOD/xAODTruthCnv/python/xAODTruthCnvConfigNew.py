# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def GEN_AOD2xAODCfg(flags, name="GEN_AOD2xAOD", **kwargs):
    acc = ComponentAccumulator()

    writeInTimePileUpTruth=False

    # Use digiSteeringConf from metadata to write full-PU truth
    # Not available yet in metadata

    from PyUtils.MetaReader import read_metadata
    infile = flags.Input.Files[0]
    thisFileMD = read_metadata(infile, None, 'full')
    metadata = thisFileMD[infile]
    digiSteeringConf = metadata['/Digitization/Parameters'].get("digiSteeringConf","")
    if digiSteeringConf == 'StandardInTimeOnlyGeantinoTruthPileUpToolsAlg':
        writeInTimePileUpTruth = True

    kwargs.setdefault('WriteInTimePileUpTruth',  writeInTimePileUpTruth)
    kwargs.setdefault('AODContainerName', 'TruthEvent')

    algo = CompFactory.xAODMaker.xAODTruthCnvAlg(name, **kwargs)
    acc.addEventAlgo(algo, primary = True)

    from OutputStreamAthenaPool.OutputStreamConfig import addToESD,addToAOD
    toAOD = ["xAOD::TruthEventContainer#*", "xAOD::TruthEventAuxContainer#*",
             "xAOD::TruthVertexContainer#*", "xAOD::TruthVertexAuxContainer#*",
             "xAOD::TruthParticleContainer#*", "xAOD::TruthParticleAuxContainer#*"]
    if writeInTimePileUpTruth:
        toAOD.append("xAOD::TruthPileupEventContainer#*")
        toAOD.append("xAOD::TruthPileupEventAuxContainer#*")
    toESD = []

    if flags.Output.doWriteESD:
        acc.merge(addToESD(flags, toAOD+toESD))
    if flags.Output.doWriteAOD:
        acc.merge(addToAOD(flags, toAOD))

    return acc
