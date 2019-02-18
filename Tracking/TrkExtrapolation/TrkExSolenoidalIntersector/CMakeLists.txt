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


atlas_add_test( SolenoidParametrization_test
                SOURCES test/SolenoidParametrization_test.cxx
                LINK_LIBRARIES TrkExUtils GaudiKernel TestTools
                EXTRA_PATTERNS "^AtlasFieldSvc +INFO"
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )


atlas_add_test( SolenoidalIntersector_test
                SOURCES test/SolenoidalIntersector_test.cxx
                LINK_LIBRARIES TrkExUtils GaudiKernel TestTools
                EXTRA_PATTERNS "^AtlasFieldSvc +INFO"
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

