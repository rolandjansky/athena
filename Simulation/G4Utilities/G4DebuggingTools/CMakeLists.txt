################################################################################
# Package: G4DebuggingTools
################################################################################

# Declare the package name:
atlas_subdir( G4DebuggingTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          Event/EventInfo
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/MCTruth
                          Simulation/G4Sim/MCTruthBase
                          Simulation/G4Sim/SimHelpers
                          Simulation/ISF/ISF_Core/ISF_Interfaces )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( TBB )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( G4DebuggingHelperLib
                   src/G4DebuggingHelper.cxx
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} )

atlas_add_component( G4DebuggingTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} GaudiKernel AthenaBaseComps CxxUtils StoreGateLib SGtests EventInfo G4AtlasInterfaces G4AtlasToolsLib MCTruth MCTruthBaseLib SimHelpers ISF_Interfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

