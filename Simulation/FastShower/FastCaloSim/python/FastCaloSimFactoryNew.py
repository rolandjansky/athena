# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


# PROPAGATORS
def RungeKuttaPropagatorCfg(flags, name="AtlasRungeKuttaPropagator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug("Start configuration")
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.Trk.RungeKuttaPropagator(name, **kwargs))
    return result

def STEP_PropagatorCfg(flags, name="AtlasSTEP_Propagator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug("Start configuration")
    result = ComponentAccumulator()
    kwargs.setdefault("MaterialEffects", False)
    result.setPrivateTools(CompFactory.Trk.STEP_Propagator(name, **kwargs))
    return result


# UPDATORS
def MaterialEffectsUpdatorCfg(flags, name="MaterialEffectsUpdator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug("Start configuration")
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.Trk.MaterialEffectsUpdator(name, **kwargs))
    return result

def NIMatEffUpdatorCfg(flags, name="NIMatEffUpdator", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug("Start configuration")
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.Trk.NIMatEffUpdator(name, **kwargs))
    return result


# NAVIGATOR
def AtlasNavigatorCfg(flags, name="AtlasNavigator", **kwargs):
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    mlog = logging.getLogger(name)
    mlog.debug("Start configuration")

    result = TrackingGeometrySvcCfg(flags)
    kwargs.setdefault("TrackingGeometrySvc", result.getPrimary())

    result.setPrivateTools(CompFactory.Trk.Navigator(name, **kwargs))
    return result


# EXTRAPOLATOR
def NITimedExtrapolatorCfg(flags, name="ISF_NITimedExtrapolator", **kwargs):
    result = ComponentAccumulator()
    mlog = logging.getLogger(name)
    mlog.debug("Start configuration")

    # Configure MagneticField (resolve AtlasFieldCacheCondObj dependancy)
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(flags))
    # add LAr calibration (resolve LAr dependancies)
    from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
    result.merge(LArElecCalibDbCfg(flags, ["fSampl"]))

    TimedPropagators = []
    TimedUpdators    = []

    # NAVIGATOR
    Navigator = result.popToolsAndMerge(AtlasNavigatorCfg(flags))
    result.addPublicTool(Navigator)
    kwargs.setdefault("Navigator", Navigator)

    # PROPAGATORS
    AtlasRungeKuttaPropagator = result.popToolsAndMerge(RungeKuttaPropagatorCfg(flags))
    result.addPublicTool(AtlasRungeKuttaPropagator)
    TimedPropagators  += [AtlasRungeKuttaPropagator]

    AtlasSTEP_Propagator = result.popToolsAndMerge(STEP_PropagatorCfg(flags))
    result.addPublicTool(AtlasSTEP_Propagator)
    TimedPropagators += [AtlasSTEP_Propagator]
    kwargs.setdefault("STEP_Propagator", result.getPublicTool(AtlasSTEP_Propagator.name))

    # UPDATORS
    MaterialEffectsUpdator = result.popToolsAndMerge(MaterialEffectsUpdatorCfg(flags))
    result.addPublicTool(MaterialEffectsUpdator)

    NIMatEffUpdator = result.popToolsAndMerge(NIMatEffUpdatorCfg(flags))
    result.addPublicTool(NIMatEffUpdator)
    TimedUpdators    += [NIMatEffUpdator]
    # kwargs.setdefault("MaterialEffectsUpdators", [result.getPublicTool(NIMatEffUpdator.name)])
    kwargs.setdefault("ApplyMaterialEffects", False)

    # CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE

    TimedSubPropagators = []
    TimedSubUpdators    = []

    # -------------------- set it depending on the geometry ----------------------------------------------------
    # default for Global is (Rk,Mat)
    TimedSubPropagators += [ AtlasRungeKuttaPropagator.name ]
    TimedSubUpdators    += [ MaterialEffectsUpdator.name ]

    # default for ID is (Rk,Mat)
    TimedSubPropagators += [ AtlasRungeKuttaPropagator.name ]
    TimedSubUpdators    += [ MaterialEffectsUpdator.name ]

    # default for BeamPipe is (Rk,Mat)
    TimedSubPropagators += [ AtlasRungeKuttaPropagator.name ]
    TimedSubUpdators    += [ MaterialEffectsUpdator.name ]

    # default for Calo is (STEP,Mat)
    TimedSubPropagators += [ AtlasSTEP_Propagator.name ]
    TimedSubUpdators    += [ NIMatEffUpdator.name ]

    # default for MS is (STEP,Mat)
    TimedSubPropagators += [ AtlasSTEP_Propagator.name ]
    TimedSubUpdators    += [ MaterialEffectsUpdator.name ]

    # default for Cavern is (Rk,Mat)
    TimedSubPropagators += [ AtlasRungeKuttaPropagator.name ]
    TimedSubUpdators    += [ MaterialEffectsUpdator.name ]
    # ----------------------------------------------------------------------------------------------------------       

    kwargs.setdefault("MaterialEffectsUpdators",  TimedUpdators)
    kwargs.setdefault("Propagators", TimedPropagators)
    kwargs.setdefault("SubPropagators", TimedSubPropagators)
    kwargs.setdefault("SubMEUpdators",  TimedSubUpdators)

    result.setPrivateTools(CompFactory.Trk.TimedExtrapolator(name, **kwargs))
    return result


# FastShowerCellBuilderTool
def FastShowerCellBuilderToolBaseCfg(flags, name="ISF_FastShowerCellBuilderTool", **kwargs):

    from RngComps.RandomServices import RNG
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    from ISF_FastCaloSimServices.ISF_FastCaloSimHelpers import AdditionalParticleParametrizationFileNames

    acc = RNG(flags.Random.Engine)
    acc.merge(addFolders(flags, "/GLOBAL/AtlfastII/FastCaloSimParam", "GLOBAL_OFL", tag="FastCaloSim_v2"))

    localFileNameList = AdditionalParticleParametrizationFileNames()
    localFileNameList.insert(0, "L1_L2_egamma_corr.config20.root")

    kwargs.setdefault("AdditionalParticleParametrizationFileNames", localFileNameList)
    kwargs.setdefault("RandomService", acc.getService("AthRNGSvc"))
    kwargs.setdefault("RandomStreamName", "FastCaloSimRnd")
    kwargs.setdefault("DoSimulWithInnerDetectorTruthOnly", True)
    kwargs.setdefault("ID_cut_off_r", [1150])
    kwargs.setdefault("ID_cut_off_z", [3490])
    kwargs.setdefault("DoSimulWithInnerDetectorV14TruthCuts", True)
    kwargs.setdefault("DoNewEnergyEtaSelection", True)
    kwargs.setdefault("DoEnergyInterpolation", True)
    kwargs.setdefault("use_Ekin_for_depositions", True)
    kwargs.setdefault("McLocation", flags.Sim.FastShower.InputCollection)
    kwargs.setdefault("ParticleParametrizationFileName", "")

    if "Extrapolator" not in kwargs:
        Extrapolator = acc.popToolsAndMerge(NITimedExtrapolatorCfg(flags))
        acc.addPublicTool(Extrapolator)
        kwargs.setdefault("Extrapolator", acc.getPublicTool(Extrapolator.name))

    # New kwarg from old FastCaloSimFactory
    from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
    kwargs.setdefault("CaloEntrance", TrkDetFlags.InDetContainerName())

    #######################################################################################################
    # kwargs.setdefault("Invisibles", [12, 14, 16, 1000022])
    #########################################################################################################

    acc.setPrivateTools(CompFactory.FastShowerCellBuilderTool(name, **kwargs))
    return acc
