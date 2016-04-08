################################################################################
# Package: TrkExSolenoidalIntersector
################################################################################

# Declare the package name:
atlas_subdir( TrkExSolenoidalIntersector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          PRIVATE
                          Event/EventPrimitives
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrkExSolenoidalIntersector
                     src/SolenoidalIntersector.cxx
                     src/SolenoidParametrization.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel MagFieldInterfaces TrkExInterfaces TrkExUtils EventPrimitives TrkSurfaces TrkParameters )

# Install files from the package:
atlas_install_headers( TrkExSolenoidalIntersector )

