################################################################################
# Package: MuonSegmentSelectionTools
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentSelectionTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces )

# Component(s) in the package:
atlas_add_component( MuonSegmentSelectionTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonSegmentMakerToolInterfaces )

