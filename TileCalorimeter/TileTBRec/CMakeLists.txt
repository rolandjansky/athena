################################################################################
# Package: TileTBRec
################################################################################

# Declare the package name:
atlas_subdir( TileTBRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileIdentifier
                          TileCalorimeter/TileSvc/TileByteStream
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/StoreGate
                          Event/ByteStreamCnvSvcBase
                          Event/xAOD/xAODEventInfo
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileRecUtils
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TileTBRec
                     src/Tile*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps GaudiKernel TileConditionsLib TileIdentifier TileByteStreamLib CaloEvent CaloIdentifier AthenaKernel StoreGateLib SGtests ByteStreamCnvSvcBaseLib xAODEventInfo TileCalibBlobObjs TileEvent TileRecUtilsLib PathResolver )

# Install files from the package:
atlas_install_headers( TileTBRec )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/TileEtaCTB.txt )

