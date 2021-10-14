# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# New configuration for ATLAS extrapolator for Run 4

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
import TrkConfig.AtlasExtrapolatorToolsConfig as TC

# import the Extrapolator configurable
Trk__Extrapolator=CompFactory.Trk.Extrapolator

# define the class
def AtlasUpgradeExtrapolatorCfg( flags, name = 'AtlasUpgradeExtrapolator' ):
       result=ComponentAccumulator()

       acc  = MagneticFieldSvcCfg(flags)
       result.merge(acc)

       # PROPAGATOR DEFAULTS --------------------------------------------------------------------------------------

       AtlasRungeKuttaPropagator = result.getPrimaryAndMerge( TC.AtlasRKPropagatorCfg(flags) )
       AtlasSTEP_Propagator = result.getPrimaryAndMerge( TC.AtlasSTEP_PropagatorCfg(flags) )
       ITkPropagator = result.getPrimaryAndMerge( TC.ITkPropagatorCfg(flags) )

       AtlasPropagators  = []
       AtlasPropagators += [AtlasRungeKuttaPropagator]
       AtlasPropagators += [AtlasSTEP_Propagator]
       AtlasPropagators += [ITkPropagator]

       # UPDATOR DEFAULTS -----------------------------------------------------------------------------------------       

       AtlasMaterialEffectsUpdator = result.getPrimaryAndMerge( TC.AtlasMaterialEffectsUpdatorCfg(flags) )
       AtlasMaterialEffectsUpdatorLandau = result.getPrimaryAndMerge( TC.AtlasMaterialEffectsUpdatorLandauCfg(flags) )
       ITkMaterialEffectsUpdator = result.getPrimaryAndMerge( TC.ITkMaterialEffectsUpdatorCfg(flags) )

       AtlasUpdators    = []
       AtlasUpdators    += [ AtlasMaterialEffectsUpdator ]
       AtlasUpdators    += [ AtlasMaterialEffectsUpdatorLandau ]
       AtlasUpdators    += [ ITkMaterialEffectsUpdator ]

       AtlasNavigator = result.getPrimaryAndMerge( TC.AtlasNavigatorCfg(flags) )

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
def egammaCaloUpgradeExtrapolatorCfg( flags, name = 'egammaCaloUpgradeExtrapolator' ):
       result=ComponentAccumulator()

       egammaExtrapolator = result.getPrimaryAndMerge(AtlasUpgradeExtrapolatorCfg(flags, name))

       # this turns off dynamic calculation of eloss in calorimeters
       egammaExtrapolator.DoCaloDynamic = False

       RungeKuttaPropagator = result.getPrimaryAndMerge( TC.AtlasRKPropagatorCfg(flags) )
       NoMatSTEP_Propagator = result.getPrimaryAndMerge( TC.AtlasNoMatSTEP_PropagatorCfg(flags) )
       ITkPropagator = result.getPrimaryAndMerge( TC.ITkPropagatorCfg(flags) )

       egammaPropagators  = []
       egammaPropagators += [RungeKuttaPropagator]
       egammaPropagators += [NoMatSTEP_Propagator]
       egammaPropagators += [ITkPropagator]

       MaterialEffectsUpdator = result.getPrimaryAndMerge( TC.AtlasMaterialEffectsUpdatorCfg(flags) )
       NoElossMaterialEffectsUpdator = result.getPrimaryAndMerge( TC.AtlasNoElossMaterialEffectsUpdatorCfg(flags) )
       ITkMaterialEffectsUpdator = result.getPrimaryAndMerge( TC.ITkMaterialEffectsUpdatorCfg(flags) )

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

       NoElossMaterialEffectsUpdator = result.getPrimaryAndMerge( TC.AtlasNoElossMaterialEffectsUpdatorCfg(flags) )
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
