################################################################################
# Package: MuonSegmentTaggerTools
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentTaggerTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerUtils
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Reconstruction/MuonIdentification/MuonSegmentTaggers/MuonSegmentTaggerToolInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_library( MuonSegmentTaggerToolsLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES GaudiKernel StoreGateLib SGtests MuonIdHelpersLib MuonRecHelperToolsLib
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps Identifier EventPrimitives MuonReadoutGeometry MuonRIO_OnTrack MuonSegment MuonSegmentMakerUtils MuonCombinedEvent TrkGeometry TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParameters TrkSegment TrkTrack TrkExInterfaces TrkToolInterfaces MuonSegmentTaggerToolInterfaces )

atlas_add_component( MuonSegmentTaggerTools
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps StoreGateLib SGtests Identifier EventPrimitives MuonReadoutGeometry MuonIdHelpersLib MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonSegmentMakerUtils MuonCombinedEvent TrkGeometry TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParameters TrkSegment TrkTrack TrkExInterfaces TrkToolInterfaces MuonSegmentTaggerToolsLib MuonSegmentTaggerToolInterfaces )

# Install files from the package:
atlas_install_joboptions( share/*.py )

