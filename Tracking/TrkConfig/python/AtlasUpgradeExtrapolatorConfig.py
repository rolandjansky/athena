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

def AtlasUpgradeNoMatSTEP_PropagatorCfg(flags, name = 'AtlasUpgradeNoMatSTEP_Propagator', **kwargs):
       kwargs.setdefault("MaterialEffects", False)
       return AtlasUpgradeSTEP_PropagatorCfg(flags, name, **kwargs)

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

def AtlasUpgradeNoElossMaterialEffectsUpdatorCfg(flags, name = 'AtlasUpgradeNoElossMaterialEffectsUpdator', **kwargs):
       kwargs.setdefault("EnergyLoss", False)
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
       
       AtlasRungeKuttaPropagator = result.getPrimaryAndMerge( AtlasUpgradeRKPropagatorCfg(flags) )
       AtlasSTEP_Propagator = result.getPrimaryAndMerge( AtlasUpgradeSTEP_PropagatorCfg(flags) )
       ITkPropagator = result.getPrimaryAndMerge( ITkPropagatorCfg(flags) )

       AtlasPropagators  = []
       AtlasPropagators += [AtlasRungeKuttaPropagator]
       AtlasPropagators += [AtlasSTEP_Propagator]
       AtlasPropagators += [ITkPropagator]

       # UPDATOR DEFAULTS -----------------------------------------------------------------------------------------       

       AtlasMaterialEffectsUpdator = result.getPrimaryAndMerge( AtlasUpgradeMaterialEffectsUpdatorCfg(flags) )
       AtlasMaterialEffectsUpdatorLandau = result.getPrimaryAndMerge( AtlasUpgradeMaterialEffectsUpdatorLandauCfg(flags) )       
       ITkMaterialEffectsUpdator = result.getPrimaryAndMerge( ITkMaterialEffectsUpdatorCfg(flags) )

       AtlasUpdators    = []
       AtlasUpdators    += [ AtlasMaterialEffectsUpdator ]
       AtlasUpdators    += [ AtlasMaterialEffectsUpdatorLandau ]
       AtlasUpdators    += [ ITkMaterialEffectsUpdator ]

       AtlasNavigator = result.getPrimaryAndMerge( AtlasUpgradeNavigatorCfg(flags) )

       # CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE

       AtlasSubPropagators = []
       AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name ] # Global
       AtlasSubPropagators += [ ITkPropagator.name ] # ID
       AtlasSubPropagators += [ AtlasSTEP_Propagator.name ] # BeamPipe
       AtlasSubPropagators += [ AtlasSTEP_Propagator.name ] # Calo
       AtlasSubPropagators += [ AtlasSTEP_Propagator.name ] # MS
       AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name ] # Cavern

       AtlasSubUpdators = []
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ] # Global
       AtlasSubUpdators    += [ ITkMaterialEffectsUpdator.name ] # ID
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ] # BeamPipe
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ] # Calo
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ] # MS
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ] # Cavern
       
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


