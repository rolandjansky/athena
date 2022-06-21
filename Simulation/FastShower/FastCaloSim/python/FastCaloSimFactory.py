# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from ISF_Config.ISF_jobProperties import ISF_Flags


def FastCaloSimFactory(name="FastCaloSimFactory", **kwargs):

    from AthenaCommon.Logging import logging
    mlog = logging.getLogger('FastCaloSimFactory::configure:')

    from AthenaCommon.AppMgr import ToolSvc

    mlog.info("now configure the non-interacting propagator...")
    from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
    niPropagator = Trk__STEP_Propagator()
    niPropagator.MaterialEffects = False
    ToolSvc += niPropagator
    mlog.info("configure nono-interacting propagator finished")

    from TrkExTools.TrkExToolsConf import Trk__Navigator
    navigator = None

    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")

    if ISF_Flags.UseTrackingGeometryCond:
        if not hasattr(condSeq, 'AtlasTrackingGeometryCondAlg'):
            from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlg import (
                ConfiguredTrackingGeometryCondAlg)
            TrkGeoCondAlg = ConfiguredTrackingGeometryCondAlg(
                'AtlasTrackingGeometryCondAlg')
            condSeq += TrkGeoCondAlg

        navigator = Trk__Navigator(
            name="FCSNavigator",
            TrackingGeometryKey=condSeq.AtlasTrackingGeometryCondAlg.TrackingGeometryWriteKey)
    else:
        navigator = Trk__Navigator(name="FCSNavigator", TrackingGeometryKey="")

    ToolSvc += navigator

    mlog.info("now configure the TimedExtrapolator...")
    from TrkExTools.TimedExtrapolator import TimedExtrapolator
    timedExtrapolator = TimedExtrapolator()
    timedExtrapolator.STEP_Propagator = niPropagator
    timedExtrapolator.Navigator = navigator
    timedExtrapolator.ApplyMaterialEffects = False
    ToolSvc += timedExtrapolator
    mlog.info("configure TimedExtrapolator finished")

    if not hasattr(condSeq, 'CellInfoContainerCondAlg'):
        from FastCaloSim.FastCaloSimConf import CellInfoContainerCondAlg
        condSeq += CellInfoContainerCondAlg("CellInfoContainerCondAlg")

    from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

    kwargs.setdefault("CaloEntrance", TrkDetFlags.InDetContainerName())
    kwargs.setdefault("Extrapolator", timedExtrapolator)

    from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
    theFastShowerCellBuilderTool = FastShowerCellBuilderTool(name, **kwargs)

    try:
        ParticleParametrizationFileName = theFastShowerCellBuilderTool.ParticleParametrizationFileName
    except Exception:
        ParticleParametrizationFileName = ""

    if ParticleParametrizationFileName == "" and len(theFastShowerCellBuilderTool.AdditionalParticleParametrizationFileNames) == 0:
        ParticleParametrizationFileName = "FastCaloSim/v1/ParticleEnergyParametrization.root"

    theFastShowerCellBuilderTool.ParticleParametrizationFileName = ParticleParametrizationFileName
    mlog.info("ParticleParametrizationFile=%s",
              ParticleParametrizationFileName)

    mlog.info("all values:")
    mlog.info(theFastShowerCellBuilderTool)

    return theFastShowerCellBuilderTool
