# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from RngComps.RandomServices import RNG
from IOVDbSvc.IOVDbSvcConfig import addFolders
from AthenaConfiguration.ComponentFactory import CompFactory

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
from ISF_FastCaloSimServices.ISF_FastCaloSimHelpers import AdditionalParticleParametrizationFileNames

# Initialize Athena logging
from AthenaCommon.Logging import logging
mlog = logging.getLogger('FastCaloSimFactory::configure:')

def NIMatEffUpdatorCfg(flags, name="ISF_NIMatEffUpdator", **kwargs):
    return CompFactory.Trk.NIMatEffUpdator(name, **kwargs)


def NIPropagatorCfg(flags, name="ISF_NIPropagator", **kwargs):
    mlog.info("Now configure the non-interacting propagator...")
    kwargs.setdefault("MaterialEffects", False)
    result = CompFactory.Trk.STEP_Propagator(name, **kwargs)
    mlog.info("Configure non-interacting propagator finished.")
    return result


def NITimedExtrapolatorCfg(flags, name="ISF_NITimedExtrapolator", **kwargs):
    mlog.info("Now configure the TimedExtrapolator...")
    kwargs.setdefault("MaterialEffectsUpdators", [NIMatEffUpdatorCfg(flags)])
    kwargs.setdefault("ApplyMaterialEffects", False)
    kwargs.setdefault("STEP_Propagator", NIPropagatorCfg(flags))
    result = CompFactory.Trk.TimedExtrapolator(name, **kwargs)
    mlog.info("Configure TimedExtrapolator finished.")
    return result


# FastShowerCellBuilderTool
def FastShowerCellBuilderToolBaseCfg(flags, name, **kwargs):

    acc = RNG(flags.Random.Engine)
    acc.merge(addFolders(flags, "/GLOBAL/AtlfastII/FastCaloSimParam", "GLOBAL_OFL", tag="FastCaloSim_v2"))
    
    localFileNameList = AdditionalParticleParametrizationFileNames()
    localFileNameList.insert(0, "L1_L2_egamma_corr.config20.root")
    kwargs.setdefault("AdditionalParticleParametrizationFileNames", localFileNameList)

    kwargs.setdefault("RandomService", acc.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", "AthRNGSvc")
    kwargs.setdefault("DoSimulWithInnerDetectorTruthOnly", True)
    kwargs.setdefault("ID_cut_off_r", [1150])
    kwargs.setdefault("ID_cut_off_z", [3490])
    kwargs.setdefault("DoSimulWithInnerDetectorV14TruthCuts", True)
    kwargs.setdefault("DoNewEnergyEtaSelection", True)
    kwargs.setdefault("DoEnergyInterpolation", True)
    kwargs.setdefault("use_Ekin_for_depositions", True)
    kwargs.setdefault("McLocation", flags.Sim.FastShower.InputCollection)
    kwargs.setdefault("ParticleParametrizationFileName", "")
    kwargs.setdefault("Extrapolator", NITimedExtrapolatorCfg(flags))
    # New kwarg from old FastCaloSimFactory
    kwargs.setdefault("CaloEntrance", TrkDetFlags.InDetContainerName())

    #######################################################################################################
    #theFastShowerCellBuilderTool.Invisibles=[12, 14, 16, 1000022]
    #########################################################################################################

    acc.setPrivateTools(CompFactory.FastShowerCellBuilderTool(name, **kwargs))
    return acc


def FastShowerCellBuilderToolCfg(flags, **kwargs):

    acc = FastShowerCellBuilderToolBaseCfg(flags, name="ISF_FastShowerCellBuilderTool", **kwargs)
    tool = acc.popPrivateTools()

    try:
        ParticleParametrizationFileName = tool.ParticleParametrizationFileName
    except Exception:
        ParticleParametrizationFileName = ""

    # TODO: Always False since len(Add...) = 636
    if ParticleParametrizationFileName == "" and len(tool.AdditionalParticleParametrizationFileNames) == 0:
        ParticleParametrizationFileName = "FastCaloSim/v1/ParticleEnergyParametrization.root"

    tool.ParticleParametrizationFileName = ParticleParametrizationFileName

    # TODO: Do we need this: very long output?
    mlog.info("ParticleParametrizationFile: %s", ParticleParametrizationFileName)
    mlog.verbose("all values:")
    mlog.verbose(tool)

    acc.setPrivateTools(tool)
    return acc
