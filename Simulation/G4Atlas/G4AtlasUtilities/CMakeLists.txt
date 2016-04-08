################################################################################
# Package: G4AtlasUtilities
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasUtilities )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Simulation/G4Sim/FADS/FadsKinematics
                          PRIVATE
                          Control/SGTools
                          Control/StoreGate
                          Event/EventInfo
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Simulation/Tools/AtlasCLHEP_RandomGenerators )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# this line failed automatic conversion in cmt2cmake :
# macro_prepend G4AtlasUtilitiesDict_shlibflags " -lG4AtlasUtilities "

# Component(s) in the package:
atlas_add_library( G4AtlasUtilities
                   src/*.cxx
                   PUBLIC_HEADERS G4AtlasUtilities
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel FadsKinematics ${EIGEN_LIBRARIES} StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools EventInfo GaudiKernel AtlasCLHEP_RandomGenerators )

atlas_add_dictionary( G4AtlasUtilitiesDict
                      G4AtlasUtilities/G4AtlasUtilitiesDict.h
                      G4AtlasUtilities/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel FadsKinematics SGTools StoreGateLib SGtests EventInfo GaudiKernel AtlasCLHEP_RandomGenerators G4AtlasUtilities )

