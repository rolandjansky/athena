################################################################################
# Package: LArHV
################################################################################

# Declare the package name:
atlas_subdir( LArHV )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/IOVSvc
                          Control/StoreGate
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArIdentifier )

# External dependencies:
find_package( GeoModel )

# Component(s) in the package:
atlas_add_library( LArHV
                   src/*.cpp
                   PUBLIC_HEADERS LArHV
		   INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaKernel IOVSvcLib StoreGateLib SGtests LArCablingLib
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities Identifier GaudiKernel LArIdentifier )

