################################################################################
# Package: MuonCombinedTestTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedTestTools )

# Component(s) in the package:
atlas_add_component( MuonCombinedTestTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRIO_OnTrack MuonCombinedToolInterfaces TrkDetDescrInterfaces TrkGeometry TrkSurfaces TrkMeasurementBase TrkParameters TrkTrack TrkExInterfaces )

