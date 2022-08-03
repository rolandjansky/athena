################################################################################
# Package: G4FastSimulation
################################################################################

# Declare the package name:
atlas_subdir( G4FastSimulation )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Possible extra dependencies:
set( extra_lib )
if( NOT SIMULATIONBASE )
   set( extra_lib ISF_FastCaloSimInterfaces ISF_FastCaloSimParametrizationLib FastCaloSimHitLib FastCaloSimLib TruthUtils )
endif()
# FastCaloSim G4VFastSimulationModel should only be used in Athena builds
set ( extra_src )
if( NOT SIMULATIONBASE )
   set( extra_src src/FastCaloSim.cxx  src/FastCaloSimTool.cxx  )
endif()

# Component(s) in the package:
atlas_add_library( G4FastSimulation
                   src/DeadMaterialShower.cxx  src/DeadMaterialShowerTool.cxx  src/SimpleFastKiller.cxx  src/SimpleFastKillerTool.cxx ${extra_src}
                   src/components/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel G4AtlasToolsLib ${extra_lib}
                )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

