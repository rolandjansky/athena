################################################################################
# Package: CaloJiveXML
################################################################################

# Declare the package name:
atlas_subdir( CaloJiveXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileEvent
                          graphics/JiveXML
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          Event/EventContainers
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawEvent
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          TileCalorimeter/TileIdentifier
                          Event/xAOD/xAODCaloEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( CaloJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloIdentifier AthenaBaseComps GaudiKernel LArToolsLib TileConditionsLib TileEvent JiveXMLLib CaloDetDescrLib CaloEvent AthenaKernel Identifier LArIdentifier LArRawEvent TileCalibBlobObjs TileIdentifier xAODCaloEvent)

# Install files from the package:
atlas_install_headers( CaloJiveXML )
atlas_install_joboptions( share/*.py )

