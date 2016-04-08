################################################################################
# Package: TileRec
################################################################################

# Declare the package name:
atlas_subdir( TileRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/Identifier
                          Event/EventContainers
                          Event/xAOD/xAODEventInfo
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          TileCalorimeter/TileDetDescr
                          TileCalorimeter/TileL2Algs
                          TileCalorimeter/TileRecUtils
                          TileCalorimeter/TileSimEvent
                          TileCalorimeter/TileSvc/TileByteStream )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TileRec
                     src/Tile*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TileConditionsLib TileEvent TileIdentifier CaloDetDescrLib CaloEvent CaloIdentifier Identifier xAODEventInfo TileCalibBlobObjs TileDetDescr TileL2AlgsLib TileRecUtilsLib TileSimEvent TileByteStreamLib )

# Install files from the package:
atlas_install_headers( TileRec )
atlas_install_joboptions( share/*.py )