# Based on Reconstruction/egamma/egammaTools/python/egammaExtrapolators.py
def EGammaUpgradeExtrapolatorCfg( flags, name = 'EGammaUpgradeExtrapolator' ):
       result=ComponentAccumulator()

       egammaExtrapolator = result.getPrimaryAndMerge(AtlasUpgradeExtrapolatorCfg(flags, name))

       # this turns off dynamic calculation of eloss in calorimeters
       egammaExtrapolator.DoCaloDynamic = False

       RungeKuttaPropagator = result.getPrimaryAndMerge( AtlasUpgradeRKPropagatorCfg(flags) )
       NoMatSTEP_Propagator = result.getPrimaryAndMerge( AtlasUpgradeNoMatSTEP_PropagatorCfg(flags) )
       ITkPropagator = result.getPrimaryAndMerge( ITkPropagatorCfg(flags) )

       egammaPropagators  = []
       egammaPropagators += [RungeKuttaPropagator]
       egammaPropagators += [NoMatSTEP_Propagator]
       egammaPropagators += [ITkPropagator]

       MaterialEffectsUpdator = result.getPrimaryAndMerge( AtlasUpgradeMaterialEffectsUpdatorCfg(flags) )
       NoElossMaterialEffectsUpdator = result.getPrimaryAndMerge( AtlasUpgradeNoElossMaterialEffectsUpdatorCfg(flags) )
       ITkMaterialEffectsUpdator = result.getPrimaryAndMerge( ITkMaterialEffectsUpdatorCfg(flags) )

       egammaUpdators    = []
       egammaUpdators    += [ MaterialEffectsUpdator ]
       egammaUpdators    += [ NoElossMaterialEffectsUpdator ]
       egammaUpdators    += [ ITkMaterialEffectsUpdator ]

       # CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE

       egammaSubPropagators = []
       egammaSubPropagators += [ RungeKuttaPropagator.name ] # Global
       egammaSubPropagators += [ ITkPropagator.name ] # ID
       egammaSubPropagators += [ RungeKuttaPropagator.name ] # BeamPipe (default is STEP)
       egammaSubPropagators += [ RungeKuttaPropagator.name ] # Calo (default is STEP)
       egammaSubPropagators += [ NoMatSTEP_Propagator.name ] # MS (default is STEP)
       egammaSubPropagators += [ RungeKuttaPropagator.name ] # Cavern

       egammaSubUpdators = []
       egammaSubUpdators    += [ MaterialEffectsUpdator.name ] # Global
       egammaSubUpdators    += [ ITkMaterialEffectsUpdator.name ] # ID
       egammaSubUpdators    += [ MaterialEffectsUpdator.name ] # BeamPipe
       egammaSubUpdators    += [ NoElossMaterialEffectsUpdator.name ] # Calo (default is Mat)
       egammaSubUpdators    += [ NoElossMaterialEffectsUpdator.name ] # MS (default is Mat)
       egammaSubUpdators    += [ MaterialEffectsUpdator.name ] # Cavern

       egammaExtrapolator.MaterialEffectsUpdators = egammaUpdators
       egammaExtrapolator.SubMEUpdators = egammaSubUpdators
       egammaExtrapolator.Propagators = egammaPropagators
       egammaExtrapolator.SubPropagators = egammaSubPropagators
       # egamma STEP with no eloss for calo intersections
       egammaExtrapolator.STEP_Propagator = NoMatSTEP_Propagator

       result.addPublicTool(egammaExtrapolator, primary=True)

       return result



# Based on PhysicsAnalysis/MCTruthClassifier/python/MCTruthClassifierBase.py
def MCTruthClassifierUpgradeExtrapolatorCfg( flags, name = 'MCTruthClassifierUpgradeExtrapolator' ):
       result=ComponentAccumulator()

       MCTruthExtrapolator = result.getPrimaryAndMerge(AtlasUpgradeExtrapolatorCfg(flags, name))

       # this turns off dynamic calculation of eloss in calorimeters
       MCTruthExtrapolator.DoCaloDynamic = False

       MCTruthUpdators    = []

       NoElossMaterialEffectsUpdator = result.getPrimaryAndMerge( AtlasUpgradeNoElossMaterialEffectsUpdatorCfg(flags) )
       MCTruthUpdators    += [ NoElossMaterialEffectsUpdator ]

       MCTruthSubUpdators = []

       # -------------------- set it depending on the geometry ----------------------------------------------------
       MCTruthSubUpdators    += [ NoElossMaterialEffectsUpdator.name ] # Global
       MCTruthSubUpdators    += [ NoElossMaterialEffectsUpdator.name ] # ID
       MCTruthSubUpdators    += [ NoElossMaterialEffectsUpdator.name ] # beampipe
       MCTruthSubUpdators    += [ NoElossMaterialEffectsUpdator.name ] # calo
       MCTruthSubUpdators    += [ NoElossMaterialEffectsUpdator.name ] # MS
       MCTruthSubUpdators    += [ NoElossMaterialEffectsUpdator.name ] # cavern

       MCTruthExtrapolator.MaterialEffectsUpdators = MCTruthUpdators
       MCTruthExtrapolator.SubMEUpdators = MCTruthSubUpdators

       result.addPublicTool(MCTruthExtrapolator, primary=True)

       return result
