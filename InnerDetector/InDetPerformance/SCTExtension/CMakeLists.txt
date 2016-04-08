################################################################################
# Package: SCTExtension
################################################################################

# Declare the package name:
atlas_subdir( SCTExtension )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( SCTExtension
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel xAODTracking InDetPrepRawData InDetRIO_OnTrack TrkMeasurementBase TrkTrack )

# Install files from the package:
atlas_install_headers( SCTExtension )
atlas_install_joboptions( share/*.py )

