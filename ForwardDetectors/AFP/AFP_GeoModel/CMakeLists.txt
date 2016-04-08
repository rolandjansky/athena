################################################################################
# Package: AFP_GeoModel
################################################################################

# Declare the package name:
atlas_subdir( AFP_GeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          ForwardDetectors/AFP/AFP_Geometry
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( AFP_GeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS AFP_GeoModel
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaKernel SGTools GeoModelKernel GeoModelUtilities AFP_Geometry StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaPoolUtilities GaudiKernel )

atlas_add_component( AFP_GeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel SGTools GeoModelKernel GeoModelUtilities AFP_Geometry StoreGateLib SGtests AthenaPoolUtilities GaudiKernel AFP_GeoModelLib )

