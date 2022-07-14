# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkGlobalChi2Fitter package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType
import AthenaCommon.SystemOfUnits as Units

# The Global Chi2 is the main/reference general
# track fitter in ATLAS
# (at least for Run-1,Run-2,Run-3 ...)
# See:
# https://iopscience.iop.org/article/10.1088/1742-6596/119/3/032013
# Advantages:
# - It only needs an initial estimate of the track parameters
# - It can  solve the left/right ambiguities in drift circle hits.
# - Yields the scattering angles on the track (used in alignment).
# Main disadvantage
# - Needs to invert much larger matrices than the Kalman fitter.


#########################
# InDet configs
#########################
def InDetGlobalChi2FitterBaseCfg(flags,
                                 name='GlobalChi2FitterBase',
                                 **kwargs):
    acc = ComponentAccumulator()

    if 'TrackingGeometryReadKey' not in kwargs:
        from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
            TrackingGeometryCondAlgCfg)
        cond_alg = TrackingGeometryCondAlgCfg(flags)
        geom_cond_key = cond_alg.getPrimary().TrackingGeometryWriteKey
        acc.merge(cond_alg)
        kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    from TrkConfig.AtlasExtrapolatorToolsConfig import (
        AtlasNavigatorCfg, AtlasEnergyLossUpdatorCfg,
        InDetMaterialEffectsUpdatorCfg, InDetMultipleScatteringUpdatorCfg)

    InDetExtrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))
    InDetNavigator = acc.popToolsAndMerge(
        AtlasNavigatorCfg(flags, name="InDetNavigator"))
    ELossUpdator = acc.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags))
    InDetMaterialEffectsUpdator = acc.popToolsAndMerge(
        InDetMaterialEffectsUpdatorCfg(flags))
    MultipleScatteringUpdator = acc.popToolsAndMerge(
        InDetMultipleScatteringUpdatorCfg(flags))

    from TrkConfig.TrkExRungeKuttaPropagatorConfig import InDetPropagatorCfg
    InDetPropagator = acc.popToolsAndMerge(InDetPropagatorCfg(flags))
    from TrkConfig.TrkMeasurementUpdatorConfig import InDetUpdatorCfg
    InDetUpdator = acc.popToolsAndMerge(InDetUpdatorCfg(flags))

    kwargs.setdefault("ExtrapolationTool", InDetExtrapolator)
    kwargs.setdefault("NavigatorTool", InDetNavigator)
    kwargs.setdefault("PropagatorTool", InDetPropagator)
    kwargs.setdefault("MultipleScatteringTool", MultipleScatteringUpdator)
    kwargs.setdefault("EnergyLossTool", ELossUpdator)
    kwargs.setdefault("MeasurementUpdateTool", InDetUpdator)
    kwargs.setdefault("MaterialUpdateTool", InDetMaterialEffectsUpdator)
    kwargs.setdefault("StraightLine", not flags.BField.solenoidOn)
    kwargs.setdefault("OutlierCut", 4)
    kwargs.setdefault("SignedDriftRadius", True)
    kwargs.setdefault("ReintegrateOutliers", True)
    kwargs.setdefault("RecalibrateSilicon", True)
    kwargs.setdefault("RecalibrateTRT", True)
    kwargs.setdefault("TRTTubeHitCut", 1.75)
    kwargs.setdefault("MaxIterations", 40)
    kwargs.setdefault("Acceleration", True)
    kwargs.setdefault("RecalculateDerivatives",
                      flags.InDet.Tracking.doMinBias or
                      flags.Beam.Type is BeamType.Cosmics or
                      flags.InDet.Tracking.doBeamGas)
    kwargs.setdefault("TRTExtensionCuts", True)
    kwargs.setdefault("TrackChi2PerNDFCut", 7)

    GlobalChi2Fitter = CompFactory.Trk.GlobalChi2Fitter(name, **kwargs)
    acc.setPrivateTools(GlobalChi2Fitter)
    return acc


