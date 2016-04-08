################################################################################
# Package: GeoModelExamples
################################################################################

# Declare the package name:
atlas_subdir( GeoModelExamples )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/Identifier
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( GeoModelExamplesLib
                   src/*.cxx
                   PUBLIC_HEADERS GeoModelExamples
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES SGTools GeoModelKernel GeoModelUtilities StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} Identifier GaudiKernel )

atlas_add_component( GeoModelExamples
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} SGTools GeoModelKernel GeoModelUtilities StoreGateLib SGtests Identifier GaudiKernel GeoModelExamplesLib )

