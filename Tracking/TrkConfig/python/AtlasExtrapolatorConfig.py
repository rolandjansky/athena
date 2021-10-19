# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# New configuration for ATLAS extrapolator
# Based heavily on AtlasExtrapolator.py

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
import TrkConfig.AtlasExtrapolatorToolsConfig as TC

# import the Extrapolator configurable
Trk__Extrapolator=CompFactory.Trk.Extrapolator


# define the class
def AtlasExtrapolatorCfg( flags, name = 'AtlasExtrapolator' ):
       result=ComponentAccumulator()

       acc  = MagneticFieldSvcCfg(flags)
       result.merge(acc)

       # PROPAGATOR DEFAULTS --------------------------------------------------------------------------------------
       
       AtlasRungeKuttaPropagator = result.getPrimaryAndMerge( TC.AtlasRKPropagatorCfg(flags) )
       AtlasSTEP_Propagator = result.getPrimaryAndMerge( TC.AtlasSTEP_PropagatorCfg(flags) )

       AtlasPropagators  = []
       AtlasPropagators += [AtlasRungeKuttaPropagator]
       AtlasPropagators += [AtlasSTEP_Propagator]

       # UPDATOR DEFAULTS -----------------------------------------------------------------------------------------       
       
       AtlasMaterialEffectsUpdator = result.getPrimaryAndMerge( TC.AtlasMaterialEffectsUpdatorCfg(flags) )
       AtlasMaterialEffectsUpdatorLandau = result.getPrimaryAndMerge( TC.AtlasMaterialEffectsUpdatorLandauCfg(flags) )

       AtlasUpdators    = []
       AtlasUpdators    += [ AtlasMaterialEffectsUpdator ]
       AtlasUpdators    += [ AtlasMaterialEffectsUpdatorLandau ]

       AtlasNavigator = result.getPrimaryAndMerge( TC.AtlasNavigatorCfg(flags) )

       # CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE
       
       AtlasSubPropagators = []
       AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name ] # Global
       AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name ] # ID
       AtlasSubPropagators += [ AtlasSTEP_Propagator.name ] # BeamPipe
       AtlasSubPropagators += [ AtlasSTEP_Propagator.name ] # Calo
       AtlasSubPropagators += [ AtlasSTEP_Propagator.name ] # MS
       AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name ] # Cavern

       AtlasSubUpdators = []
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ] # Global
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ] # ID
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


       result.setPrivateTools(Extrapolator)

       return result


# Based on Reconstruction/egamma/egammaTools/python/egammaExtrapolators.py
def egammaCaloExtrapolatorCfg( flags, name = 'egammaCaloExtrapolator' ):
       result=ComponentAccumulator()

       egammaExtrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags, name))

       # this turns off dynamic calculation of eloss in calorimeters
       egammaExtrapolator.DoCaloDynamic = False

       RungeKuttaPropagator = result.getPrimaryAndMerge( TC.AtlasRKPropagatorCfg(flags) )
       NoMatSTEP_Propagator = result.getPrimaryAndMerge( TC.AtlasNoMatSTEP_PropagatorCfg(flags) )

       egammaPropagators  = []
       egammaPropagators += [RungeKuttaPropagator]
       egammaPropagators += [NoMatSTEP_Propagator]

       MaterialEffectsUpdator = result.getPrimaryAndMerge( TC.AtlasMaterialEffectsUpdatorCfg(flags) )
       NoElossMaterialEffectsUpdator = result.getPrimaryAndMerge( TC.AtlasNoElossMaterialEffectsUpdatorCfg(flags) )

       egammaUpdators    = []
       egammaUpdators    += [ MaterialEffectsUpdator ]
       egammaUpdators    += [ NoElossMaterialEffectsUpdator ]

       # CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE

       egammaSubPropagators = []
       egammaSubPropagators += [ RungeKuttaPropagator.name ] # Global
       egammaSubPropagators += [ RungeKuttaPropagator.name ] # ID
       egammaSubPropagators += [ RungeKuttaPropagator.name ] # BeamPipe (default is STEP)
       egammaSubPropagators += [ RungeKuttaPropagator.name ] # Calo (default is STEP)
       egammaSubPropagators += [ NoMatSTEP_Propagator.name ] # MS (default is STEP)
       egammaSubPropagators += [ RungeKuttaPropagator.name ] # Cavern

       egammaSubUpdators = []
       egammaSubUpdators    += [ MaterialEffectsUpdator.name ] # Global
       egammaSubUpdators    += [ MaterialEffectsUpdator.name ] # ID
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

       result.setPrivateTools(egammaExtrapolator)

       return result



# Based on PhysicsAnalysis/MCTruthClassifier/python/MCTruthClassifierBase.py
def MCTruthClassifierExtrapolatorCfg( flags, name = 'MCTruthClassifierExtrapolator' ):
       result=ComponentAccumulator()

       MCTruthExtrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags, name))

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

       result.setPrivateTools(MCTruthExtrapolator)

       return result


def InDetExtrapolatorCfg(flags, name='InDetExtrapolator', **kwargs) :
    result = ComponentAccumulator()

    # FIXME copied from the old config, also needs fixing on the c++ side.
    if 'Propagators' not in kwargs :
           InDetPropagator = result.getPrimaryAndMerge(TC.InDetPropagatorCfg(flags))
           Propagators = [InDetPropagator]
           kwargs.setdefault( "Propagators", Propagators )

    propagator= kwargs.get('Propagators')[0].name if kwargs.get('Propagators',None) is not None and len(kwargs.get('Propagators',None))>0 else None

    if 'MaterialEffectsUpdators' not in kwargs :
           InDetMaterialEffectsUpdator = result.getPrimaryAndMerge(TC.InDetMaterialEffectsUpdatorCfg(flags))
           MaterialEffectsUpdators = [InDetMaterialEffectsUpdator]
           kwargs.setdefault( "MaterialEffectsUpdators", MaterialEffectsUpdators )
    material_updator= kwargs.get('MaterialEffectsUpdators')[0].name if  kwargs.get('MaterialEffectsUpdators',None) is not None and len(kwargs.get('MaterialEffectsUpdators',None))>0  else None

    if 'Navigator' not in kwargs :
           AtlasNavigator = result.getPrimaryAndMerge( TC.AtlasNavigatorCfg(flags) )
           kwargs.setdefault( "Navigator", AtlasNavigator)

    sub_propagators = []
    sub_updators    = []

    # -------------------- set it depending on the geometry ----------------------------------------------------
    # default for ID is (Rk,Mat)
    sub_propagators += [ propagator ]
    sub_updators    += [ material_updator ]

    # default for Calo is (Rk,MatLandau)
    sub_propagators += [ propagator ]
    sub_updators    += [ material_updator ]

    # default for MS is (STEP,Mat)
    #  sub_propagators += [ InDetStepPropagator.name() ]
    sub_updators    += [ material_updator ]
    # @TODO should check that all sub_propagators and sub_updators are actually defined.

    kwargs.setdefault("SubPropagators"          , sub_propagators)
    kwargs.setdefault("SubMEUpdators"           , sub_updators)

    extrapolator = CompFactory.Trk.Extrapolator(name, **kwargs)
    result.addPublicTool(extrapolator, primary=True)
    return result

