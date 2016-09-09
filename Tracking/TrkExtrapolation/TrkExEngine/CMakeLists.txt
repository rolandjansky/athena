################################################################################
# Package: TrkExEngine
################################################################################

# Declare the package name:
atlas_subdir( TrkExEngine )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          PRIVATE
                          Tracking/TrkDetDescr/TrkDetDescrUtils )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrkExEngine
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel TrkDetDescrInterfaces TrkGeometry TrkSurfaces TrkVolumes TrkEventPrimitives TrkNeutralParameters TrkParameters TrkExInterfaces TrkExUtils TrkDetDescrUtils )

# Install files from the package:
atlas_install_headers( TrkExEngine )
atlas_install_python_modules( python/*.py )

