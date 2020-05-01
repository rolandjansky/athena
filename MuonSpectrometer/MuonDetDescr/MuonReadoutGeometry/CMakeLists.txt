################################################################################
# Package: MuonReadoutGeometry
################################################################################

# Declare the package name:
atlas_subdir( MuonReadoutGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CxxUtils
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonAlignment/MuonAlignmentData
                          MuonSpectrometer/MuonIdHelpers
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkDistortedSurfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AGDD/AGDDModel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          MuonSpectrometer/MuonDetDescr/MuonAGDDDescription )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( MuonReadoutGeometry
                   src/*.c*
                   PUBLIC_HEADERS MuonReadoutGeometry
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives Identifier GaudiKernel MuonAlignmentData TrkDetElementBase TrkDistortedSurfaces TrkSurfaces MuonIdHelpersLib StoreGateLib GeoModelUtilities CxxUtils
                   PRIVATE_LINK_LIBRARIES AGDDModel MuonAGDDDescription )


# Code in this file makes heavy use of eigen and runs orders of magnitude
# more slowly without optimization.  So force this to be optimized even
# in debug builds.  If you need to debug it you might want to change this.
# Specifying optimization via an attribute on the particular
# function didn't work, because that still didn't allow inlining.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     ${CMAKE_CURRENT_SOURCE_DIR}/src/MdtReadoutElement.cxx
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
     COMPILE_DEFINITIONS "FLATTEN" )
endif()
