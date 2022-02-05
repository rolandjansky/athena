# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

######################################################
# AtlasExtrapolator module
#
# it inherits from Trk__Extrapolator and uses
# the AtlasTrackingGeometrySvc
#
######################################################

# import the Extrapolator configurable
from TrkExTools.TrkExToolsConf import Trk__Extrapolator

# define the class


class AtlasExtrapolator(Trk__Extrapolator):
    # constructor
    def __init__(self, name='AtlasExtrapolator'):

        # import the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc
        if 'ToolSvc' not in dir():
            ToolSvc = ToolSvc()

        # the UNIQUE NAVIGATOR  sets also the UNIQUE GEOMETRY
        from InDetRecExample import TrackingCommon
        AtlasNavigator = TrackingCommon.getAtlasNavigator()
        # PROPAGATOR DEFAULTS

        self.AtlasPropagators = []

        from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import (
            Trk__RungeKuttaPropagator as RkPropagator)
        AtlasRungeKuttaPropagator = RkPropagator(
            name='AtlasRungeKuttaPropagator')
        ToolSvc += AtlasRungeKuttaPropagator

        self.AtlasPropagators += [AtlasRungeKuttaPropagator]

        from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import (
            Trk__STEP_Propagator as STEP_Propagator)
        AtlasSTEP_Propagator = STEP_Propagator(
            name='AtlasSTEP_Propagator')
        ToolSvc += AtlasSTEP_Propagator

        self.AtlasPropagators += [AtlasSTEP_Propagator]

        # UPDATOR DEFAULTS

        self.AtlasUpdators = []

        from TrkExTools.TrkExToolsConf import (
            Trk__MaterialEffectsUpdator as MaterialEffectsUpdator)
        AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(
            name='AtlasMaterialEffectsUpdator')

        self.AtlasUpdators += [AtlasMaterialEffectsUpdator]

        AtlasMaterialEffectsUpdatorLandau = MaterialEffectsUpdator(
            name='AtlasMaterialEffectsUpdatorLandau')
        AtlasMaterialEffectsUpdatorLandau.LandauMode = True

        self.AtlasUpdators += [AtlasMaterialEffectsUpdatorLandau]

        # CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE

        AtlasSubPropagators = []
        AtlasSubUpdators = []

        # set them depending on the geometry signatures
        # default for Global is (Rk,Mat)
        AtlasSubPropagators += [AtlasRungeKuttaPropagator.name()]
        AtlasSubUpdators += [AtlasMaterialEffectsUpdator.name()]

        # default for ID is (Rk,Mat)
        AtlasSubPropagators += [AtlasRungeKuttaPropagator.name()]
        AtlasSubUpdators += [AtlasMaterialEffectsUpdator.name()]

        # default for BeamPipe is (STEP,Mat)
        AtlasSubPropagators += [AtlasSTEP_Propagator.name()]
        AtlasSubUpdators += [AtlasMaterialEffectsUpdator.name()]

        # default for Calo is (STEP,Mat)
        AtlasSubPropagators += [AtlasSTEP_Propagator.name()]
        AtlasSubUpdators += [AtlasMaterialEffectsUpdator.name()]

        # default for MS is (STEP,Mat)
        AtlasSubPropagators += [AtlasSTEP_Propagator.name()]
        AtlasSubUpdators += [AtlasMaterialEffectsUpdator.name()]

        # default for Cavern is (Rk,Mat)
        AtlasSubPropagators += [AtlasRungeKuttaPropagator.name()]
        AtlasSubUpdators += [AtlasMaterialEffectsUpdator.name()]

        # ----------------------------------------------------------------------------------------------------------

        # call the base class constructor
        Trk__Extrapolator.__init__(self, name,
                                   Navigator=AtlasNavigator,
                                   MaterialEffectsUpdators=self.AtlasUpdators,
                                   Propagators=self.AtlasPropagators,
                                   SubPropagators=AtlasSubPropagators,
                                   SubMEUpdators=AtlasSubUpdators,
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
        for iupd in self.AtlasUpdators:
            iupd.OutputLevel = msgLvl

    # switches for material effects integration
    def setEnergyLoss(self, eloss):
        for iupd in self.AtlasUpdators:
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
