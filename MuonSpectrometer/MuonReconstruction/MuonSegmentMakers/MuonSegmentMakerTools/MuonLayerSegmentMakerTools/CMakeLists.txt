################################################################################
# Package: MuonLayerSegmentMakerTools
################################################################################

# Declare the package name:
atlas_subdir( MuonLayerSegmentMakerTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/EventPrimitives
                          MuonSpectrometer/MuonCnv/MuonPrepRawDataProviderTools
                          MuonSpectrometer/MuonDetDescr/MuonDetDescrUtils
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonPatternFinders/MuonPatternFinderTools/MuonHoughPatternTools
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonLayerEvent
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonRecUtils/MuonLayerHough
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerAlgs/CscSegmentMakers
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
			  Tracking/TrkEvent/TrkSegment )

# Component(s) in the package:
atlas_add_component( MuonLayerSegmentMakerTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps EventPrimitives MuonPrepRawDataProviderToolsLib MuonDetDescrUtils MuonIdHelpersLib MuonHoughPatternToolsLib MuonLayerEvent MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonLayerHough TrkSegment )

