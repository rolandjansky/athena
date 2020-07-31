################################################################################
# Package: MuonClusterSegmentMakerTools
################################################################################

# Declare the package name:
atlas_subdir( MuonClusterSegmentMakerTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          MuonSpectrometer/MuonCnv/MuonPrepRawDataProviderTools
                          MuonSpectrometer/MuonDetDescr/MuonDetDescrUtils
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonDataPrep/MuonClusterization
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonRecUtils/MuonLinearSegmentMakerUtilities
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerUtils
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTools/TrkToolInterfaces 
                          Generators/AtlasHepMC )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonClusterSegmentMakerTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib GaudiKernel AthenaBaseComps MuonPrepRawDataProviderToolsLib MuonDetDescrUtils MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonLinearSegmentMakerUtilities MuonSegmentMakerUtils TrkParameters TrkTrack TrkTruthData TrkFitterInterfaces TrkToolInterfaces MuonSegmentMakerToolInterfaces )

