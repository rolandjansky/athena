"""ComponentAccumulator service configuration for ISF_FastCaloSimServices

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from RngComps.RandomServices import RNG

###################################################################################################
# Moved from AdditionalConfigNew

from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConfigNew import FastCaloSimCaloExtrapolationCfg
from FastCaloSim.FastCaloSimFactoryNew import (NITimedExtrapolatorCfg,
                                               FastShowerCellBuilderToolCfg)
from FastCaloSim.AddNoiseCellBuilderToolConfig import AddNoiseCellBuilderToolCfg


def PunchThroughToolCfg(flags, name="ISF_PunchThroughTool", **kwargs):

    from BarcodeServices.BarcodeServicesConfigNew import BarcodeSvcCfg
    from SubDetectorEnvelopes.SubDetectorEnvelopesConfigNew import EnvelopeDefSvcCfg

    acc = RNG(flags.Random.Engine)
    kwargs.setdefault("RandomNumberService", acc.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", "AthRNGSvc")
    kwargs.setdefault("FilenameLookupTable", "CaloPunchThroughParametrisation.root")
    kwargs.setdefault("PunchThroughInitiators", [211])
    kwargs.setdefault("PunchThroughParticles", [   2212,     211,      22,      11,      13])
    kwargs.setdefault("DoAntiParticles"      , [  False,    True,   False,    True,    True])
    kwargs.setdefault("CorrelatedParticle"   , [    211,    2212,      11,      22,       0])
    kwargs.setdefault("FullCorrelationEnergy", [100000., 100000., 100000., 100000.,      0.])
    kwargs.setdefault("MinEnergy"            , [  938.3,   135.6,     50.,     50.,   105.7])
    kwargs.setdefault("MaxNumParticles"      , [     -1,      -1,      -1,      -1,      -1])
    kwargs.setdefault("EnergyFactor"         , [     1.,      1.,      1.,      1.,      1.])
    acc_bar = BarcodeSvcCfg(flags)
    kwargs.setdefault("BarcodeSvc", acc_bar.getPrimary())
    acc.merge(acc_bar)
    acc.merge(EnvelopeDefSvcCfg(flags))
    kwargs.setdefault("EnvelopeDefSvc", acc.getService("AtlasGeometry_EnvelopeDefSvc"))
    kwargs.setdefault("BeamPipeRadius", 500.)
    acc.addPublicTool(CompFactory.ISF.PunchThroughTool(name, **kwargs))
    return acc


def EmptyCellBuilderToolCfg(flags, name="ISF_EmptyCellBuilderTool", **kwargs):
    acc = ComponentAccumulator()
    acc.addPublicTool(CompFactory.EmptyCellBuilderTool(name, **kwargs))
    return acc


def LegacyFastShowerCellBuilderToolCfg(flags, name="ISF_LegacyFastShowerCellBuilderTool", **kwargs):
    acc = FastShowerCellBuilderToolCfg(flags, name, **kwargs)
    FastShowerCellBuilderTool = acc.getPublicTool("ISF_FastShowerCellBuilderTool")
    FastShowerCellBuilderTool.Invisibles += [13]
    return acc

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
    return FastShowerCellBuilderToolCfg(flags, name, **kwargs)

def FastHitConvertToolCfg(flags, name="ISF_FastHitConvertTool", **kwargs):
    from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

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
    kwargs.setdefault("tileHitContainername", tile_hits_collection_name)

    acc.addPublicTool(CompFactory.FastHitConvertTool(name, **kwargs))
    return acc

# TODO: do we need this - nowhere called?
# def CaloNoiseToolCfg(flags, name="ISF_FCS_CaloNoiseTool", **kwargs):
#     from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
#     return CaloNoiseToolDefault(name, **kwargs)

def AddNoiseCellBuilderPublicToolCfg(flags, name="ISF_AddNoiseCellBuilderTool", **kwargs):
    acc = AddNoiseCellBuilderToolCfg(flags)
    tool = acc.popPrivateTools()
    acc.addPublicTool(tool)
    return acc


def CaloCellContainerFinalizerToolCfg(flags, name="ISF_CaloCellContainerFinalizerTool", **kwargs):
    acc = ComponentAccumulator()
    acc.addPublicTool(CompFactory.CaloCellContainerFinalizerTool(name, **kwargs))
    return acc


def CaloCellContainerFCSFinalizerToolCfg(flags, name="ISF_CaloCellContainerFCSFinalizerTool", **kwargs):
    acc = ComponentAccumulator()
    acc.addPublicTool(CompFactory.CaloCellContainerFCSFinalizerTool(name, **kwargs))
    return acc


def FastHitConvAlgCfg(flags, name="ISF_FastHitConvAlg", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CaloCellsInputName"  , flags.Sim.FastCalo.CaloCellsName)
    # TODO: do we need this?
    #from AthenaCommon.DetFlags import DetFlags
    #if DetFlags.pileup.LAr_on() or DetFlags.pileup.Tile_on():
    #  kwargs.setdefault("doPileup", True)
    #else:
    #  kwargs.setdefault("doPileup", False)
    acc.addEventAlgo(CompFactory.FastHitConv(name, **kwargs))
    return acc


def FastCaloToolBaseCfg(flags, name="ISF_FastCaloTool", **kwargs):
    acc = ComponentAccumulator()
    
    acc.merge(PunchThroughToolCfg(flags))
    acc.merge(FastShowerCellBuilderToolCfg(flags))
    acc.merge(FastHitConvertToolCfg(flags))
    acc.merge(EmptyCellBuilderToolCfg(flags))
    acc.merge(CaloCellContainerFinalizerToolCfg(flags))
    kwargs.setdefault("BatchProcessMcTruth"              , False)
    kwargs.setdefault("SimulateUndefinedBarcodeParticles", False)
    kwargs.setdefault("CaloCellsOutputName"        , flags.Sim.FastCalo.CaloCellsName)
    kwargs.setdefault("PunchThroughTool"           , acc.getPublicTool("ISF_PunchThroughTool"))
    kwargs.setdefault("DoPunchThroughSimulation"   , False)
    kwargs.setdefault("CaloCellMakerTools_setup"   , [acc.getPublicTool("ISF_EmptyCellBuilderTool")])
    kwargs.setdefault("CaloCellMakerTools_simulate", [acc.getPublicTool("ISF_FastShowerCellBuilderTool")])
    kwargs.setdefault("CaloCellMakerTools_release" , [acc.getPublicTool("ISF_CaloCellContainerFinalizerTool"),
                                                      acc.getPublicTool("ISF_FastHitConvertTool")])
    kwargs.setdefault("Extrapolator", NITimedExtrapolatorCfg(flags))
    # FIXME not migrated. Remove or replace
    # register the FastCaloSim random number streams
    #from G4AtlasApps.SimFlags import simFlags
    #if not simFlags.RandomSeedList.checkForExistingSeed(ISF_FastCaloSimFlags.RandomStreamName()):
        #simFlags.RandomSeedList.addSeed( ISF_FastCaloSimFlags.RandomStreamName(), 98346412, 12461240)
    acc.addPublicTool(CompFactory.ISF.FastCaloTool(name, **kwargs))
    return acc

# TODO: Do we need this - can we only call FastCaloToolBaseCfg?
# def FastCaloToolCfg(flags, name="ISF_FastCaloTool", **kwargs):
#     return FastCaloToolBaseCfg(flags, name, **kwargs)

def FastCaloPileupToolCfg(flags, name="ISF_FastCaloPileupTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CaloCellsOutputName", flags.Sim.FastCalo.CaloCellsName + "PileUp")
    acc.merge(PileupFastShowerCellBuilderToolCfg(flags))
    tool = acc.getPublicTool("ISF_PileupFastShowerCellBuilderTool")
    kwargs.setdefault("CaloCellMakerTools_simulate", [tool])
    acc.merge(FastCaloToolBaseCfg(name, **kwargs))
    return acc

def LegacyAFIIFastCaloToolCfg(flags, name="ISF_LegacyAFIIFastCaloTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("BatchProcessMcTruth", True)
    acc.merge(LegacyFastShowerCellBuilderToolCfg(flags))
    tool = acc.getPublicTool("ISF_LegacyFastShowerCellBuilderTool")
    kwargs.setdefault("CaloCellMakerTools_simulate", [tool])
    acc.merge(FastCaloToolBaseCfg(name, **kwargs))
    return acc

def FastCaloSimV2ToolCfg(flags, name="ISF_FastCaloSimV2Tool", **kwargs):
    acc = ComponentAccumulator()
    
    acc.merge(FastHitConvertToolCfg(flags))
    acc.merge(EmptyCellBuilderToolCfg(flags))
    acc.merge(CaloCellContainerFCSFinalizerToolCfg(flags))
    kwargs.setdefault("CaloCellsOutputName", flags.Sim.FastCalo.CaloCellsName)
    kwargs.setdefault("CaloCellMakerTools_setup", [acc.getPublicTool("ISF_EmptyCellBuilderTool")])
    kwargs.setdefault("CaloCellMakerTools_release", [acc.getPublicTool("ISF_CaloCellContainerFCSFinalizerTool"),
                                                     acc.getPublicTool("ISF_FastHitConvertTool")])
    kwargs.setdefault("FastCaloSimCaloExtrapolation", FastCaloSimCaloExtrapolationCfg(flags))
    kwargs.setdefault("ParamSvc", acc.popToolsAndMerge(FastCaloSimV2ParamSvcCfg(flags)))
    acc.merge(RNG(flags.Random.Engine))
    kwargs.setdefault("RandomSvc", acc.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStream", "AthRNGSvc")
    kwargs.setdefault("PunchThroughTool", acc.merge(PunchThroughToolCfg(flags)))

    acc.setPrivateTools(CompFactory.ISF.FastCaloSimV2Tool(name, **kwargs))
    return acc

###################################################################################################
# Config

def FastCaloSimSvcCfg(flags, name="ISF_FastCaloSimSvc", **kwargs):
    acc = ComponentAccumulator()
    
    if "SimulatorTool" not in kwargs:
        acc.merge(FastCaloToolBaseCfg(flags))
        kwargs.setdefault("SimulatorTool", acc.getPublicTool("ISF_FastCaloTool"))
    kwargs.setdefault("Identifier", "FastCaloSim")
    acc.addService(CompFactory.ISF.LegacySimSvc(name, **kwargs))
    return acc


def FastCaloSimPileupSvcCfg(flags, name="ISF_FastCaloSimPileupSvc", **kwargs):
    acc = FastCaloPileupToolCfg(flags)
    kwargs.setdefault("SimulatorTool", acc.getPublicTool("ISF_FastCaloTool"))
    acc.merge(FastCaloSimSvcCfg(flags, name, **kwargs))
    return acc


def LegacyAFIIFastCaloSimSvcCfg(flags, name="ISF_LegacyAFIIFastCaloSimSvc", **kwargs):
    acc = LegacyAFIIFastCaloToolCfg(flags)
    kwargs.setdefault("SimulatorTool", acc.getPublicTool("ISF_FastCaloTool"))
    acc.merge(FastCaloSimSvcCfg(flags, name, **kwargs))
    return acc


def FastHitConvAlgFastCaloSimSvcCfg(flags, name="ISF_FastHitConvAlgFastCaloSimSvc",**kwargs):
    acc = FastHitConvAlgCfg(flags)
    acc.merge(AddNoiseCellBuilderPublicToolCfg(flags))
    acc.merge(CaloCellContainerFCSFinalizerToolCfg(flags))
    kwargs.setdefault("CaloCellMakerTools_release", [acc.getPublicTool("AddNoiseCellBuilderTool"),
                                                     acc.getPublicTool("ISF_CaloCellContainerFCSFinalizerTool")])
    # setup FastCaloSim hit converter and add it to the alg sequence:
    # -> creates HITS from reco cells
    acc.merge(FastCaloSimSvcCfg(flags, name, **kwargs))
    return acc


def FastHitConvAlgLegacyAFIIFastCaloSimSvcCfg(flags, name="ISF_FastHitConvAlgLegacyAFIIFastCaloSimSvc", **kwargs):
    kwargs.setdefault("BatchProcessMcTruth", True) #TODO: This seems not to exist
    return FastHitConvAlgFastCaloSimSvcCfg(flags, name, **kwargs)


def FastCaloSimPileupOTSvcCfg(flags, name="ISF_FastCaloSimPileupOTSvc", **kwargs):
    acc = ComponentAccumulator()
    
    acc.merge(PunchThroughToolCfg(flags))
    acc.merge(FastShowerCellBuilderToolCfg(flags))
    acc.merge(FastHitConvertToolCfg(flags))
    acc.merge(EmptyCellBuilderToolCfg(flags))
    acc.merge(CaloCellContainerFCSFinalizerToolCfg(flags))
    kwargs.setdefault("BatchProcessMcTruth", False)
    kwargs.setdefault("SimulateUndefinedBarcodeParticles", False)
    kwargs.setdefault("Identifier", "FastCaloSim")
    kwargs.setdefault("CaloCellsOutputName", flags.Sim.FastCalo.CaloCellsName + "PileUp")
    kwargs.setdefault("PunchThroughTool", acc.getPublicTool("ISF_PunchThroughTool"))
    kwargs.setdefault("DoPunchThroughSimulation", False)
    kwargs.setdefault("PUWeights_lar_bapre", flags.Sim.FastChain.PUWeights_lar_bapre)
    kwargs.setdefault("PUWeights_lar_hec", flags.Sim.FastChain.PUWeights_lar_hec)
    kwargs.setdefault("PUWeights_lar_em", flags.Sim.FastChain.PUWeights_lar_em)
    kwargs.setdefault("PUWeights_tile", flags.Sim.FastChain.PUWeights_tile)
    kwargs.setdefault("CaloCellMakerTools_setup", [acc.getPublicTool("ISF_EmptyCellBuilderTool")])
    kwargs.setdefault("CaloCellMakerTools_simulate", [acc.getPublicTool("ISF_FastShowerCellBuilderTool")])
    kwargs.setdefault("CaloCellMakerTools_release", [acc.getPublicTool("ISF_CaloCellContainerFCSFinalizerTool"),
                                                     acc.getPublicTool("ISF_FastHitConvertTool")])
    kwargs.setdefault("Extrapolator", NITimedExtrapolatorCfg(flags))
    # FIXME not migrated. Remove or replace
    # register the FastCaloSim random number streams
    #from G4AtlasApps.SimFlags import simFlags
    #if not simFlags.RandomSeedList.checkForExistingSeed(ISF_FastCaloSimFlags.RandomStreamName()):
        #simFlags.RandomSeedList.addSeed( ISF_FastCaloSimFlags.RandomStreamName(), 98346412, 12461240)
    acc.addService(CompFactory.ISF.FastCaloSimSvcPU(name, **kwargs))
    return acc


def FastCaloSimV2ParamSvcCfg(flags, name="ISF_FastCaloSimV2ParamSvc", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ParamsInputFilename", flags.Sim.FastCalo.ParamsInputFilename)
    kwargs.setdefault("ParamsInputObject", "SelPDGID")
    acc.addService(CompFactory.ISF.FastCaloSimV2ParamSvc(name, **kwargs))
    return acc


def FastCaloSimV2SvcCfg(flags, name="ISF_FastCaloSimSvcV2", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SimulatorTool", acc.popToolsAndMerge(FastCaloSimV2ToolCfg(flags)))
    kwargs.setdefault("Identifier", "FastCaloSim")
    acc.addService(CompFactory.ISF.LegacySimSvc(name, **kwargs))
    return acc


def DNNCaloSimSvcCfg(flags, name="ISF_DNNCaloSimSvc", **kwargs):
    acc = ComponentAccumulator()
    
    acc.merge(FastHitConvertToolCfg(flags))
    acc.merge(EmptyCellBuilderToolCfg(flags))
    acc.merge(CaloCellContainerFinalizerToolCfg(flags))
    kwargs.setdefault("CaloCellsOutputName", flags.Sim.FastCalo.CaloCellsName)
    kwargs.setdefault("CaloCellMakerTools_setup", [acc.getPublicTool("ISF_EmptyCellBuilderTool")])
    kwargs.setdefault("CaloCellMakerTools_release", [acc.getPublicTool("ISF_CaloCellContainerFinalizerTool"),
                                                     acc.getPublicTool("ISF_FastHitConvertTool")]) #DR needed ?
    kwargs.setdefault("ParamsInputFilename", flags.Sim.FastCalo.ParamsInputFilename)
    kwargs.setdefault("FastCaloSimCaloExtrapolation", FastCaloSimCaloExtrapolationCfg(flags))

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
