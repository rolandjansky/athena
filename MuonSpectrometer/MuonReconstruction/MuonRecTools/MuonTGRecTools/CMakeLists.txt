################################################################################
# Package: MuonTGRecTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTGRecTools )

# External dependencies:
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( MuonTGRecToolsLib
                   MuonTGRecTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonTGRecTools
                   LINK_LIBRARIES GaudiKernel TrkGeometry EventPrimitives AthenaBaseComps TrkTrack TrkExInterfaces TrkToolInterfaces TrkFitterUtils TrackRecordLib TrkEventPrimitives TrkGeometry TrkPrepRawData MuonTrackMakerUtils MuonIdHelpersLib MuonPrepRawData AthContainers AthenaKernel TrkExInterfaces TrkSurfaces TrkSegment )

# Component(s) in the package:
atlas_add_component( MuonTGRecTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} MuonTGRecToolsLib AthLinks StoreGateLib SGtests GeneratorObjects MuonReadoutGeometry MuonStationIndexLib xAODTracking MuonDetDescrUtils MuonDigitContainer MuonCompetingRIOsOnTrack MuonLayerEvent MuonRIO_OnTrack MuonRecToolInterfaces MuonSimData MuonSimEvent RecoToolInterfaces TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkRIO_OnTrack )

# Install files from the package:
atlas_install_joboptions( share/*.py )

