################################################################################
# Package: TrkExRungeKuttaIntersector
################################################################################

# Declare the package name:
atlas_subdir( TrkExRungeKuttaIntersector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          MagneticField/MagFieldConditions
                          MagneticField/MagFieldElements
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          PRIVATE
                          Control/CxxUtils
                          AtlasTest/TestTools
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters
                          Tools/PathResolver )

# External dependencies:
find_package( Eigen )
find_package( ROOT )


# Component(s) in the package:
atlas_add_component( TrkExRungeKuttaIntersector
                     src/IntersectorWrapper.cxx
                     src/RungeKuttaIntersector.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives GaudiKernel MagFieldConditions MagFieldElements TrkExInterfaces TrkExUtils TrkDetDescrUtils TrkGeometry TrkSurfaces TrkParameters TrkExInterfaces )

# Install files from the package:
atlas_install_headers( TrkExRungeKuttaIntersector )


atlas_add_test( RungeKuttaIntersector_test
                SOURCES test/RungeKuttaIntersector_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES TrkExUtils GaudiKernel TestTools PathResolver CxxUtils ${ROOT_LIBRARIES} MagFieldElements MagFieldConditions TrkExInterfaces 
                PROPERTIES TIMEOUT 300
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

