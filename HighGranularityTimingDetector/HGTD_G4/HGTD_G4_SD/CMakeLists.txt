################################################################################
# Package: HGTD_G4_SD
################################################################################

# Declare the package name:
atlas_subdir( HGTD_G4_SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
        Control/CxxUtils
        Control/StoreGate
        GaudiKernel
        HighGranularityTimingDetector/HGTD_DetDescr/HGTD_Identifier
        HighGranularityTimingDetector/HGTD_DetDescr/HGTD_ReadoutGeometry
        InnerDetector/InDetSimEvent
        Simulation/G4Atlas/G4AtlasTools
        Simulation/G4Sim/MCTruth )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )

# Component(s) in the package:
atlas_add_component( HGTD_G4_SD
                     src/*.h src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEANT4_LIBRARIES} CxxUtils StoreGateLib
                     GaudiKernel HGTD_Identifier HGTD_ReadoutGeometry InDetSimEvent G4AtlasToolsLib MCTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py )
