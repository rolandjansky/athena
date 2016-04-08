################################################################################
# Package: PionDecayer
################################################################################

# Declare the package name:
atlas_subdir( PionDecayer )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Simulation/G4Sim/FADS/FadsKinematics
                          PRIVATE
                          Simulation/G4Sim/MCTruth )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# this line failed automatic conversion in cmt2cmake :
# macro_prepend PionDecayerDict_shlibflags " -lPionDecayer "

# Component(s) in the package:
atlas_add_component( PionDecayer
                     src/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} FadsKinematics MCTruth )

atlas_add_dictionary( PionDecayerDict
                      PionDecayer/PionDecayerDict.h
                      PionDecayer/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} FadsKinematics MCTruth )

# Install files from the package:
atlas_install_headers( PionDecayer )

