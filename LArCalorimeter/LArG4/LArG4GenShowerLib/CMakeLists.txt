################################################################################
# Package: LArG4GenShowerLib
################################################################################

# Declare the package name:
atlas_subdir( LArG4GenShowerLib )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Control/StoreGate
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Atlas/G4AtlasInterfaces
                          LArCalorimeter/LArG4/LArG4Code
                          PRIVATE
                          Control/CxxUtils
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          Event/EventInfo
                          Generators/GeneratorObjects
                          LArCalorimeter/LArG4/LArG4ShowerLib )

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
                   LINK_LIBRARIES AthenaBaseComps DataModel SGTools GaudiKernel G4AtlasToolsLib
                   PRIVATE_LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} EventInfo GeneratorObjects LArG4Code LArG4ShowerLib )

 atlas_add_component( LArG4GenShowerLib
                      src/components/*.cxx
                      INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps DataModel SGTools GaudiKernel G4AtlasToolsLib EventInfo GeneratorObjects LArG4Code LArG4ShowerLib LArG4GenShowerLibLib )

atlas_add_dictionary( LArG4GenShowerLibDict
                      LArG4GenShowerLib/LArG4GenShowerLibDict.h
                      LArG4GenShowerLib/selection.xml
                      INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps DataModel SGTools GaudiKernel G4AtlasToolsLib EventInfo GeneratorObjects LArG4Code LArG4ShowerLib LArG4GenShowerLibLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/*.py )

