################################################################################
# Package: MdtDriftCircleOnTrackCreator
################################################################################

# Declare the package name:
atlas_subdir( MdtDriftCircleOnTrackCreator )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_component( MdtDriftCircleOnTrackCreator
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps GaudiKernel MuonRIO_OnTrack MuonRecToolInterfaces TrkEventPrimitives TrkSpaceTimePoint TrkToolInterfaces StoreGateLib SGtests Identifier EventPrimitives MdtCalibData MdtCalibSvcLib MuonCalibEvent MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData TrkDistortedSurfaces TrkSurfaces TrkParameters )

# Install files from the package:
atlas_install_headers( MdtDriftCircleOnTrackCreator )
atlas_install_joboptions( share/*.py )

