"""ComponentAccumulator service configuration for ISF_FastCaloSimServices

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from RngComps.RandomServices import RNG


def FastCaloSimSvcCfg(flags, name="ISF_FastCaloSimSvc", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SimulatorTool", "ISF_FastCaloTool")
    kwargs.setdefault("Identifier", "FastCaloSim")
    acc.addService(CompFactory.ISF.LegacySimSvc(name, **kwargs))
    return acc


def FastCaloSimPileupSvcCfg(flags, name="ISF_FastCaloSimPileupSvc", **kwargs):
    kwargs.setdefault("SimulatorTool", "ISF_FastCaloPileupTool")
    return FastCaloSimSvcCfg(flags, name, **kwargs)


def LegacyAFIIFastCaloSimSvcCfg(flags, name="ISF_LegacyAFIIFastCaloSimSvc", **kwargs):
    kwargs.setdefault("SimulatorTool", "ISF_LegacyAFIIFastCaloTool")
    return FastCaloSimSvcCfg(flags, name, **kwargs)


def FastHitConvAlgFastCaloSimSvcCfg(flags, name="ISF_FastHitConvAlgFastCaloSimSvc",**kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CaloCellMakerTools_release", ["ISF_AddNoiseCellBuilderTool",
                                                     "ISF_CaloCellContainerFCSFinalizerTool"])
    # setup FastCaloSim hit converter and add it to the alg sequence:
    # -> creates HITS from reco cells
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence=AlgSequence()
    from AthenaCommon.CfgGetter import getAlgorithm
    topSequence+=getAlgorithm("ISF_FastHitConvAlg")
    acc.merge(FastCaloSimSvcCfg(flags, name, **kwargs))
    return acc


def FastHitConvAlgLegacyAFIIFastCaloSimSvcCfg(flags, name="ISF_FastHitConvAlgLegacyAFIIFastCaloSimSvc", **kwargs):
    kwargs.setdefault("BatchProcessMcTruth", True)
    return FastHitConvAlgFastCaloSimSvcCfg(flags, name, **kwargs)


def FastCaloSimPileupOTSvcCfg(flags, name="ISF_FastCaloSimPileupOTSvc", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("BatchProcessMcTruth", False)
    kwargs.setdefault("SimulateUndefinedBarcodeParticles", False)
    kwargs.setdefault("Identifier", "FastCaloSim")
    kwargs.setdefault("CaloCellsOutputName", flags.Sim.FastCalo.CaloCellsName + "PileUp")
    kwargs.setdefault("PunchThroughTool", "ISF_PunchThroughTool")
    kwargs.setdefault("DoPunchThroughSimulation", False)
    kwargs.setdefault("PUWeights_lar_bapre", flags.Sim.FastChain.PUWeights_lar_bapre)
    kwargs.setdefault("PUWeights_lar_hec", flags.Sim.FastChain.PUWeights_lar_hec)
    kwargs.setdefault("PUWeights_lar_em", flags.Sim.FastChain.PUWeights_lar_em)
    kwargs.setdefault("PUWeights_tile", flags.Sim.FastChain.PUWeights_tile)
    kwargs.setdefault("CaloCellMakerTools_setup", ["ISF_EmptyCellBuilderTool"])
    kwargs.setdefault("CaloCellMakerTools_simulate", ["ISF_FastShowerCellBuilderTool"])
    kwargs.setdefault("CaloCellMakerTools_release", [#"ISF_AddNoiseCellBuilderTool",
                                                     "ISF_CaloCellContainerFCSFinalizerTool",
                                                     "ISF_FastHitConvertTool"])
    kwargs.setdefault("Extrapolator", "ISF_NITimedExtrapolator")
    # FIXME not migrated. Remove or replace
    # register the FastCaloSim random number streams
    #from G4AtlasApps.SimFlags import simFlags
    #if not simFlags.RandomSeedList.checkForExistingSeed(ISF_FastCaloSimFlags.RandomStreamName()):
        #simFlags.RandomSeedList.addSeed( ISF_FastCaloSimFlags.RandomStreamName(), 98346412, 12461240)
    acc.addService(CompFactory.ISF.FastCaloSimSvcPU(name, **kwargs))
    return acc


def FastCaloSimV2ParamSvcCfg(flags, name="ISF_FastCaloSimV2ParamSvc", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ParamsInputFilename", flags.Sim.FastCaloParamsInputFilename)
    kwargs.setdefault("ParamsInputObject", "SelPDGID")
    acc.addService(CompFactory.ISF.FastCaloSimV2ParamSvc(name, **kwargs))
    return acc


def FastCaloSimSvcV2Cfg(flags, name="ISF_FastCaloSimSvcV2", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SimulatorTool", "ISF_FastCaloSimV2Tool")
    kwargs.setdefault("Identifier", "FastCaloSim")
    acc.addService(CompFactory.ISF.LegacySimSvc(name, **kwargs))
    return acc


def DNNCaloSimSvcCfg(flags, name="ISF_DNNCaloSimSvc", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CaloCellsOutputName", flags.Sim.FastCaloCaloCellsName)
    kwargs.setdefault("CaloCellMakerTools_setup", ["ISF_EmptyCellBuilderTool"])
    kwargs.setdefault("CaloCellMakerTools_release", ["ISF_CaloCellContainerFinalizerTool",
                                                     "ISF_FastHitConvertTool"]) #DR needed ?
    kwargs.setdefault("ParamsInputFilename", flags.Sim.FastCaloParamsInputFilename)
    kwargs.setdefault("FastCaloSimCaloExtrapolation", "FastCaloSimCaloExtrapolation") # tool TODO

    # FIXME not migrated. Remove or replace
    # register the FastCaloSim random number streams
    #from G4AtlasApps.SimFlags import simFlags
    #if not simFlags.RandomSeedList.checkForExistingSeed(ISF_FastCaloSimFlags.RandomStreamName()):
        #simFlags.RandomSeedList.addSeed( ISF_FastCaloSimFlags.RandomStreamName(), 98346412, 12461240)
    #kwargs.setdefault("RandomStream", ISF_FastCaloSimFlags.RandomStreamName())
    acc.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomSvc", acc.getService("AthRNGSvc"))
    acc.addService(CompFactory.ISF.DNNCaloSimSvc(name, **kwargs))
    return acc
