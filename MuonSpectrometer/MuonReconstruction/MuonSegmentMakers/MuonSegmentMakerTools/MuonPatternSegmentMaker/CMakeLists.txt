################################################################################
# Package: MuonPatternSegmentMaker
################################################################################

# Declare the package name:
atlas_subdir( MuonPatternSegmentMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
                          PRIVATE
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
			  MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonEDM_AssociationObjects
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( MuonPatternSegmentMakerLib
                   MuonPatternSegmentMaker/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonPatternSegmentMaker
                   LINK_LIBRARIES GaudiKernel AthenaBaseComps MuonSegmentMakerToolInterfaces MuonRecToolInterfaces MuonPattern MuonPrepRawData MuonIdHelpersLib StoreGateLib MuonSegment Identifier )

# Component(s) in the package:
atlas_add_component( MuonPatternSegmentMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} MuonPatternSegmentMakerLib SGtests MuonReadoutGeometry MuonRIO_OnTrack MuonRecHelperToolsLib MuonEDM_AssociationObjects TrkParameters TrkSegment TrkToolInterfaces )
