################################################################################
# Package: TileCalibAlgs
################################################################################

# Declare the package name:
atlas_subdir( TileCalibAlgs )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( TileCalibAlgs
                     src/Tile*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} CaloDetDescrLib CaloIdentifier AthenaBaseComps Identifier GaudiKernel TileConditionsLib TileIdentifier TrigT1CaloCalibToolInterfaces CaloEvent AthenaKernel StoreGateLib SGtests AthenaPoolUtilities ByteStreamCnvSvcBaseLib xAODEventInfo xAODTrigL1Calo TileCalibBlobObjs TileEvent TileRecUtilsLib TileByteStreamLib TileMonitoringLib RegistrationServicesLib )

# Install files from the package:
atlas_install_headers( TileCalibAlgs )
atlas_install_joboptions( share/*.py )

