################################################################################
# Package: MuonTGRecTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTGRecTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthLinks
                          Control/AthContainers
                          Control/StoreGate
                          Event/EventPrimitives
                          GaudiKernel
                          Generators/GeneratorObjects
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonStationIndex
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonTrackMakers/MuonTrackMakerUtils
                          Simulation/G4Sim/TrackRecord
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterUtils
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Event/xAOD/xAODTracking
                          MuonSpectrometer/MuonDetDescr/MuonDetDescrUtils
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonLayerEvent
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonSimData
                          MuonSpectrometer/MuonSimEvent
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack )

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

