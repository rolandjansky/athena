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
                          MagneticField/MagFieldConditions
                          MagneticField/MagFieldElements
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          PRIVATE
                          Control/CxxUtils
                          Event/EventPrimitives
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters
                          Tools/PathResolver )

# External dependencies:
find_package( Eigen )
find_package( ROOT )

atlas_add_library( TrkExSolenoidalIntersectorLib
                   src/SolenoidalIntersector.cxx
                   src/SolenoidParametrization.cxx
                   src/SolenoidParametrizationCondAlg.cxx
                   PUBLIC_HEADERS TrkExSolenoidalIntersector
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel MagFieldElements MagFieldConditions TrkExInterfaces TrkExUtils EventPrimitives TrkSurfaces TrkParameters )

# Component(s) in the package:
atlas_add_component( TrkExSolenoidalIntersector
                     src/components/*.cxx
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} TrkExSolenoidalIntersectorLib )

# Tests in the package:
atlas_add_test( SolenoidParametrization_test
                SOURCES test/SolenoidParametrization_test.cxx
                LINK_LIBRARIES TrkExUtils GaudiKernel TestTools PathResolver MagFieldElements MagFieldConditions
                PROPERTIES TIMEOUT 300
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )


atlas_add_test( SolenoidalIntersector_test
                SOURCES test/SolenoidalIntersector_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES TrkExUtils GaudiKernel TestTools CxxUtils ${ROOT_LIBRARIES} MagFieldElements MagFieldConditions PathResolver TrkExSolenoidalIntersectorLib
                PROPERTIES TIMEOUT 300
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

