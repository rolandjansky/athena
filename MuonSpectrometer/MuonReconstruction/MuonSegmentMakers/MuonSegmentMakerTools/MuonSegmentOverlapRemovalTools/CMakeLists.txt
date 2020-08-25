################################################################################
# Package: MuonSegmentOverlapRemovalTools
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentOverlapRemovalTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonDataPrep/MuonClusterization
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentCombiners/MuonSegmentCombinerTools/MuonSegmentCombinerToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerUtils
                          Tracking/TrkEvent/TrkEventPrimitives )

# Component(s) in the package:
atlas_add_component( MuonSegmentOverlapRemovalTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonIdHelpersLib MuonClusterizationLib MuonCompetingRIOsOnTrack MuonPattern MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonSegmentMakerUtils TrkEventPrimitives MuonSegmentCombinerToolInterfaces MuonSegmentMakerToolInterfaces )

