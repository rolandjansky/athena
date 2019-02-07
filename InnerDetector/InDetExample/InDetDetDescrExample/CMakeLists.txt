################################################################################
# Package: InDetDetDescrExample
################################################################################

# Declare the package name:
atlas_subdir( InDetDetDescrExample )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          DetectorDescription/RegionSelector
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SiPropertiesSvc
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( InDetDetDescrExample
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GeoPrimitives Identifier RegionSelectorLib GaudiKernel InDetCondTools SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry )

# Install files from the package:
atlas_install_headers( InDetDetDescrExample )
atlas_install_joboptions( share/*.py )