def InDetGlobalChi2FitterCfg(flags,
                             name='InDetGlobalChi2Fitter',
                             **kwargs):
    acc = ComponentAccumulator()

    if 'RotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import InDetRotCreatorCfg
        InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags))
        kwargs.setdefault('RotCreatorTool', InDetRotCreator)
    if 'BroadRotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import (
            InDetBroadRotCreatorCfg)
        InDetBroadRotCreator = acc.popToolsAndMerge(
            InDetBroadRotCreatorCfg(flags))
        kwargs.setdefault('BroadRotCreatorTool', InDetBroadRotCreator)

    # PHF cut during fit iterations to save CPU time
    kwargs.setdefault(
        'MinPHFCut', flags.InDet.Tracking.ActivePass.minTRTPrecFrac)

    if flags.InDet.Tracking.doDBMstandalone:
        kwargs.setdefault('StraightLine', True)
        kwargs.setdefault('OutlierCut', 5)
        kwargs.setdefault('RecalibrateTRT', False)
        kwargs.setdefault('TRTExtensionCuts', False)
        kwargs.setdefault('TrackChi2PerNDFCut', 20)

    if ((flags.InDet.Tracking.useBroadPixClusterErrors or
        flags.InDet.Tracking.useBroadSCTClusterErrors) and not
            flags.InDet.Tracking.doDBMstandalone):
        kwargs.setdefault('RecalibrateSilicon', False)

    if flags.InDet.Tracking.doRobustReco:
        kwargs.setdefault('OutlierCut', 10.0)
        kwargs.setdefault('TrackChi2PerNDFCut', 20)

    if (flags.InDet.Tracking.doRobustReco or
            flags.Beam.Type is BeamType.Cosmics):
        kwargs.setdefault('MaxOutliers', 99)

    if (flags.Beam.Type is BeamType.Cosmics or
            flags.InDet.Tracking.doBeamGas):
        kwargs.setdefault('Acceleration', False)

    if (flags.InDet.Tracking.materialInteractions and not
            flags.BField.solenoidOn):
        kwargs.setdefault('Momentum', 1000.*Units.MeV)

    InDetGlobalChi2Fitter = acc.popToolsAndMerge(
        InDetGlobalChi2FitterBaseCfg(flags, name, **kwargs))
    acc.setPrivateTools(InDetGlobalChi2Fitter)
    return acc


def InDetGlobalChi2FitterTRTCfg(flags,
                                name='InDetGlobalChi2FitterTRT',
                                **kwargs):
    acc = ComponentAccumulator()
    '''
    Global Chi2 Fitter for TRT segments with different settings
    '''

    if 'RotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import (
            InDetRefitRotCreatorCfg)
        InDetRefitRotCreator = acc.popToolsAndMerge(
            InDetRefitRotCreatorCfg(flags))
        kwargs.setdefault("RotCreatorTool", InDetRefitRotCreator)

    kwargs.setdefault("MaterialUpdateTool", '')
    kwargs.setdefault("SignedDriftRadius", True)
    kwargs.setdefault("RecalibrateSilicon", False)
    kwargs.setdefault("RecalibrateTRT", False)
    kwargs.setdefault("TRTTubeHitCut", 2.5)
    kwargs.setdefault("MaxIterations", 10)
    kwargs.setdefault("Acceleration", False)
    kwargs.setdefault("RecalculateDerivatives", False)
    kwargs.setdefault("TRTExtensionCuts", True)
    kwargs.setdefault("TrackChi2PerNDFCut", 999999)
    kwargs.setdefault(
        "Momentum", 1000.*Units.MeV
        if flags.InDet.Tracking.materialInteractions
        and not flags.BField.solenoidOn else 0)
    kwargs.setdefault("OutlierCut", 5)
    kwargs.setdefault(
        "MaxOutliers", 99 if flags.InDet.Tracking.doRobustReco or
        flags.Beam.Type is BeamType.Cosmics else 10)
    kwargs.setdefault("ReintegrateOutliers", False)

    InDetGlobalChi2Fitter = acc.popToolsAndMerge(
        InDetGlobalChi2FitterBaseCfg(flags, name, **kwargs))
    acc.setPrivateTools(InDetGlobalChi2Fitter)
    return acc


