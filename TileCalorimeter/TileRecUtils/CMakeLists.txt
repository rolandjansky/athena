################################################################################
# Package: TileRecUtils
################################################################################

# Declare the package name:
atlas_subdir( TileRecUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloUtils
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          GaudiKernel
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Control/AthAllocators
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          Event/EventContainers
                          Event/xAOD/xAODEventInfo
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          TileCalorimeter/TileDetDescr
                          Tools/PathResolver
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TileRecUtilsLib
                   src/Tile*.cxx
                   PUBLIC_HEADERS TileRecUtils
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloConditions CaloIdentifier AthenaBaseComps AthenaKernel Identifier GaudiKernel TileEvent TileIdentifier CaloUtilsLib TileConditionsLib CaloDetDescrLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} CaloEvent AthAllocators xAODEventInfo TileCalibBlobObjs TileDetDescr PathResolver )

atlas_add_component( TileRecUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} CaloConditions CaloIdentifier CaloUtilsLib AthenaBaseComps AthenaKernel Identifier GaudiKernel TileConditionsLib TileEvent TileIdentifier CaloDetDescrLib CaloEvent AthAllocators StoreGateLib SGtests xAODEventInfo TileCalibBlobObjs TileDetDescr PathResolver TileRecUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

