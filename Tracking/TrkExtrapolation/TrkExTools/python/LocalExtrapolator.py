# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

######################################################
# LocalExtrapolator module
#
# it inherits from Trk__Extrapolator and uses
# the LocalTrackingGeometrySvc
#
######################################################

# import the include statement
from AthenaCommon.Include import include

# import the Extrapolator configurable
from TrkExTools.TrkExToolsConf import Trk__Extrapolator

# define the class
class LocalExtrapolator( Trk__Extrapolator ):
    # constructor
    def __init__(self,name = 'LocalExtrapolator'):
       
       # get the correct TrackingGeometry setup
       include('TrkDetDescrSvc/LocalTrackingGeometrySvc.py')
       from __main__ import LocalTrackingGeometrySvc
       from __main__ import ToolSvc

       # the Navigator has to get this one
       from TrkExTools.TrkExToolsConf import Trk__Navigator
       LocalNavigator = Trk__Navigator(name = 'LocalNavigator')
       LocalNavigator.TrackingGeometryName = LocalTrackingGeometrySvc.TrackingGeometryName
       ToolSvc += LocalNavigator

       # the standard RungeKuttaPropagator
       from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
       LocalPropagator = Propagator(name = 'LocalPropagator')
       ToolSvc += LocalPropagator
       # call the base class constructor
       Trk__Extrapolator.__init__(self,name,Navigator = LocalNavigator, Propagators = [ LocalPropagator ] )
