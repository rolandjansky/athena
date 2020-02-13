# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate InDet tools for egamma with default configuration"
__author__ = "Bruno Lenzi"

from egammaRec.Factories import FcnWrapper, ToolFactory, PublicToolFactory

# Tools for extrapolating to the calo
def configureExtrapolator( egammaExtrapolator ):

  from AthenaCommon.AppMgr import ToolSvc  
  # this turns off dynamic calculation of eloss in calorimeters
  egammaExtrapolator.DoCaloDynamic = False 
    
  # all left to MaterialEffects/EnergyLossUpdators
  from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator as MaterialEffectsUpdator
  egammaMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'egammaMaterialEffectsUpdator')
  egammaNoElossMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'egammaNoElossMaterialEffectsUpdator')
  egammaNoElossMaterialEffectsUpdator.EnergyLoss = False

  if not hasattr(ToolSvc,'egammaMaterialEffectsUpdator'):
    ToolSvc += egammaMaterialEffectsUpdator 
  if  not hasattr(ToolSvc,'egammaNoElossMaterialEffectsUpdator'):  
    ToolSvc += egammaNoElossMaterialEffectsUpdator

  MyUpdators = []
  MyUpdators += [egammaMaterialEffectsUpdator] 
  MyUpdators += [egammaNoElossMaterialEffectsUpdator]
  
  MySubUpdators = []
  MySubUpdators += [ egammaMaterialEffectsUpdator.name() ] # for Global
  MySubUpdators += [ egammaMaterialEffectsUpdator.name() ] # for ID
  MySubUpdators += [ egammaMaterialEffectsUpdator.name() ] # for BeamPipe
  MySubUpdators += [ egammaNoElossMaterialEffectsUpdator.name() ] # for Calo
  MySubUpdators += [ egammaNoElossMaterialEffectsUpdator.name() ] # for muon spectrometer
  MySubUpdators += [ egammaMaterialEffectsUpdator.name() ] # for cavern

  #egamma RungeKutta Propagator  
  from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as RkPropagator
  egammaRungeKuttaPropagator = RkPropagator(name = 'egammaRungeKuttaPropagator')
  
  #egamma STEP_Propagator with no material effects
  from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as STEP_Propagator
  egammaNoMatSTEP_Propagator = STEP_Propagator(name = 'egammaNoMatSTEP_Propagator')
  egammaNoMatSTEP_Propagator.MaterialEffects=False

  if not hasattr(ToolSvc,'egammaRungeKuttaPropagator'):
    ToolSvc += egammaRungeKuttaPropagator
  if not hasattr(ToolSvc,'egammaNoMatSTEP_Propagator'):
    ToolSvc += egammaNoMatSTEP_Propagator

  myPropagators  = []
  myPropagators += [egammaRungeKuttaPropagator]
  myPropagators += [egammaNoMatSTEP_Propagator]
  
  MySubPropagators = []
  MySubPropagators += [ egammaRungeKuttaPropagator.name() ] # for Global
  MySubPropagators += [ egammaRungeKuttaPropagator.name() ] # for ID
  MySubPropagators += [ egammaRungeKuttaPropagator.name()] # for BeamPipe
  MySubPropagators += [ egammaRungeKuttaPropagator.name() ] # for Calo
  MySubPropagators += [ egammaNoMatSTEP_Propagator.name() ] # for MS
  MySubPropagators += [ egammaRungeKuttaPropagator.name() ] # for cavern
  
  egammaExtrapolator.MaterialEffectsUpdators = MyUpdators
  egammaExtrapolator.SubMEUpdators = MySubUpdators
  egammaExtrapolator.Propagators = myPropagators
  egammaExtrapolator.SubPropagators = MySubPropagators
  #egamma STEP with no eloss for calo intersections
  egammaExtrapolator.STEP_Propagator=egammaNoMatSTEP_Propagator

###############

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
#The general use extrapolator now same as ATLAS default
egammaExtrapolator = ToolFactory(AtlasExtrapolator,
                                 name = 'egammaExtrapolator')

#Specialized for e/gamma calo extrapolations i.e ignore material effect 
#that are not that relevant in the calo side
egammaCaloExtrapolator = ToolFactory(AtlasExtrapolator,
                                 postInit=[configureExtrapolator],
                                 name = 'egammaCaloExtrapolator')



