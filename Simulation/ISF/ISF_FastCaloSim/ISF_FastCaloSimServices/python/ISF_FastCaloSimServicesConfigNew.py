"""ComponentAccumulator service configuration for ISF_FastCaloSimServices

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from RngComps.RandomServices import AthRNGSvcCfg
from ISF_Services.ISF_ServicesConfigNew import TruthServiceCfg

###################################################################################################
# Moved from AdditionalConfig

from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConfigNew import FastCaloSimCaloExtrapolationCfg
from FastCaloSim.FastCaloSimFactoryNew import (NITimedExtrapolatorCfg,
                                               FastShowerCellBuilderToolBaseCfg)


def PunchThroughToolCfg(flags, name="ISF_PunchThroughTool", **kwargs):

    from BarcodeServices.BarcodeServicesConfigNew import BarcodeSvcCfg
    from SubDetectorEnvelopes.SubDetectorEnvelopesConfigNew import EnvelopeDefSvcCfg
    acc = ComponentAccumulator()
    kwargs.setdefault("FilenameLookupTable", "FastCaloSim/MC16/TFCSparam_mpt_v01.root")
    kwargs.setdefault("PunchThroughInitiators", [211])
    kwargs.setdefault("InitiatorsMinEnergy"     , [ 65536 ]                                         )
    kwargs.setdefault("InitiatorsEtaRange"      , [       -2.7,     2.7 ]                               )
    kwargs.setdefault("PunchThroughParticles", [   2212,     211,      22,      11,      13])
    kwargs.setdefault("DoAntiParticles"      , [  False,    True,   False,    True,    True])
    kwargs.setdefault("CorrelatedParticle"      , []    )
    kwargs.setdefault("FullCorrelationEnergy", [100000., 100000., 100000., 100000.,      0.])
    kwargs.setdefault("MinEnergy"            , [  938.3,   135.6,     50.,     50.,   105.7])
    kwargs.setdefault("MaxNumParticles"      , [     -1,      -1,      -1,      -1,      -1])
    kwargs.setdefault("EnergyFactor"         , [     1.,      1.,      1.,      1.,      1.])
    kwargs.setdefault("BarcodeSvc", acc.getPrimaryAndMerge(BarcodeSvcCfg(flags)).name)
    kwargs.setdefault("EnvelopeDefSvc", acc.getPrimaryAndMerge(EnvelopeDefSvcCfg(flags)).name)
    kwargs.setdefault("BeamPipeRadius", 500.)
    acc.setPrivateTools(CompFactory.ISF.PunchThroughTool(name, **kwargs))
    return acc


def EmptyCellBuilderToolCfg(flags, name="ISF_EmptyCellBuilderTool", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.EmptyCellBuilderTool(name, **kwargs))
    return acc


def LegacyFastShowerCellBuilderToolCfg(flags, name="ISF_LegacyFastShowerCellBuilderTool", **kwargs):
    kwargs.setdefault("Invisibles", [0, 13])
    return FastShowerCellBuilderToolBaseCfg(flags, name, **kwargs)


def PileupFastShowerCellBuilderToolCfg(flags, name="ISF_PileupFastShowerCellBuilderTool", **kwargs):
    # weights from:
    # https://acode-browser.usatlas.bnl.gov/lxr/source/athena/Simulation/FastShower/FastCaloSim/FastCaloSim/FastCaloSim_CaloCell_ID.h
    weightsfcs = [
        ### LAr presampler
        #FirstSample=CaloCell_ID::PreSamplerB,
        2.0,
        ### LAr barrel
        #PreSamplerB=CaloCell_ID::PreSamplerB,
        #EMB1=CaloCell_ID::EMB1,
        #EMB2=CaloCell_ID::EMB2,
        #EMB3=CaloCell_ID::EMB3,
        2.0, 2.0, 2.0, 2.0,
        ### LAr EM endcap
        #PreSamplerE=CaloCell_ID::PreSamplerE,
        #EME1=CaloCell_ID::EME1,
        #EME2=CaloCell_ID::EME2,
        #EME3=CaloCell_ID::EME3,
        2.0, 2.0, 2.0, 2.0,
        ### Hadronic end cap cal.
        #HEC0=CaloCell_ID::HEC0,
        #HEC1=CaloCell_ID::HEC1,
        #HEC2=CaloCell_ID::HEC2,
        #HEC3=CaloCell_ID::HEC3,
        2.0, 2.0, 2.0, 2.0,
        ### Tile barrel
        #TileBar0=CaloCell_ID::TileBar0,
        #TileBar1=CaloCell_ID::TileBar1,
        #TileBar2=CaloCell_ID::TileBar2,
        1.0, 1.0, 1.0,
        ### Tile gap (ITC & scint)
        #TileGap1=CaloCell_ID::TileGap1,
        #TileGap2=CaloCell_ID::TileGap2,
        #TileGap3=CaloCell_ID::TileGap3,
        1.0, 1.0, 1.0,
        ### Tile extended barrel
        #TileExt0=CaloCell_ID::TileExt0,
        #TileExt1=CaloCell_ID::TileExt1,
        #TileExt2=CaloCell_ID::TileExt2,
        1.0, 1.0, 1.0,
        ### Forward EM endcap
        #FCAL0=CaloCell_ID::FCAL0,
        #FCAL1=CaloCell_ID::FCAL1,
        #FCAL2=CaloCell_ID::FCAL2,
        1.0, 1.0, 1.0,
        ### Beware of MiniFCAL! We don"t have it, so different numbers after FCAL2
        #LastSample = CaloCell_ID::FCAL2,
        #MaxSample = LastSample+1
        1.0, 1.0,
    ]

    kwargs.setdefault("sampling_energy_reweighting", weightsfcs )
    return FastShowerCellBuilderToolBaseCfg(flags, name, **kwargs)


def FastHitConvertToolCfg(flags, name="ISF_FastHitConvertTool", **kwargs):
    from ISF_Algorithms.CollectionMergerConfig import CollectionMergerCfg

    acc = ComponentAccumulator()
    mergeable_collection_suffix = "_FastCaloSim"
    region = "CALO"

    EMB_hits_bare_collection_name = "LArHitEMB"
    EMB_hits_merger_input_property = "LArEMBHits"
    acc1, EMB_hits_collection_name = CollectionMergerCfg(
        flags,
        EMB_hits_bare_collection_name,
        mergeable_collection_suffix,
        EMB_hits_merger_input_property,
        region)
    acc.merge(acc1)

    EMEC_hits_bare_collection_name = "LArHitEMEC"
    EMEC_hits_merger_input_property = "LArEMECHits"
    acc2, EMEC_hits_collection_name = CollectionMergerCfg(
        flags,
        EMEC_hits_bare_collection_name,
        mergeable_collection_suffix,
        EMEC_hits_merger_input_property,
        region)
    acc.merge(acc2)

    FCAL_hits_bare_collection_name = "LArHitFCAL"
    FCAL_hits_merger_input_property = "LArFCALHits"
    acc3, FCAL_hits_collection_name = CollectionMergerCfg(
        flags,
        FCAL_hits_bare_collection_name,
        mergeable_collection_suffix,
        FCAL_hits_merger_input_property,
        region)
    acc.merge(acc3)

    HEC_hits_bare_collection_name = "LArHitHEC"
    HEC_hits_merger_input_property = "LArHECHits"
    acc4, HEC_hits_collection_name = CollectionMergerCfg(
        flags,
        HEC_hits_bare_collection_name,
        mergeable_collection_suffix,
        HEC_hits_merger_input_property,
        region)
    acc.merge(acc4)

    tile_hits_bare_collection_name = "TileHitVec"
    tile_hits_merger_input_property = "TileHits"
    acc5, tile_hits_collection_name = CollectionMergerCfg(
        flags,
        tile_hits_bare_collection_name,
        mergeable_collection_suffix,
        tile_hits_merger_input_property,
        region)
    acc.merge(acc5)

    kwargs.setdefault("embHitContainername", EMB_hits_collection_name)
    kwargs.setdefault("emecHitContainername", EMEC_hits_collection_name)
    kwargs.setdefault("fcalHitContainername", FCAL_hits_collection_name)
    kwargs.setdefault("hecHitContainername", HEC_hits_collection_name)

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge(TileCablingSvcCfg(flags))

    from TileConditions.TileSamplingFractionConfig import TileSamplingFractionCondAlgCfg
    acc.merge( TileSamplingFractionCondAlgCfg(flags) )

    kwargs.setdefault("tileHitContainername", tile_hits_collection_name)

    acc.setPrivateTools(CompFactory.FastHitConvertTool(name, **kwargs))
    return acc


def AddNoiseCellBuilderToolCfg(flags, name="ISF_AddNoiseCellBuilderTool", **kwargs):
    from FastCaloSim.AddNoiseCellBuilderToolConfig import AddNoiseCellBuilderToolCfg
    return AddNoiseCellBuilderToolCfg(flags)


def CaloCellContainerFinalizerToolCfg(flags, name="ISF_CaloCellContainerFinalizerTool", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.CaloCellContainerFinalizerTool(name, **kwargs))
    return acc


def CaloCellContainerFCSFinalizerToolCfg(flags, name="ISF_CaloCellContainerFCSFinalizerTool", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.CaloCellContainerFCSFinalizerTool(name, **kwargs))
    return acc


def FastHitConvAlgCfg(flags, name="ISF_FastHitConvAlg", **kwargs):
    acc = ComponentAccumulator()

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge(TileCablingSvcCfg(flags))

    from TileConditions.TileSamplingFractionConfig import TileSamplingFractionCondAlgCfg
    acc.merge( TileSamplingFractionCondAlgCfg(flags) )

    kwargs.setdefault("CaloCellsInputName"  , flags.Sim.FastCalo.CaloCellsName)
    acc.addEventAlgo(CompFactory.FastHitConv(name, **kwargs))
    return acc


def FastCaloToolBaseCfg(flags, name="ISF_FastCaloTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("BatchProcessMcTruth"              , False)
    kwargs.setdefault("SimulateUndefinedBarcodeParticles", False)
    kwargs.setdefault("CaloCellsOutputName"              , flags.Sim.FastCalo.CaloCellsName)
    if "CaloCellMakerTools_setup" not in kwargs:
        EmptyCellBuilder = acc.popToolsAndMerge(EmptyCellBuilderToolCfg(flags))
        acc.addPublicTool(EmptyCellBuilder)
    kwargs.setdefault("CaloCellMakerTools_setup"         , [acc.getPublicTool(EmptyCellBuilder.name)])
    if "CaloCellMakerTools_simulate" not in kwargs:
        FastShowerCell_tool = acc.popToolsAndMerge(FastShowerCellBuilderToolBaseCfg(flags))
        acc.addPublicTool(FastShowerCell_tool)
        kwargs.setdefault("CaloCellMakerTools_simulate"      , [acc.getPublicTool(FastShowerCell_tool.name)])
    if "CaloCellMakerTools_release" not in kwargs:
        CaloCellContainer = acc.popToolsAndMerge(CaloCellContainerFinalizerToolCfg(flags))
        acc.addPublicTool(CaloCellContainer)
        FastHit = acc.popToolsAndMerge(FastHitConvertToolCfg(flags))
        acc.addPublicTool(FastHit)
        kwargs.setdefault("CaloCellMakerTools_release"       ,
                          [# AddNoiseCellBuilderToolCfg(flags)",
                            acc.getPublicTool(CaloCellContainer.name),
                            acc.getPublicTool(FastHit.name)])
    if "Extrapolator" not in kwargs:
        Extrapolator = acc.popToolsAndMerge(NITimedExtrapolatorCfg(flags))
        acc.addPublicTool(Extrapolator)
        kwargs.setdefault("Extrapolator", acc.getPublicTool(Extrapolator.name))
    if "ParticleTruthSvc" not in kwargs:
        kwargs.setdefault("ParticleTruthSvc", acc.getPrimaryAndMerge(TruthServiceCfg(flags)).name)
    acc.setPrivateTools(CompFactory.ISF.FastCaloTool(name, **kwargs))
    return acc


def FastCaloPileupToolCfg(flags, name="ISF_FastCaloPileupTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CaloCellsOutputName", flags.Sim.FastCalo.CaloCellsName + "PileUp")
    acc.merge(PileupFastShowerCellBuilderToolCfg(flags))
    tool = acc.getPublicTool("ISF_PileupFastShowerCellBuilderTool")
    kwargs.setdefault("CaloCellMakerTools_simulate", [tool])
    acc.popToolsAndMerge(FastCaloToolBaseCfg(flags, name, **kwargs))
    return acc


def LegacyAFIIFastCaloToolCfg(flags, name="ISF_LegacyAFIIFastCaloTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("BatchProcessMcTruth", True)
    lfscb = acc.popToolsAndMerge(LegacyFastShowerCellBuilderToolCfg(flags))
    acc.addPublicTool(lfscb)
    kwargs.setdefault("CaloCellMakerTools_simulate", [acc.getPublicTool(lfscb.name)])
    tool = acc.popToolsAndMerge(FastCaloToolBaseCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def FastCaloSimV2ToolCfg(flags, name="ISF_FastCaloSimV2Tool", **kwargs):
    acc = ComponentAccumulator()

    FastHit = acc.popToolsAndMerge(FastHitConvertToolCfg(flags))
    acc.addPublicTool(FastHit)
    EmptyCellBuilder = acc.popToolsAndMerge(EmptyCellBuilderToolCfg(flags))
    acc.addPublicTool(EmptyCellBuilder)
    CaloCellContainer = acc.popToolsAndMerge(CaloCellContainerFCSFinalizerToolCfg(flags))
    acc.addPublicTool(CaloCellContainer)
    Extrapolator = acc.popToolsAndMerge(FastCaloSimCaloExtrapolationCfg(flags))
    acc.addPublicTool(Extrapolator)

    kwargs.setdefault("CaloCellsOutputName", flags.Sim.FastCalo.CaloCellsName)
    kwargs.setdefault("CaloCellMakerTools_setup",    [acc.getPublicTool(EmptyCellBuilder.name)])
    kwargs.setdefault("CaloCellMakerTools_release",  [acc.getPublicTool(CaloCellContainer.name),
                                                      acc.getPublicTool(FastHit.name)])
    kwargs.setdefault("FastCaloSimCaloExtrapolation", acc.getPublicTool(Extrapolator.name))
    kwargs.setdefault("ParamSvc", acc.getPrimaryAndMerge(FastCaloSimV2ParamSvcCfg(flags)).name)
    kwargs.setdefault("RandomSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    kwargs.setdefault("RandomStream", "FastCaloSimRnd")
    PT_tool = acc.popToolsAndMerge(PunchThroughToolCfg(flags))
    kwargs.setdefault("PunchThroughTool", PT_tool)
    if "ParticleTruthSvc" not in kwargs:
        kwargs.setdefault("ParticleTruthSvc", acc.getPrimaryAndMerge(TruthServiceCfg(flags)).name)

    acc.setPrivateTools(CompFactory.ISF.FastCaloSimV2Tool(name, **kwargs))
    return acc


###################################################################################################
# Config

def FastCaloSimSvcCfg(flags, name="ISF_FastCaloSimSvc", **kwargs):
    acc = ComponentAccumulator()

    if "SimulatorTool" not in kwargs:
        tool = acc.popToolsAndMerge(FastCaloToolBaseCfg(flags))
        acc.addPublicTool(tool)
        kwargs.setdefault("SimulatorTool", acc.getPublicTool(tool.name))
    kwargs.setdefault("Identifier", "FastCaloSim")
    acc.addService(CompFactory.ISF.LegacySimSvc(name, **kwargs), primary = True)
    return acc


def FastCaloSimPileupSvcCfg(flags, name="ISF_FastCaloSimPileupSvc", **kwargs):
    acc = ComponentAccumulator()
    tool = acc.popToolsAndMerge(FastCaloPileupToolCfg(flags))
    acc.addPublicTool(tool)
    kwargs.setdefault("SimulatorTool", acc.getPublicTool(tool.name))
    acc.merge(FastCaloSimSvcCfg(flags, name, **kwargs))
    return acc


def LegacyAFIIFastCaloSimSvcCfg(flags, name="ISF_LegacyAFIIFastCaloSimSvc", **kwargs):
    acc = ComponentAccumulator()
    tool = acc.popToolsAndMerge(LegacyAFIIFastCaloToolCfg(flags))
    acc.addPublicTool(tool)
    kwargs.setdefault("SimulatorTool", acc.getPublicTool(tool.name))
    acc.merge(FastCaloSimSvcCfg(flags, name, **kwargs))
    return acc


def FastHitConvAlgFastCaloSimSvcCfg(flags, name="ISF_FastHitConvAlgFastCaloSimSvc",**kwargs):
    acc = FastHitConvAlgCfg(flags)
    acc.merge(AddNoiseCellBuilderToolCfg(flags))
    CaloCellContainer = acc.popToolsAndMerge(CaloCellContainerFCSFinalizerToolCfg(flags))
    acc.addPublicTool(CaloCellContainer)
    kwargs.setdefault("CaloCellMakerTools_release", [acc.getPublicTool("AddNoiseCellBuilderTool"),
                                                     acc.getPublicTool(CaloCellContainer.name)])
    # setup FastCaloSim hit converter and add it to the alg sequence:
    # -> creates HITS from reco cells
    acc.merge(FastCaloSimSvcCfg(flags, name, **kwargs))
    return acc


def FastCaloSimPileupOTSvcCfg(flags, name="ISF_FastCaloSimPileupOTSvc", **kwargs):
    acc = ComponentAccumulator()
    
    FastHit = acc.popToolsAndMerge(FastHitConvertToolCfg(flags))
    acc.addPublicTool(FastHit)
    EmptyCellBuilder = acc.popToolsAndMerge(EmptyCellBuilderToolCfg(flags))
    acc.addPublicTool(EmptyCellBuilder)
    CaloCellContainer = acc.popToolsAndMerge(CaloCellContainerFCSFinalizerToolCfg(flags))
    acc.addPublicTool(CaloCellContainer)
    FastShowerCell_tool = acc.popToolsAndMerge(FastShowerCellBuilderToolBaseCfg(flags))
    acc.addPublicTool(FastShowerCell_tool)
    Extrapolator = acc.popToolsAndMerge(NITimedExtrapolatorCfg(flags))
    acc.addPublicTool(Extrapolator)

    kwargs.setdefault("BatchProcessMcTruth", False)
    kwargs.setdefault("SimulateUndefinedBarcodeParticles", False)
    kwargs.setdefault("Identifier", "FastCaloSim")
    kwargs.setdefault("CaloCellsOutputName", flags.Sim.FastCalo.CaloCellsName + "PileUp")
    kwargs.setdefault("PUWeights_lar_bapre", flags.Sim.FastChain.PUWeights_lar_bapre)
    kwargs.setdefault("PUWeights_lar_hec", flags.Sim.FastChain.PUWeights_lar_hec)
    kwargs.setdefault("PUWeights_lar_em", flags.Sim.FastChain.PUWeights_lar_em)
    kwargs.setdefault("PUWeights_tile", flags.Sim.FastChain.PUWeights_tile)
    kwargs.setdefault("CaloCellMakerTools_setup",    [acc.getPublicTool(EmptyCellBuilder.name)])
    kwargs.setdefault("CaloCellMakerTools_simulate", [acc.getPublicTool(FastShowerCell_tool.name)])
    kwargs.setdefault("CaloCellMakerTools_release",  [ # AddNoiseCellBuilderToolCfg(flags),
                                                      acc.getPublicTool(CaloCellContainer.name),
                                                      acc.getPublicTool(FastHit.name)])
    kwargs.setdefault("Extrapolator", acc.getPublicTool(Extrapolator.name))
    if "ParticleTruthSvc" not in kwargs:
        kwargs.setdefault("ParticleTruthSvc", acc.getPrimaryAndMerge(TruthServiceCfg(flags)).name)
    acc.addService(CompFactory.ISF.FastCaloSimSvcPU(name, **kwargs), primary = True)
    return acc


def FastCaloSimV2ParamSvcCfg(flags, name="ISF_FastCaloSimV2ParamSvc", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ParamsInputFilename", flags.Sim.FastCalo.ParamsInputFilename)
    kwargs.setdefault("ParamsInputObject", "SelPDGID")
    acc.addService(CompFactory.ISF.FastCaloSimV2ParamSvc(name, **kwargs), primary = True)
    return acc


def FastCaloSimV2SvcCfg(flags, name="ISF_FastCaloSimSvcV2", **kwargs):
    acc = ComponentAccumulator()
    tool = acc.popToolsAndMerge(FastCaloSimV2ToolCfg(flags))
    acc.addPublicTool(tool)
    kwargs.setdefault("SimulatorTool", acc.getPublicTool(tool.name))
    kwargs.setdefault("Identifier", "FastCaloSim")
    acc.addService(CompFactory.ISF.LegacySimSvc(name, **kwargs), primary = True)
    return acc


def DNNCaloSimSvcCfg(flags, name="ISF_DNNCaloSimSvc", **kwargs):
    acc = ComponentAccumulator()

    FastHit = acc.popToolsAndMerge(FastHitConvertToolCfg(flags))
    acc.addPublicTool(FastHit)
    EmptyCellBuilder = acc.popToolsAndMerge(EmptyCellBuilderToolCfg(flags))
    acc.addPublicTool(EmptyCellBuilder)
    CaloCellContainer = acc.popToolsAndMerge(CaloCellContainerFinalizerToolCfg(flags))
    acc.addPublicTool(CaloCellContainer)
    Extrapolator = acc.popToolsAndMerge(FastCaloSimCaloExtrapolationCfg(flags))
    acc.addPublicTool(Extrapolator)

    kwargs.setdefault("CaloCellsOutputName", flags.Sim.FastCalo.CaloCellsName)
    kwargs.setdefault("CaloCellMakerTools_setup",   [acc.getPublicTool(EmptyCellBuilder.name)])
    kwargs.setdefault("CaloCellMakerTools_release", [acc.getPublicTool(CaloCellContainer.name),
                                                     acc.getPublicTool(FastHit.name)]) #DR needed ?
    kwargs.setdefault("ParamsInputFilename", flags.Sim.FastCalo.ParamsInputFilename)
    kwargs.setdefault("FastCaloSimCaloExtrapolation", acc.getPublicTool(Extrapolator.name))
    kwargs.setdefault("RandomStream", "FastCaloSimRnd")
    kwargs.setdefault("RandomSvc",
                      acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    acc.addService(CompFactory.ISF.DNNCaloSimSvc(name, **kwargs), primary = True)
    return acc
