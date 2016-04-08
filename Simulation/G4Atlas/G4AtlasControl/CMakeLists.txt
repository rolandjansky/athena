################################################################################
# Package: G4AtlasControl
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasControl )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Simulation/G4Sim/FADS/FadsField
                          PRIVATE
                          Simulation/G4Atlas/G4AtlasAlg
                          Simulation/G4Sim/FADS/FadsKinematics
                          Simulation/G4Sim/FADS/FadsPackageLoader
                          Simulation/G4Sim/FADS/FadsUtilities
                          Simulation/G4Sim/FADS/FadsXMLParser
                          Simulation/G4Sim/MCTruthBase
                          Simulation/G4Utilities/G4Field
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# this line failed automatic conversion in cmt2cmake :
# macro_prepend G4AtlasControlDict_shlibflags " -lG4AtlasControl "

# Component(s) in the package:
atlas_add_library( G4AtlasControl
                   src/*.cxx
                   PUBLIC_HEADERS G4AtlasControl
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GaudiKernel FadsField G4AtlasAlgLib MCTruthBaseLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} FadsKinematics FadsPackageLoader FadsUtilities FadsXMLParser G4Field PathResolver )

atlas_add_dictionary( G4AtlasControlDict
                      G4AtlasControl/G4AtlasControlDict.h
                      G4AtlasControl/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel FadsField G4AtlasAlgLib FadsKinematics FadsPackageLoader FadsUtilities FadsXMLParser MCTruthBaseLib G4Field PathResolver G4AtlasControl )