def InDetGlobalChi2FitterLowPtCfg(flags,
                                  name='InDetGlobalChi2FitterLowPt',
                                  **kwargs):
    acc = ComponentAccumulator()

    if 'RotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import InDetRotCreatorCfg
        InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags))
        kwargs.setdefault('RotCreatorTool', InDetRotCreator)
    if 'BroadRotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import (
            InDetBroadRotCreatorCfg)
        InDetBroadRotCreator = acc.popToolsAndMerge(
            InDetBroadRotCreatorCfg(flags))
        kwargs.setdefault('BroadRotCreatorTool', InDetBroadRotCreator)

    kwargs.setdefault('OutlierCut', 5.0)
    kwargs.setdefault('Acceleration', False)
    kwargs.setdefault('RecalculateDerivatives', True)
    kwargs.setdefault('TrackChi2PerNDFCut', 10)

    InDetGlobalChi2Fitter = acc.popToolsAndMerge(
        InDetGlobalChi2FitterBaseCfg(flags, name, **kwargs))
    acc.setPrivateTools(InDetGlobalChi2Fitter)
    return acc


def InDetGlobalChi2FitterBTCfg(flags,
                               name='InDetGlobalChi2FitterBT',
                               **kwargs):
    '''
    Global Chi2 Fitter for backtracking
    '''
    kwargs.setdefault("MinPHFCut", 0.)
    return InDetGlobalChi2FitterCfg(flags, name, **kwargs)


##############################
#   Muon configs
##############################
def MuonChi2TrackFitterCfg(flags,
                           name='MuonChi2TrackFitter',
                           **kwargs):
    result = ComponentAccumulator()
    from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasMultipleScatteringUpdatorCfg

    from TrkConfig.TrkRIO_OnTrackCreatorConfig import MuonRotCreatorCfg
    rotcreator = result.popToolsAndMerge(MuonRotCreatorCfg(flags))
    from TrkConfig.TrkMeasurementUpdatorConfig import KalmanUpdatorCfg
    measurement_updater = result.popToolsAndMerge(
        KalmanUpdatorCfg(flags, name='MuonMeasUpdator'))

    if 'ExtrapolationTool' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import MuonExtrapolatorCfg
        kwargs.setdefault("ExtrapolationTool", result.popToolsAndMerge(MuonExtrapolatorCfg(flags)))
    kwargs.setdefault("MultipleScatteringTool", result.popToolsAndMerge(AtlasMultipleScatteringUpdatorCfg(flags, UseTrkUtils=True)))
    kwargs.setdefault("RotCreatorTool", rotcreator)
    kwargs.setdefault("MeasurementUpdateTool", measurement_updater)
    kwargs.setdefault("StraightLine", False)
    kwargs.setdefault("OutlierCut", 3.0)
    kwargs.setdefault("GetMaterialFromTrack", False)
    kwargs.setdefault("RejectLargeNScat", True)

    # take propagator and navigator from the extrapolator
    extrapolator = kwargs['ExtrapolationTool']
    kwargs.setdefault("PropagatorTool",
                      extrapolator.Propagators[0]
                      if len(extrapolator.Propagators) > 0
                      else "")
    kwargs.setdefault("NavigatorTool",  extrapolator.Navigator)
    kwargs.setdefault("EnergyLossTool", extrapolator.EnergyLossUpdater)
    # We need to include the tracking geometry conditions
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
        TrackingGeometryCondAlgCfg)
    geom_cond_key = result.getPrimaryAndMerge(
        TrackingGeometryCondAlgCfg(flags)).TrackingGeometryWriteKey
    kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)
    fitter = CompFactory.Trk.GlobalChi2Fitter(name=name, **kwargs)
    result.setPrivateTools(fitter)
    return result


def MCTBFitterCfg(flags,
                  name='MCTBFitter',
                  **kwargs):
    from TrkConfig.AtlasExtrapolatorConfig import MCTBExtrapolatorCfg
    result = MCTBExtrapolatorCfg(flags)
    kwargs.setdefault("GetMaterialFromTrack", True)
    kwargs.setdefault("ExtrapolationTool", result.popPrivateTools())
    return MuonChi2TrackFitterCfg(flags, name=name, **kwargs)

def MCTBFitterMaterialFromTrackCfg(flags, name='MCTBFitterMaterialFromTrack', **kwargs):
    return MCTBFitterCfg(flags, name, GetMaterialFromTrack=True, **kwargs)

