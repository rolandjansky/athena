################################################################################
# Package: MuonCalibExtraTreeAlg
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibExtraTreeAlg )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonCalibExtraTreeAlgLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibExtraTreeAlg
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MuonPattern TrkTrack StoreGateLib SGtests MuonIdHelpersLib MuonRecHelperToolsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent CaloIdentifier CxxUtils AthContainers EventPrimitives xAODMuon xAODTracking MuonCalibEvent MuonCalibEventBase MuonCalibExtraTreeEvent MuonCalibITools MuonCalibIdentifier MuonCalibNtuple MuonReadoutGeometry MuonRIO_OnTrack MuonSegment TileEvent TrkDetElementBase TrkGeometry TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkExInterfaces TrkToolInterfaces TrigConfL1Data TrigT1Interfaces TrigT1Result )

atlas_add_component( MuonCalibExtraTreeAlg
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel MuonPattern TrkTrack CaloEvent CaloIdentifier CxxUtils AthContainers EventPrimitives xAODMuon xAODTracking MuonCalibEvent MuonCalibEventBase MuonCalibExtraTreeEvent MuonCalibITools MuonCalibIdentifier MuonCalibNtuple MuonReadoutGeometry MuonIdHelpersLib MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib TileEvent TrkDetElementBase TrkGeometry TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkExInterfaces TrkToolInterfaces TrigConfL1Data TrigT1Interfaces TrigT1Result MuonCalibExtraTreeAlgLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

