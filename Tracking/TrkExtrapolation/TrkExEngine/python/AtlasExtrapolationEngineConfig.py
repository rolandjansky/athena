
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# New configuration for ATLAS extrapolator
# Based heavily on AtlasExtrapolationEngine.py

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

# import the ExtrapolationEngine configurable
from TrkExEngine.TrkExEngineConf import Trk__ExtrapolationEngine as ExEngine

def AtlasExtrapolationEngineCfg( flags, name = 'Extrapolation', nameprefix='Atlas' ):
    result=ComponentAccumulator()

    acc  = MagneticFieldSvcCfg(flags)
    result.merge(acc)

    # get the correct TrackingGeometry setup
    from TrkDetDescrSvc.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc = TrackingGeometrySvcCfg(flags)
    trackingGeomSvc = acc.getPrimary()
    result.merge(acc)
       
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as RkPropagator
    rungeKuttaPropagator = RkPropagator(name = 'AtlasRungeKuttaPropagator')
    result.addPublicTool(rungeKuttaPropagator) #TODO remove one day
      
    # from the Propagator create a Propagation engine to handle path length
    Trk__PropagationEngine=CompFactory.Trk__PropagationEngine
    staticPropagator = Trk__PropagationEngine(name = nameprefix+'StaticPropagation')
    # give the tools it needs 
    staticPropagator.Propagator               = rungeKuttaPropagator
    # configure output formatting               
    staticPropagator.OutputPrefix             = '[SP] - '
    staticPropagator.OutputPostfix            = ' - '
    result.addPublicTool(staticPropagator) #TODO remove one day
       
    # load the material effects engine
    Trk__MaterialEffectsEngine=CompFactory.Trk__MaterialEffectsEngine
    materialEffectsEngine = Trk__MaterialEffectsEngine(name = nameprefix+'MaterialEffects')
    # configure output formatting               
    materialEffectsEngine.OutputPrefix        = '[ME] - '
    materialEffectsEngine.OutputPostfix       = ' - '
    result.addPublicTool(materialEffectsEngine) #TODO remove one day

        
    # load the static navigation engine
    Trk__StaticNavigationEngine=CompFactory.Trk__StaticNavigationEngine
    staticNavigator = Trk__StaticNavigationEngine(name = nameprefix+'StaticNavigation')
    # give the tools it needs 
    staticNavigator.PropagationEngine        = staticPropagator
    staticNavigator.MaterialEffectsEngine    = materialEffectsEngine
    # Geometry name
    staticNavigator.TrackingGeometry         = trackingGeomSvc.TrackingGeometryName
    # configure output formatting               
    staticNavigator.OutputPrefix             = '[SN] - '
    staticNavigator.OutputPostfix            = ' - '
    # add to tool service
    result.addPublicTool(staticNavigator) #TODO remove one day
    
    # load the Static ExtrapolationEngine
    Trk__StaticEngine=CompFactory.Trk__StaticEngine
    staticExtrapolator = Trk__StaticEngine(name = nameprefix+'StaticExtrapolation')
    # give the tools it needs 
    staticExtrapolator.PropagationEngine        = staticPropagator
    staticExtrapolator.MaterialEffectsEngine    = materialEffectsEngine
    staticExtrapolator.NavigationEngine         = staticNavigator
    # configure output formatting               
    staticExtrapolator.OutputPrefix             = '[SE] - '
    staticExtrapolator.OutputPostfix            = ' - '
    # add to tool service
    result.addPublicTool(staticExtrapolator) #TODO remove one day
    
    # call the base class constructor
    extrapolator = ExEngine(name=nameprefix+'Extrapolation',
                      ExtrapolationEngines   = [ staticExtrapolator ], 
                      PropagationEngine      = staticPropagator, 
                      NavigationEngine       = staticNavigator, 
                      TrackingGeometrySvc    = trackingGeomSvc, 
                      OutputPrefix           = '[ME] - ', 
                      OutputPostfix          = ' - ')
    result.addPublicTool(extrapolator, primary=True)
    return result

