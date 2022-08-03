# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
import TrkConfig.AtlasExtrapolatorToolsConfig as TC

# EXTRAPOLATOR
def NITimedExtrapolatorCfg(flags, name="ISF_NITimedExtrapolator", **kwargs):
    result = ComponentAccumulator()
    mlog = logging.getLogger(name)
    mlog.debug("Start configuration")

    # Configure MagneticField (resolve AtlasFieldCacheCondObj dependancy)
    from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
    result.merge(AtlasFieldCacheCondAlgCfg(flags))
    # add LAr calibration (resolve LAr dependancies)
    from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
    result.merge(LArElecCalibDbCfg(flags, ["fSampl"]))

    TimedPropagators = []
    TimedUpdators    = []

    # NAVIGATOR
    Navigator = result.popToolsAndMerge(TC.FastSimNavigatorCfg(flags))
    result.addPublicTool(Navigator)
    kwargs.setdefault("Navigator", Navigator)

    # PROPAGATORS
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
    AtlasRungeKuttaPropagator = result.popToolsAndMerge(RungeKuttaPropagatorCfg(flags))
    TimedPropagators  += [AtlasRungeKuttaPropagator]

    from TrkConfig.TrkExSTEP_PropagatorConfig import AtlasNoMatSTEP_PropagatorCfg
    AtlasSTEP_Propagator = result.popToolsAndMerge(AtlasNoMatSTEP_PropagatorCfg(flags))
    result.addPublicTool(AtlasSTEP_Propagator)
    TimedPropagators += [AtlasSTEP_Propagator]
    kwargs.setdefault("STEP_Propagator", AtlasSTEP_Propagator)

    # UPDATORS
    MaterialEffectsUpdator = result.popToolsAndMerge(TC.AtlasMaterialEffectsUpdatorCfg(flags))
    result.addPublicTool(MaterialEffectsUpdator)

    NIMatEffUpdator = result.popToolsAndMerge(TC.NIMatEffUpdatorCfg(flags))
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

    from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasEnergyLossUpdatorCfg
    AtlasELossUpdater = result.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags))

    # ----------------------------------------------------------------------------------------------------------       

    kwargs.setdefault("MaterialEffectsUpdators",  TimedUpdators)
    kwargs.setdefault("Propagators", TimedPropagators)
    kwargs.setdefault("SubPropagators", TimedSubPropagators)
    kwargs.setdefault("SubMEUpdators",  TimedSubUpdators)
    kwargs.setdefault("EnergyLossUpdater", AtlasELossUpdater)

    result.setPrivateTools(CompFactory.Trk.TimedExtrapolator(name, **kwargs))
    return result


# FastShowerCellBuilderTool
def FastShowerCellBuilderToolBaseCfg(flags, name="ISF_FastShowerCellBuilderTool", **kwargs):

    from RngComps.RandomServices import AthRNGSvcCfg
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    from ISF_FastCaloSimServices.ISF_FastCaloSimHelpers import AdditionalParticleParametrizationFileNames

    acc = ComponentAccumulator()
    acc.merge(addFolders(flags, "/GLOBAL/AtlfastII/FastCaloSimParam", "GLOBAL_OFL", tag="FastCaloSim_v2"))

    localFileNameList = AdditionalParticleParametrizationFileNames()
    localFileNameList.insert(0, "L1_L2_egamma_corr.config20.root")

    kwargs.setdefault("AdditionalParticleParametrizationFileNames", localFileNameList)
    kwargs.setdefault("RandomService", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
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

    # Cond algorithm for building CellInfoContainer
    from FastCaloSim.CellInfoContainerCondAlgConfig import CellInfoContainerCondAlgCfg
    acc.merge(CellInfoContainerCondAlgCfg(flags))

    acc.setPrivateTools(CompFactory.FastShowerCellBuilderTool(name, **kwargs))
    return acc