def MCTBSLFitterCfg(flags, name = 'MCTBSLFitter', **kwargs):
    kwargs["StraightLine"] = True       # always set
    kwargs.setdefault("Momentum", flags.Muon.straightLineFitMomentum)    
    return MCTBFitterCfg(flags, name=name, **kwargs)

def MCTBSLFitterMaterialFromTrackCfg(flags,
                                     name='MCTBSLFitterMaterialFromTrack',
                                     **kwargs):
    result = ComponentAccumulator()
    kwargs["GetMaterialFromTrack"] = True  # always set
    from TrkConfig.AtlasExtrapolatorConfig import (
        MuonStraightLineExtrapolatorCfg)
    extrapolator = result.popToolsAndMerge(
        MuonStraightLineExtrapolatorCfg(flags))
    kwargs.setdefault("ExtrapolationTool", extrapolator)

    from TrkConfig.TrkExRungeKuttaPropagatorConfig import (
        RungeKuttaPropagatorCfg)
    propagator = result.popToolsAndMerge(
        RungeKuttaPropagatorCfg(flags, name="MuonRK_Propagator"))
    kwargs["PropagatorTool"] = propagator

    result.setPrivateTools(result.popToolsAndMerge(
        MCTBSLFitterCfg(flags, name, **kwargs)))
    return result


# Unused
def MuonCombinedTrackFitterCfg(flags,
                               name="MuonCombinedTrackFitter",
                               **kwargs):
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    from TrkConfig.AtlasExtrapolatorToolsConfig import (
        AtlasNavigatorCfg, AtlasEnergyLossUpdatorCfg)
    from TrkConfig.TrkRIO_OnTrackCreatorConfig import MuonRotCreatorCfg
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import (
        MuonCombinedPropagatorCfg)
    from TrkConfig.TrkMeasurementUpdatorConfig import KalmanUpdatorCfg

    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("ExtrapolationTool", result.popPrivateTools())
    kwargs.setdefault("NavigatorTool", result.popToolsAndMerge(
        AtlasNavigatorCfg(flags)))
    kwargs.setdefault("PropagatorTool", result.popToolsAndMerge(
        MuonCombinedPropagatorCfg(flags)))
    kwargs.setdefault("RotCreatorTool", result.popToolsAndMerge(
        MuonRotCreatorCfg(flags)))
    kwargs.setdefault("EnergyLossTool", result.popToolsAndMerge(
        AtlasEnergyLossUpdatorCfg(flags)))
    kwargs.setdefault("MeasurementUpdateTool",
                      result.popToolsAndMerge(
                          KalmanUpdatorCfg(flags,
                                           name="MuonMeasUpdator")))

    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
        TrackingGeometryCondAlgCfg)
    result.merge(TrackingGeometryCondAlgCfg(flags))
    kwargs.setdefault("TrackingGeometryReadKey", "AtlasTrackingGeometry")

    kwargs.setdefault("ExtrapolatorMaterial", True)

    from MuonCombinedConfig.MuonCombinedRecToolsConfig import (
        MuidMaterialEffectsOnTrackProviderCfg)
    kwargs.setdefault("MuidTool", result.popToolsAndMerge(
        MuidMaterialEffectsOnTrackProviderCfg(flags)))
    kwargs.setdefault("MuidToolParam", None)
    if flags.Beam.Type is BeamType.Cosmics:
        from MuonCombinedConfig.MuonCombinedRecToolsConfig import (
            MuidMaterialEffectsOnTrackProviderParamCfg)
        kwargs.setdefault("MuidToolParam", result.popToolsAndMerge(
            MuidMaterialEffectsOnTrackProviderParamCfg(flags)))
    kwargs.setdefault("MuidMat", True)
    kwargs.setdefault("StraightLine", flags.Beam.Type is BeamType.Cosmics)
    # ^ Was: not jobproperties.BField.solenoidOn() and not jobproperties.BField.allToroidOn()
    kwargs.setdefault("MaxIterations", 50)
    kwargs.setdefault("GetMaterialFromTrack",
                      flags.Beam.Type is not BeamType.Cosmics)
    # ^ Was: jobproperties.BField.solenoidOn() and jobproperties.BField.allToroidOn()
    kwargs.setdefault("RecalculateDerivatives", False)
    kwargs.setdefault("UseCaloTG", True)
    tool = CompFactory.Trk.GlobalChi2Fitter(name, **kwargs)
    result.setPrivateTools(tool)
    return result


