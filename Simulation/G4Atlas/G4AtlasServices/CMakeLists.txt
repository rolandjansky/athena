################################################################################
# Package: G4AtlasServices
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          MagneticField/MagFieldInterfaces
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( TBB )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( G4AtlasServices
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} GaudiKernel AthenaBaseComps CxxUtils G4AtlasInterfaces G4AtlasToolsLib MagFieldInterfaces)

# Install files from the package:
atlas_install_headers( G4AtlasServices )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
