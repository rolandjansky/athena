
# Declare the package name:
atlas_subdir( ActsAlignment )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondServices
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Control/AthenaExamples/AthExHive 
                          ActsGeometry )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

if(NOT ATH_ACTS_BUILD_SUBDIR)
  find_package(Acts REQUIRED COMPONENTS Core)
endif()

# Component(s) in the package:
atlas_add_component( ActsAlignment
                     src/*.cxx
                     src/*.cpp
                     src/obj/*.cpp
                     src/Extrapolation/*.cpp
                     src/components/*.cxx
                     PUBLIC_HEADERS ActsAlignment
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib GaudiKernel InDetCondServices InDetIdentifier InDetReadoutGeometry AthExHive ActsGeometryLib Acts::Core)

# Install files from the package:
atlas_install_headers( ActsAlignment )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

