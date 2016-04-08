# block the include for all others
include.block('TrkExExample/EmptyExtrapolator.py')
######################################################
# EmptyExtrapolator module
#
# it used a local tracking geometry file
#
######################################################
# include the configuration of the Extrapolator && GeometryBuilder
include('TrkExTools/ExtrapolationConfiguration.py')
include('TrkDetDescrSvc/EmptyTrackingGeometrySvc.py')
# Navigator with Emptynstruction geometry
EmptyNavigator = ConfiguredNavigator('EmptyNavigator', \
                                     tgname = EmptyTrackingGeometrySvc.trackingGeometryName())
# The standard material effects updator
EmptyMaterialUpdator = ConfiguredUpdator('EmptyMaterialUpdator', ms=false, eloss=false)
# The Propagator
EmptyPropagator = ConfiguredPropagator('Trk::RungeKuttaPropagator', 'EmptyPropagator')

# Extrapolator for Empty setup
EmptyExtrapolator = ConfiguredExtrapolator('EmptyExtrapolator', \
                                            EmptyNavigator, \
                                            EmptyMaterialUpdator, \
                                            [ EmptyPropagator] )
                                                    
# define the function to set the Output Level of the EmptyExtrapolator
def EmptyExtrapolatorOutputLevel(msgOutputLevel) :
  # Steer the message stream level                                                    
  EmptyNavigator.msgStreamLevel(msgOutputLevel)
  # the Material Effects
  EmptyMaterialUpdator.msgStreamLevel(msgOutputLevel)
  # nw Material Upator
  ToolSvc.EmptyEnergyLossUpdator.OutputLevel = msgOutputLevel
  # Propagator output level
  EmptyPropagator.msgStreamLevel(msgOutputLevel)
  # extrapolator
  EmptyExtrapolator.msgStreamLevel(msgOutputLevel)

# make a screen summary
EmptyExtrapolator.printInfo()

#EmptyExtrapolatorOutputLevel(1)
