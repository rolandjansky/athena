# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# New configuration for ATLAS extrapolator
# Based heavily on AtlasExtrapolator.py

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
# @TODO reture once migration to tracking geometry cond alg is complete
from InDetRecExample.TrackingCommon import use_tracking_geometry_cond_alg

# import the Extrapolator configurable
Trk__Extrapolator=CompFactory.Trk.Extrapolator

def AtlasNavigatorCfg(flags, name='AtlasNavigator') :
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
            result.merge( TrackingGeometryCondAlgCfg(flags) );
            geom_cond_key = 'AtlasTrackingGeometry'
            # @TOOD how to get the key of the TrackingGeometry conditions data ?
       # the UNIQUE NAVIGATOR ( === UNIQUE GEOMETRY) --------------------------------------------------------------
       Trk__Navigator=CompFactory.Trk.Navigator
       AtlasNavigator = Trk__Navigator(name = 'AtlasNavigator',
                                       TrackingGeometrySvc = geom_svc,
                                       TrackingGeometryKey = geom_cond_key)
       result.setPrivateTools(AtlasNavigator)
       return result


# define the class
def AtlasExtrapolatorCfg( flags, name = 'AtlasExtrapolator' ):
       result=ComponentAccumulator()

       acc  = MagneticFieldSvcCfg(flags)
       result.merge(acc)

       # PROPAGATOR DEFAULTS --------------------------------------------------------------------------------------
       
       AtlasPropagators  = []
       
       RkPropagator=CompFactory.Trk.RungeKuttaPropagator
       AtlasRungeKuttaPropagator = RkPropagator(name = 'AtlasRungeKuttaPropagator')
       result.addPublicTool(AtlasRungeKuttaPropagator) #TODO remove one day
       
       AtlasPropagators += [AtlasRungeKuttaPropagator]
       
       STEP_Propagator=CompFactory.Trk.STEP_Propagator 
       AtlasSTEP_Propagator = STEP_Propagator(name = 'AtlasSTEP_Propagator')
       result.addPublicTool(AtlasSTEP_Propagator) #TODO remove one day
       
       AtlasPropagators += [AtlasSTEP_Propagator]

       # UPDATOR DEFAULTS -----------------------------------------------------------------------------------------       
       
       AtlasUpdators    = []

       MaterialEffectsUpdator=CompFactory.Trk.MaterialEffectsUpdator
       AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdator')
       result.addPublicTool(AtlasMaterialEffectsUpdator) #TODO remove one day
       
       AtlasUpdators    += [ AtlasMaterialEffectsUpdator ]

       AtlasMaterialEffectsUpdatorLandau = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdatorLandau')
       AtlasMaterialEffectsUpdatorLandau.LandauMode = True
       result.addPublicTool(AtlasMaterialEffectsUpdatorLandau) #TODO remove one day
       
       AtlasUpdators    += [ AtlasMaterialEffectsUpdatorLandau ]

       AtlasNavigator=result.popToolsAndMerge( AtlasNavigatorCfg(flags) )
       result.addPublicTool(AtlasNavigator) #TODO remove one day

       # CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE
       
       AtlasSubPropagators = []
       AtlasSubUpdators = []

       # -------------------- set it depending on the geometry ----------------------------------------------------
       # default for Global is (Rk,Mat)
       AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]

       # default for ID is (Rk,Mat)
       AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]

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


       result.setPrivateTools(Extrapolator)

       return result
