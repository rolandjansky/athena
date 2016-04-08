################################################################################
# Package: TrkCompetingRIOsOnTrackTool
################################################################################

# Declare the package name:
atlas_subdir( TrkCompetingRIOsOnTrackTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# External dependencies:
find_package( ROOT COMPONENTS Gpad Graf )

# Component(s) in the package:
atlas_add_component( TrkCompetingRIOsOnTrackTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TrkParameters TrkToolInterfaces AtlasDetDescr TrkSurfaces TrkCompetingRIOsOnTrack TrkEventUtils TrkPrepRawData TrkRIO_OnTrack )

# Install files from the package:
atlas_install_headers( TrkCompetingRIOsOnTrackTool )

