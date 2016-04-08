################################################################################
# Package: TrkRIO_OnTrackCreator
################################################################################

# Declare the package name:
atlas_subdir( TrkRIO_OnTrackCreator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_component( TrkRIO_OnTrackCreator
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel EventPrimitives GaudiKernel TrkParameters TrkToolInterfaces AthenaPoolUtilities AtlasDetDescr Identifier xAODEventInfo InDetIdentifier TrkPrepRawData TrkRIO_OnTrack )

# Install files from the package:
atlas_install_headers( TrkRIO_OnTrackCreator )
atlas_install_joboptions( share/*.py )

