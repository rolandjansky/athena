################################################################################
# Package: VP1RawDataSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1RawDataSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          graphics/VP1/VP1Base
                          PRIVATE
                          DetectorDescription/GeoModel/GeoModelKernel
                          Event/EventPrimitives
                          ForwardDetectors/LUCID/LUCID_RawEvent
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetBCM_RawData
                          InnerDetector/InDetRawEvent/InDetRawData
                          graphics/VP1/VP1HEPVis
                          graphics/VP1/VP1Utils )

# External dependencies:
find_package( Coin3D )
find_package( Eigen )

include_directories( /usr/X11R6/include )

# Component(s) in the package:
atlas_add_library( VP1RawDataSystems
                   src/*.c*
                   PUBLIC_HEADERS VP1RawDataSystems
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives VP1Base GL
                   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} GeoModelKernel EventPrimitives LUCID_RawEvent GaudiKernel InDetIdentifier InDetReadoutGeometry InDetBCM_RawData InDetRawData VP1HEPVis VP1Utils )

