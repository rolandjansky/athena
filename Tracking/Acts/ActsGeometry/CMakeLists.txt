
# Declare the package name:
atlas_subdir( ActsGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AtenaBaseComps
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
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          DetectorDescription/GeoModel/GeoModelUtilities/
                          ActsInterop )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

if(NOT ATH_ACTS_BUILD_SUBDIR)
  find_package( Acts REQUIRED COMPONENTS Core Legacy MaterialPlugin ProtobufPlugin )
endif()

# Component(s) in the package:

atlas_add_library( ActsGeometryLib
                     src/*.cxx
                     src/*.cpp
                     src/obj/*.cpp
                     src/Extrapolation/*.cpp
                     PUBLIC_HEADERS ActsGeometry
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GeoPrimitives Identifier RegionSelectorLib GaudiKernel InDetCondServices SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry MagFieldInterfaces TrkGeometry GeoModelUtilities ActsInteropLib Acts::Core Acts::Legacy Acts::MaterialPlugin Acts::ProtobufPlugin)

atlas_add_component( ActsGeometry
                     src/*.cxx
                     src/*.cpp
                     src/obj/*.cpp
                     src/Extrapolation/*.cpp
                     src/components/*.cxx
                     PUBLIC_HEADERS ActsGeometry
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GeoPrimitives Identifier RegionSelectorLib GaudiKernel InDetCondServices SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry MagFieldInterfaces TrkGeometry GeoModelUtilities ActsInteropLib ActsAlignmentLib Acts::Core Acts::Legacy Acts::MaterialPlugin Acts::ProtobufPlugin)

# Install files from the package:
atlas_install_headers( ActsGeometry )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