##################################
# InDet trigger config
##################################
def InDetTrigGlobalChi2FitterCfg(flags, name='InDetTrigTrackFitter', **kwargs):
    acc = ComponentAccumulator()

    if 'TrackingGeometryReadKey' not in kwargs:
        from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
            TrackingGeometryCondAlgCfg)
        cond_alg = TrackingGeometryCondAlgCfg(flags)
        geom_cond_key = cond_alg.getPrimary().TrackingGeometryWriteKey
        acc.merge(cond_alg)
        kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    from TrkConfig.AtlasExtrapolatorToolsConfig import (
        AtlasNavigatorCfg, AtlasEnergyLossUpdatorCfg,
        AtlasMaterialEffectsUpdatorCfg)

    InDetExtrapolator = acc.getPrimaryAndMerge(
        InDetExtrapolatorCfg(flags, name="InDetTrigExtrapolator"))
    InDetNavigator = acc.popToolsAndMerge(
        AtlasNavigatorCfg(flags, name="InDetTrigNavigator"))
    ELossUpdator = acc.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags))
    MaterialEffectsUpdator = acc.popToolsAndMerge(
        AtlasMaterialEffectsUpdatorCfg(flags,
                                       name="InDetTrigMaterialEffectsUpdator"))

    from TrkConfig.TrkExRungeKuttaPropagatorConfig import (
        RungeKuttaPropagatorCfg)
    InDetTrigRKPropagator = acc.popToolsAndMerge(
        RungeKuttaPropagatorCfg(flags, name="InDetTrigRKPropagator"))
    from TrkConfig.TrkMeasurementUpdatorConfig import KalmanUpdatorCfg
    InDetTrigUpdator = acc.popToolsAndMerge(
        KalmanUpdatorCfg(flags, name="InDetTrigUpdator"))
    from TrkConfig.TrkRIO_OnTrackCreatorConfig import TrigRotCreatorCfg
    TrigRotCreator = acc.popToolsAndMerge(TrigRotCreatorCfg(flags))

    kwargs.setdefault("ExtrapolationTool", InDetExtrapolator)
    kwargs.setdefault("NavigatorTool", InDetNavigator)
    kwargs.setdefault("PropagatorTool", InDetTrigRKPropagator)
    kwargs.setdefault("EnergyLossTool", ELossUpdator)
    kwargs.setdefault("MeasurementUpdateTool", InDetTrigUpdator)
    kwargs.setdefault("MaterialUpdateTool", MaterialEffectsUpdator)
    kwargs.setdefault("RotCreatorTool", TrigRotCreator)
    # InDetTrigBroadInDetRotCreator, #TODO, we have function to configure it
    kwargs.setdefault("BroadRotCreatorTool", None)

    kwargs.setdefault("StraightLine", not flags.BField.solenoidOn)
    kwargs.setdefault("OutlierCut", 4)
    kwargs.setdefault("SignedDriftRadius", True)
    kwargs.setdefault("TrackChi2PerNDFCut", 9)
    kwargs.setdefault("TRTExtensionCuts", True)
    kwargs.setdefault("MaxIterations", 40)
    kwargs.setdefault("Acceleration", True)

    # TODO come back to these settings
    # if InDetTrigFlags.useBroadClusterErrors():
    #   InDetTrigTrackFitter.RecalibrateSilicon = False

    # if InDetTrigFlags.doRefit():
    #   InDetTrigTrackFitter.BroadRotCreatorTool = None
    #   InDetTrigTrackFitter.RecalibrateSilicon = False
    #   InDetTrigTrackFitter.RecalibrateTRT     = False
    #   InDetTrigTrackFitter.ReintegrateOutliers= False

    # if InDetTrigFlags.doRobustReco():
    #   InDetTrigTrackFitter.OutlierCut         = 10.0
    #   InDetTrigTrackFitter.TrackChi2PerNDFCut = 20
    #   InDetTrigTrackFitter.MaxOutliers        = 99
    #   #only switch off for cosmics InDetTrigTrackFitter.Acceleration = False

    GlobalChi2Fitter = CompFactory.Trk.GlobalChi2Fitter(name, **kwargs)
    acc.setPrivateTools(GlobalChi2Fitter)
    return acc


