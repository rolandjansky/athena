################################################################################
# Package: TRT_Cabling
################################################################################

# Declare the package name:
atlas_subdir( TRT_Cabling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tools/PathResolver )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TRT_Cabling
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} GaudiKernel AthenaBaseComps Identifier InDetIdentifier InDetReadoutGeometry PathResolver )

# Install files from the package:
atlas_install_headers( TRT_Cabling )
atlas_install_runtime( share/*.dat )

