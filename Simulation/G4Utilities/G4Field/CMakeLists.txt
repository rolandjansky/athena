################################################################################
# Package: G4Field
################################################################################

# Declare the package name:
atlas_subdir( G4Field )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Simulation/G4Sim/FADS/FadsField
                          Simulation/G4Sim/FADS/FadsXMLParser
                          Tools/PathResolver
                          PRIVATE
                          MagneticField/MagFieldInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# this line failed automatic conversion in cmt2cmake :
# macro_prepend G4FieldDict_shlibflags " -lG4Field "

# Component(s) in the package:
atlas_add_library( G4Field
                   src/*.cxx
                   PUBLIC_HEADERS G4Field
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel FadsField FadsXMLParser PathResolver
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} MagFieldInterfaces )

atlas_add_dictionary( G4FieldDict
                      G4Field/G4FieldDict.h
                      G4Field/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel FadsField FadsXMLParser PathResolver MagFieldInterfaces G4Field )