#########################
# ITk configs
#########################
def ITkGlobalChi2FitterBaseCfg(flags,
                               name='ITkGlobalChi2FitterBase',
                               **kwargs):
    acc = ComponentAccumulator()

    if 'TrackingGeometryReadKey' not in kwargs:
        from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
            TrackingGeometryCondAlgCfg)
        geom_cond = TrackingGeometryCondAlgCfg(flags)
        geom_cond_key = geom_cond.getPrimary().TrackingGeometryWriteKey
        acc.merge(acc)
        kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    from TrkConfig.AtlasExtrapolatorToolsConfig import (
        AtlasNavigatorCfg, AtlasEnergyLossUpdatorCfg,
        ITkMaterialEffectsUpdatorCfg, ITkMultipleScatteringUpdatorCfg)

    Extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    Navigator = acc.popToolsAndMerge(AtlasNavigatorCfg(flags))
    ELossUpdator = acc.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags))
    ITkMaterialEffectsUpdator = acc.popToolsAndMerge(
        ITkMaterialEffectsUpdatorCfg(flags))
    MultipleScatteringUpdator = acc.popToolsAndMerge(
        ITkMultipleScatteringUpdatorCfg(flags))

    from TrkConfig.TrkExRungeKuttaPropagatorConfig import ITkPropagatorCfg
    ITkPropagator = acc.popToolsAndMerge(ITkPropagatorCfg(flags))
    from TrkConfig.TrkMeasurementUpdatorConfig import ITkUpdatorCfg
    ITkUpdator = acc.popToolsAndMerge(ITkUpdatorCfg(flags))

    kwargs.setdefault("ExtrapolationTool", Extrapolator)
    kwargs.setdefault("NavigatorTool", Navigator)
    kwargs.setdefault("PropagatorTool", ITkPropagator)
    kwargs.setdefault("MultipleScatteringTool", MultipleScatteringUpdator)
    kwargs.setdefault("EnergyLossTool", ELossUpdator)
    kwargs.setdefault("MeasurementUpdateTool", ITkUpdator)
    kwargs.setdefault("MaterialUpdateTool", ITkMaterialEffectsUpdator)
    kwargs.setdefault("StraightLine", not flags.BField.solenoidOn)
    kwargs.setdefault("OutlierCut", 4)
    kwargs.setdefault("SignedDriftRadius", True)
    kwargs.setdefault("ReintegrateOutliers", True)
    kwargs.setdefault("RecalibrateSilicon", True)
    kwargs.setdefault("MaxIterations", 40)
    kwargs.setdefault("Acceleration", True)
    kwargs.setdefault("RecalculateDerivatives",
                      flags.Beam.Type is BeamType.Cosmics)
    kwargs.setdefault("TrackChi2PerNDFCut", 7)

    GlobalChi2Fitter = CompFactory.Trk.GlobalChi2Fitter(name=name, **kwargs)
    acc.setPrivateTools(GlobalChi2Fitter)
    return acc


def ITkGlobalChi2FitterCfg(flags,
                           name='ITkGlobalChi2Fitter',
                           **kwargs):
    acc = ComponentAccumulator()

    if 'RotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import ITkRotCreatorCfg
        ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags))
        kwargs.setdefault('RotCreatorTool', ITkRotCreator)

    if 'BroadRotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import ITkBroadRotCreatorCfg
        ITkBroadRotCreator = acc.popToolsAndMerge(ITkBroadRotCreatorCfg(flags))
        kwargs.setdefault('BroadRotCreatorTool', ITkBroadRotCreator)

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault('MaxOutliers', 99)
        kwargs.setdefault('Acceleration', False)

    if flags.ITk.Tracking.materialInteractions and not flags.BField.solenoidOn:
        kwargs.setdefault('Momentum', 1000.*Units.MeV)

    ITkGlobalChi2Fitter = acc.popToolsAndMerge(
        ITkGlobalChi2FitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkGlobalChi2Fitter)
    return acc
