################################################################################
# Package: AFP_Geometry
################################################################################

# Declare the package name:
atlas_subdir( AFP_Geometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/GeoModel/GeoModelKernel
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( AFP_Geometry
                   src/*.cxx
                   PUBLIC_HEADERS AFP_Geometry
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} SGTools GeoModelKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel PathResolver )

