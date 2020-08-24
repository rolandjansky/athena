################################################################################
# Package: MuonCurvedSegmentCombiner
################################################################################

# Declare the package name:
atlas_subdir( MuonCurvedSegmentCombiner )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonEDM_AssociationObjects
                          MuonSpectrometer/MuonReconstruction/MuonSegmentCombiners/MuonSegmentCombinerTools/MuonSegmentCombinerToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_component( MuonCurvedSegmentCombiner
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonEDM_AssociationObjects StoreGateLib SGtests Identifier MuonReadoutGeometry MuonIdHelpersLib MuonPattern MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParameters TrkPrepRawData TrkRIO_OnTrack MuonSegmentCombinerToolInterfaces MuonSegmentMakerToolInterfaces )

# Install files from the package:
atlas_install_headers( MuonCurvedSegmentCombiner )

