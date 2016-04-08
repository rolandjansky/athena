################################################################################
# Package: MuonCombinedTestTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedTestTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# Component(s) in the package:
atlas_add_component( MuonCombinedTestTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRIO_OnTrack MuonCombinedToolInterfaces TrkDetDescrInterfaces TrkGeometry TrkSurfaces TrkMeasurementBase TrkParameters TrkTrack TrkExInterfaces )

