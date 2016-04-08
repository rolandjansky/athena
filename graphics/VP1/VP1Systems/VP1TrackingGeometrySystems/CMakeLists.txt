################################################################################
# Package: VP1TrackingGeometrySystems
################################################################################

# Declare the package name:
atlas_subdir( VP1TrackingGeometrySystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoModel/GeoModelKernel
                          Tracking/TrkDetDescr/TrkVolumes
                          graphics/VP1/VP1Base
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Simulation/ISF/ISF_Fatras/ISF_FatrasDetDescrModel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          graphics/VP1/VP1HEPVis
                          graphics/VP1/VP1Utils )

# External dependencies:
find_package( Coin3D )
find_package( Eigen )

include_directories( /usr/X11R6/include )

# Component(s) in the package:
atlas_add_library( VP1TrackingGeometrySystems
                   src/*.c*
                   PUBLIC_HEADERS VP1TrackingGeometrySystems
                   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES GeoModelKernel TrkVolumes VP1Base GL StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel ISF_FatrasDetDescrModel TrkDetDescrInterfaces TrkDetDescrUtils TrkGeometry VP1HEPVis VP1Utils )

