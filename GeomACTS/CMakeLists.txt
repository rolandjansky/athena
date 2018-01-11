
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

set(ACTS_COMPONENTS Core MaterialPlugin)
find_package(ACTS REQUIRED COMPONENTS ${ACTS_COMPONENTS})
message(STATUS ${ACTS_INCLUDE_DIRS})
message(STATUS ${ACTS_LIBRARIES})

# Component(s) in the package:
atlas_add_component( GeomACTS
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GeoPrimitives Identifier RegionSelectorLib GaudiKernel InDetCondServices SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry ACTS::ACTSCore)

# Install files from the package:
atlas_install_headers( GeomACTS )
atlas_install_joboptions( share/*.py )

