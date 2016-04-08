# block the include for all others
include.block('TrkExExample/LocalExtrapolator.py')
######################################################
# LocalExtrapolator module
#
# it using the standard Local Tracking Geometry
#
######################################################

print 'Setting up default LocalExtrapolator'
#
# Magnetic field tool
#
from TrkMagFieldTools.TrkMagFieldToolsConf import Trk__MagneticFieldTool
LocalMagField = Trk__MagneticFieldTool(name = 'LocalMagField')
ToolSvc += LocalMagField
print      LocalMagField
#
# get the ATLAS tracking geometry
include('TrkDetDescrSvc/LocalTrackingGeometrySvc.py')

# Setup the Navigator (default, could be removed)
#
from TrkExTools.TrkExToolsConf import Trk__Navigator
LocalNavigator = Trk__Navigator(name = 'LocalNavigator')
LocalNavigator.TrackingGeometryName = LocalTrackingGeometrySvc.TrackingGeometryName
ToolSvc += LocalNavigator
print      LocalNavigator
#
# Setup the MaterialEffectsUpdator
#
from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
LocalMaterialUpdator = Trk__MaterialEffectsUpdator(name = "LocalMaterialEffectsUpdator")
ToolSvc += LocalMaterialUpdator
print      LocalMaterialUpdator
#
# set the propagator
#
from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
LocalPropagator = Propagator(name = 'LocalPropagator')
ToolSvc += LocalPropagator
print      LocalPropagator
#
# create extrapolator
#
from TrkExTools.TrkExToolsConf import Trk__Extrapolator
LocalExtrapolator = Trk__Extrapolator(name                   = 'LocalExtrapolator', 
                                      Propagators            = [ LocalPropagator ],
                                      Navigator              = LocalNavigator,
                                      MaterialEffectsUpdator = LocalMaterialUpdator)
ToolSvc += LocalExtrapolator
print      LocalExtrapolator                                          
print 'Finished setting up default LocalExtrapolator'

#LocalExtrapolatorOutputLevel(1)

ToolSvc.LocalMaterialUpdator.ReferenceMaterial   = True
ToolSvc.LocalMaterialUpdator.ValidationMode      = False
ToolSvc.LocalMaterialUpdator.ValidationDirection = -1