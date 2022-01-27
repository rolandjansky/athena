# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

######################################################
# TimedExtrapolator module
# it inherits from Trk__TimedExtrapolator and
# provides a completer setup
# Used by Fast Simulation
######################################################


from ISF_Config.ISF_jobProperties import ISF_Flags
from TrkExTools.TrkExToolsConf import Trk__TimedExtrapolator


def getNINavigator(name="NINavigator", **kwargs):

    # Decide if we want to use the Tracking Geometry
    # from conditions or not
    if (ISF_Flags.UseTrackingGeometryCond
            and 'TrackingGeometryKey' not in kwargs):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, 'AtlasTrackingGeometryCondAlg'):
            from InDetCondFolders import InDetAlignFolders_FATRAS  # noqa: F401
            from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlg import (
                ConfiguredTrackingGeometryCondAlg)
            TrkGeoCondAlg = ConfiguredTrackingGeometryCondAlg(
                'AtlasTrackingGeometryCondAlg')
            condSeq += TrkGeoCondAlg

        kwargs.setdefault(
            'TrackingGeometryKey',
            condSeq.AtlasTrackingGeometryCondAlg.TrackingGeometryWriteKey)
    elif 'TrackingGeometrySvc' not in kwargs:
        from TrkDetDescrSvc.AtlasTrackingGeometrySvc import (
            AtlasTrackingGeometrySvc)
        kwargs.setdefault('TrackingGeometrySvc', AtlasTrackingGeometrySvc)
        kwargs.setdefault('TrackingGeometryKey', '')

    from TrkExTools.TrkExToolsConf import Trk__Navigator
    return Trk__Navigator(name, **kwargs)


class TimedExtrapolator(Trk__TimedExtrapolator):
    # constructor
    def __init__(self, name='TimedExtrapolator'):

        # get the correct TrackingGeometry setup

        # import the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc
        if 'ToolSvc' not in dir():
            ToolSvc = ToolSvc()

        # PROPAGATOR DEFAULTS -------------------------------------------

        self.TimedPropagators = []
        from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import (
            Trk__RungeKuttaPropagator as RkPropagator)
        AtlasRungeKuttaPropagator = RkPropagator(
            name='AtlasRungeKuttaPropagator')
        ToolSvc += AtlasRungeKuttaPropagator

        self.TimedPropagators += [AtlasRungeKuttaPropagator]

        from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import (
            Trk__STEP_Propagator as STEP_Propagator)
        AtlasSTEP_Propagator = STEP_Propagator(name='AtlasSTEP_Propagator')
        ToolSvc += AtlasSTEP_Propagator

        self.TimedPropagators += [AtlasSTEP_Propagator]

        # UPDATOR DEFAULTS ----------------------------

        from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
        MaterialEffectsUpdator = Trk__MaterialEffectsUpdator(
            name='MaterialEffectsUpdator')
        ToolSvc += MaterialEffectsUpdator

        self.TimedUpdators = []

        from TrkExTools.TrkExToolsConf import (
            Trk__NIMatEffUpdator as NIMatEffUpdator)
        NIMatEffUpdator = NIMatEffUpdator(name='NIMatEffUpdator')
        ToolSvc += NIMatEffUpdator

        self.TimedUpdators += [NIMatEffUpdator]

        # the UNIQUE NAVIGATOR ( === UNIQUE GEOMETRY) -----------------------
        AtlasNINavigator = getNINavigator(name='AtlasNINavigator')
        ToolSvc += AtlasNINavigator

        # CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE

        TimedSubPropagators = []
        TimedSubUpdators = []

        # ----------- set it depending on the geometry -------
        # default for Global is (Rk,Mat)
        TimedSubPropagators += [AtlasRungeKuttaPropagator.name()]
        TimedSubUpdators += [MaterialEffectsUpdator.name()]

        # default for ID is (Rk,Mat)
        TimedSubPropagators += [AtlasRungeKuttaPropagator.name()]
        TimedSubUpdators += [MaterialEffectsUpdator.name()]

        # default for BeamPipe is (Rk,Mat)
        TimedSubPropagators += [AtlasRungeKuttaPropagator.name()]
        TimedSubUpdators += [MaterialEffectsUpdator.name()]

        # default for Calo is (STEP,Mat)
        TimedSubPropagators += [AtlasSTEP_Propagator.name()]
        TimedSubUpdators += [NIMatEffUpdator.name()]

        # default for MS is (STEP,Mat)
        TimedSubPropagators += [AtlasSTEP_Propagator.name()]
        TimedSubUpdators += [MaterialEffectsUpdator.name()]

        # default for Cavern is (Rk,Mat)
        TimedSubPropagators += [AtlasRungeKuttaPropagator.name()]
        TimedSubUpdators += [MaterialEffectsUpdator.name()]

        # ---------------------------------------

        # call the base class constructor
        Trk__TimedExtrapolator.__init__(
            self, name,
            Navigator=AtlasNINavigator,
            MaterialEffectsUpdators=self.TimedUpdators,
            Propagators=self.TimedPropagators,
            SubPropagators=TimedSubPropagators,
            SubMEUpdators=TimedSubUpdators,
            DoCaloDynamic=False
        )

    # switches for simple steering : output navigators
    def setNavigatorOutputLevel(self, msgLvl):
        self.Navigator.OutputLevel = msgLvl

    # switches for simple steering : output propagators
    def setPropagatorsOutputLevel(self, msgLvl):
        for iprop in self.Propagators:
            iprop.OutputLevel = msgLvl

    # switches for simple steering : output material effect updators
    def setUpdatorsOutputLevel(self, msgLvl):
        for iupd in self.TimedUpdators:
            iupd.OutputLevel = msgLvl

    # switches for material effects integration
    def setEnergyLoss(self, eloss):
        for iupd in self.TimedUpdators:
            try:
                iupd.EnergyLoss = eloss
            except AttributeError:
                pass

    # switches for material effects integration
    def setMultipleScattering(self, ms):
        for iupd in self.AtlasUpdators:
            try:
                iupd.MultipleScattering = ms
            except AttributeError:
                pass
