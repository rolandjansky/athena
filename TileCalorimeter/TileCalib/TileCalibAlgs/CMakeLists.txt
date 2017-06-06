################################################################################
# Package: TileCalibAlgs
################################################################################

# Declare the package name:
atlas_subdir( TileCalibAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          GaudiKernel
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileIdentifier
                          Trigger/TrigT1/TrigT1CaloCalibToolInterfaces
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/RegistrationServices
                          Event/ByteStreamCnvSvcBase
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTrigL1Calo                          
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileMonitoring
                          TileCalorimeter/TileRecUtils
                          TileCalorimeter/TileSvc/TileByteStream )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( TileCalibAlgs
                     src/Tile*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} CaloDetDescrLib CaloIdentifier AthenaBaseComps Identifier GaudiKernel TileConditionsLib TileIdentifier TrigT1CaloCalibToolInterfaces CaloEvent AthenaKernel StoreGateLib SGtests AthenaPoolUtilities ByteStreamCnvSvcBaseLib xAODEventInfo xAODTrigL1Calo TileCalibBlobObjs TileEvent TileRecUtilsLib TileByteStreamLib )

# Install files from the package:
atlas_install_headers( TileCalibAlgs )
atlas_install_joboptions( share/*.py )

