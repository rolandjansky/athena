
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

if(NOT ATH_ACTS_BUILD_SUBDIR)
  find_package( Acts REQUIRED COMPONENTS Core Legacy )
endif()

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# Component(s) in the package:
atlas_add_component( GeomACTS
                     src/*.cxx
                     src/*.cpp
                     src/obj/*.cpp
                     src/Extrapolation/*.cpp
                     src/components/*.cxx
                     PUBLIC_HEADERS GeomACTS
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GeoPrimitives Identifier RegionSelectorLib GaudiKernel InDetCondServices SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry MagFieldInterfaces Acts::Core Acts::Legacy)

# Install files from the package:
atlas_install_headers( GeomACTS )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

