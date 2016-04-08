################################################################################
# Package: TrkMeasurementUpdator
################################################################################

# Declare the package name:
atlas_subdir( TrkMeasurementUpdator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrkMeasurementUpdator
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives GaudiKernel TrkEventPrimitives TrkParameters TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( TrkMeasurementUpdator )

