################################################################################
# Package: LArHV
################################################################################

# Declare the package name:
atlas_subdir( LArHV )

# Possible extra dependencies:
set( extra_dep )
set( extra_lib )
if( NOT SIMULATIONBASE )
   set( extra_dep LArCalorimeter/LArRecConditions )
   set( extra_lib LArRecConditions )
endif()

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
                          LArCalorimeter/LArIdentifier
                          ${extra_dep} )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArHV
                   src/*.cpp
                   PUBLIC_HEADERS LArHV
		   INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel IOVSvcLib StoreGateLib SGtests LArCablingLib ${extra_lib}
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities Identifier GaudiKernel LArIdentifier )

