################################################################################
# Package: LArG4GenShowerLib
################################################################################

# Declare the package name:
atlas_subdir( LArG4GenShowerLib )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( LArG4GenShowerLibLib
                   src/*.cxx
                   PUBLIC_HEADERS LArG4GenShowerLib
                   PRIVATE_INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthContainers AtlasHepMCLib GaudiKernel G4AtlasToolsLib LArG4Code
                   PRIVATE_LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} GeneratorObjects GeoModelInterfaces LArG4ShowerLib )

atlas_add_component( LArG4GenShowerLib
                     src/components/*.cxx
                     INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES LArG4GenShowerLibLib )

atlas_add_dictionary( LArG4GenShowerLibDict
                      LArG4GenShowerLib/LArG4GenShowerLibDict.h
                      LArG4GenShowerLib/selection.xml
                      INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthContainers GaudiKernel G4AtlasToolsLib GeneratorObjects LArG4Code LArG4ShowerLib LArG4GenShowerLibLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
