
# Declare the package name:
atlas_subdir( GeomACTS )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          DetectorDescription/RegionSelector
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondServices
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SiPropertiesSvc
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
#find_package(ACTFramework REQUIRED)
#find_package(Boost 1.62 REQUIRED COMPONENTS program_options unit_test_framework)

set(ACTFW_INCLUDE_DIRS "/atlas/scratch0/pagessin/dev/install/acts-framework/include")
set(ACTFW_LIBRARY "/atlas/scratch0/pagessin/dev/install/acts-framework/lib64/libACTFramework.so")
add_library(ACTFramework SHARED IMPORTED)
set_property(TARGET ACTFramework PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${ACTFW_INCLUDE_DIRS})
set_property(TARGET ACTFramework PROPERTY IMPORTED_LOCATION ${ACTFW_LIBRARY})


# Component(s) in the package:
atlas_add_component( GeomACTS
                     src/*.cxx
                     src/*.cpp
                     src/obj/*.cpp
                     src/Extrapolation/*.cpp
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${ACTFW_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GeoPrimitives Identifier RegionSelectorLib GaudiKernel InDetCondServices SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry ACTSCore ACTFramework)

# Install files from the package:
atlas_install_headers( GeomACTS )
atlas_install_joboptions( share/*.py )

