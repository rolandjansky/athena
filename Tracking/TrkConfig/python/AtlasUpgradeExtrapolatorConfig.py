# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# New configuration for ATLAS extrapolator for Run 4

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
# @TODO reture once migration to tracking geometry cond alg is complete
from InDetRecExample.TrackingCommon import use_tracking_geometry_cond_alg

# import the Extrapolator configurable
Trk__Extrapolator=CompFactory.Trk.Extrapolator

def AtlasUpgradeNavigatorCfg(flags, name='AtlasUpgradeNavigator') :
       # get the correct TrackingGeometry setup
       result=ComponentAccumulator()
       geom_svc=None
       geom_cond_key=''
       if not use_tracking_geometry_cond_alg :
              from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
              acc = TrackingGeometrySvcCfg(flags)
              geom_svc = acc.getPrimary()
              result.merge(acc)
       else :
            from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
            result.merge( TrackingGeometryCondAlgCfg(flags) )
            geom_cond_key = 'AtlasTrackingGeometry'
            # @TOOD how to get the key of the TrackingGeometry conditions data ?
       # the UNIQUE NAVIGATOR ( === UNIQUE GEOMETRY) --------------------------------------------------------------
       Trk__Navigator=CompFactory.Trk.Navigator
       AtlasNavigator = Trk__Navigator(name = name,
                                       TrackingGeometrySvc = geom_svc,
                                       TrackingGeometryKey = geom_cond_key)
       result.addPublicTool(AtlasNavigator, primary=True)
       return result

def AtlasUpgradeRKPropagatorCfg(flags, name = 'AtlasUpgradeRungeKuttaPropagator', **kwargs):
       result=ComponentAccumulator()
       RkPropagator=CompFactory.Trk.RungeKuttaPropagator
       AtlasRungeKuttaPropagator = RkPropagator(name, **kwargs)
       result.addPublicTool(AtlasRungeKuttaPropagator, primary=True)
       return result

def AtlasUpgradeSTEP_PropagatorCfg(flags, name = 'AtlasUpgradeSTEP_Propagator', **kwargs):
       result=ComponentAccumulator()
       STEP_Propagator=CompFactory.Trk.STEP_Propagator 
       AtlasSTEP_Propagator = STEP_Propagator(name, **kwargs)
       result.addPublicTool(AtlasSTEP_Propagator, primary=True)
       return result

def ITkPropagatorCfg(flags, name='ITkPropagator',**kwargs):
       result = ComponentAccumulator()

       ITkPropagator = None
       if flags.ITk.propagatorType == "STEP":
              ITkPropagator = result.getPrimaryAndMerge(AtlasUpgradeSTEP_PropagatorCfg(flags, name, **kwargs))
       elif flags.ITk.propagatorType == "RungeKutta":
              kwargs.setdefault("AccuracyParameter", 0.0001)
              kwargs.setdefault("MaxStraightLineStep", .004) # Fixes a failed fit
              ITkPropagator = result.getPrimaryAndMerge(AtlasUpgradeRKPropagatorCfg(flags, name, **kwargs))

       result.addPublicTool(ITkPropagator, primary=True)
       return result

def AtlasUpgradeMaterialEffectsUpdatorCfg(flags, name = 'AtlasUpgradeMaterialEffectsUpdator', **kwargs):
       result=ComponentAccumulator()
       MaterialEffectsUpdator=CompFactory.Trk.MaterialEffectsUpdator
       AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name, **kwargs)
       result.addPublicTool(AtlasMaterialEffectsUpdator, primary=True)
       return result

def AtlasUpgradeMaterialEffectsUpdatorLandauCfg(flags, name = 'AtlasUpgradeMaterialEffectsUpdatorLandau', **kwargs):
       kwargs.setdefault("LandauMode", True)
       return AtlasUpgradeMaterialEffectsUpdatorCfg(flags, name, **kwargs)

def ITkMaterialEffectsUpdatorCfg(flags, name = "ITkMaterialEffectsUpdator", **kwargs):
       if not flags.BField.solenoidOn:
              import AthenaCommon.SystemOfUnits as Units
              kwargs.setdefault(EnergyLoss          = False)
              kwargs.setdefault(ForceMomentum       = True)
              kwargs.setdefault(ForcedMomentumValue = 1000*Units.MeV)
       return AtlasUpgradeMaterialEffectsUpdatorCfg(flags, name, **kwargs)

# define the class
def AtlasUpgradeExtrapolatorCfg( flags, name = 'AtlasUpgradeExtrapolator' ):
       result=ComponentAccumulator()

       acc  = MagneticFieldSvcCfg(flags)
       result.merge(acc)

       # PROPAGATOR DEFAULTS --------------------------------------------------------------------------------------
       
       AtlasPropagators  = []
       
       AtlasRungeKuttaPropagator = result.getPrimaryAndMerge( AtlasUpgradeRKPropagatorCfg(flags) )
       AtlasPropagators += [AtlasRungeKuttaPropagator]

       AtlasSTEP_Propagator = result.getPrimaryAndMerge( AtlasUpgradeSTEP_PropagatorCfg(flags) )
       AtlasPropagators += [AtlasSTEP_Propagator]

       ITkPropagator = result.getPrimaryAndMerge( ITkPropagatorCfg(flags) )
       AtlasPropagators += [ITkPropagator]

       # UPDATOR DEFAULTS -----------------------------------------------------------------------------------------       
       
       AtlasUpdators    = []

       AtlasMaterialEffectsUpdator = result.getPrimaryAndMerge( AtlasUpgradeMaterialEffectsUpdatorCfg(flags) )
       AtlasUpdators    += [ AtlasMaterialEffectsUpdator ]

       AtlasMaterialEffectsUpdatorLandau = result.getPrimaryAndMerge( AtlasUpgradeMaterialEffectsUpdatorLandauCfg(flags) )       
       AtlasUpdators    += [ AtlasMaterialEffectsUpdatorLandau ]

       ITkMaterialEffectsUpdator = result.getPrimaryAndMerge( ITkMaterialEffectsUpdatorCfg(flags) )
       AtlasUpdators    += [ ITkMaterialEffectsUpdator ]

       AtlasNavigator = result.getPrimaryAndMerge( AtlasUpgradeNavigatorCfg(flags) )

       # CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE
       
       AtlasSubPropagators = []
       AtlasSubUpdators = []

       # -------------------- set it depending on the geometry ----------------------------------------------------
       # default for Global is (Rk,Mat)
       AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]

       # default for ID is (Rk,Mat)
       AtlasSubPropagators += [ ITkPropagator.name ]
       AtlasSubUpdators    += [ ITkMaterialEffectsUpdator.name ]

       # default for BeamPipe is (STEP,Mat)
       AtlasSubPropagators += [ AtlasSTEP_Propagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]
       
       # default for Calo is (STEP,Mat)
       AtlasSubPropagators += [ AtlasSTEP_Propagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]
       
       # default for MS is (STEP,Mat)
       AtlasSubPropagators += [ AtlasSTEP_Propagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]

       # default for Cavern is (Rk,Mat)
       AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]
       
       # ----------------------------------------------------------------------------------------------------------          
       
       # call the base class constructor
       Extrapolator = Trk__Extrapolator(name,\
                                  Navigator = AtlasNavigator,\
                                  MaterialEffectsUpdators = AtlasUpdators,\
                                  Propagators = AtlasPropagators,\
                                  SubPropagators = AtlasSubPropagators,\
                                  SubMEUpdators = AtlasSubUpdators
                                  )


       result.addPublicTool(Extrapolator, primary=True)

       return result
