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
                          AtlasTest/TestTools
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          Generators/GeneratorObjects
                          Simulation/Interfaces/HepMC_Interfaces
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Tools/PmbCxxUtils
                          Simulation/ISF/ISF_Core/ISF_Algorithms
                          LArCalorimeter/LArSimEvent
                          TileCalorimeter/TileSimEvent
                          MuonSpectrometer/MuonSimEvent
                           )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

find_package( GTest )
find_package( Eigen )
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_component( ZDC_SD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CxxUtils StoreGateLib SGtests ZDC_SimEvent G4AtlasToolsLib )

atlas_add_library( ZDC_SDLib
                   src/*.cxx
                   PUBLIC_HEADERS ZDC_SD
INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CxxUtils StoreGateLib SGtests ZDC_SimEvent G4AtlasToolsLib )

atlas_add_test( ZDC_StripSD_gtest
                SOURCES test/ZDC_StripSD_gtest.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                LINK_LIBRARIES TestTools pthread ZDC_SDLib ${GTEST_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} GaudiKernel CxxUtils StoreGateLib SGtests ZDC_SimEvent G4AtlasToolsLib ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" AtlasDetDescr GeneratorObjects ISF_Event ISF_Interfaces PmbCxxUtils InDetSimEvent LArSimEvent TileSimEvent MuonSimEvent AthenaBaseComps)

# Install files from the package:
atlas_install_python_modules( python/*.py )

