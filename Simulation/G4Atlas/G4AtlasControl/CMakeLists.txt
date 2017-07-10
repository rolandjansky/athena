################################################################################
# Package: G4AtlasControl
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasControl )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )
find_package( Eigen )

# this line failed automatic conversion in cmt2cmake :
# macro_prepend G4AtlasControlDict_shlibflags " -lG4AtlasControl "

# Component(s) in the package:
atlas_add_library( G4AtlasControl
                   src/*.cxx
                   PUBLIC_HEADERS G4AtlasControl
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} )

atlas_add_dictionary( G4AtlasControlDict
                      G4AtlasControl/G4AtlasControlDict.h
                      G4AtlasControl/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel G4AtlasControl )
