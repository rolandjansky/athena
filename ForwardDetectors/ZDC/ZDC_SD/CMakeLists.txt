################################################################################
# Package: ZDC_SD
################################################################################

# Declare the package name:
atlas_subdir( ZDC_SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/CxxUtils
                          Control/StoreGate
                          ForwardDetectors/ZDC/ZDC_SimEvent
                          Simulation/G4Atlas/G4AtlasTools
                          Tools/AtlasDoxygen )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( ZDC_SD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CxxUtils StoreGateLib SGtests ZDC_SimEvent G4AtlasToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

