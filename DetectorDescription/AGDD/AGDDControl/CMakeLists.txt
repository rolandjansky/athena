################################################################################
# Package: AGDDControl
################################################################################

# Declare the package name:
atlas_subdir( AGDDControl )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/AGDD/AGDDKernel
                          DetectorDescription/GeoModel/GeoModelKernel
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AGDD/AGDDModel
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( AGDDControl
                   src/*.cxx
                   PUBLIC_HEADERS AGDDControl
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AGDDKernel GeoModelKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AGDDModel GeoModelUtilities GaudiKernel PathResolver )

