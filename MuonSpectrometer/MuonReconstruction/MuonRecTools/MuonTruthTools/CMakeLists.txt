# $Id: CMakeLists.txt 770063 2016-08-25 12:55:48Z krasznaa $
################################################################################
# Package: MuonTruthTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTruthTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Control/AthenaBaseComps
   DetectorDescription/GeoPrimitives
   DetectorDescription/Identifier
   Event/EventPrimitives
   GaudiKernel
   MuonSpectrometer/MuonIdHelpers
   MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
   MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
   MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
   MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
   MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
   MuonSpectrometer/MuonReconstruction/MuonTrackMakers/MuonTrackMakerUtils
   MuonSpectrometer/MuonSimData
   Simulation/G4Sim/TrackRecord
   Tracking/TrkDetDescr/TrkDetElementBase
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkMeasurementBase
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkPrepRawData
   Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
   Tracking/TrkEvent/TrkRIO_OnTrack
   Tracking/TrkEvent/TrkSegment
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkEvent/TrkTruthData
   Tracking/TrkExtrapolation/TrkExInterfaces
   Tracking/TrkFitter/TrkFitterInterfaces
   Tracking/TrkTruthTracks/TrkTruthTrackInterfaces 
   Generators/AtlasHepMC )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_component( MuonTruthTools
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
   LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps
   GeoPrimitives Identifier EventPrimitives GaudiKernel MuonIdHelpersLib
   MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib
   MuonRecToolInterfaces MuonTrackMakerUtils MuonSimData TrkDetElementBase
   TrkSurfaces TrkMeasurementBase TrkParameters TrkPrepRawData
   TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkTrack TrkTruthData
   TrkExInterfaces TrkFitterInterfaces TrackRecordLib TrkTruthTrackInterfaces )
