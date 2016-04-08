################################################################################
# Package: TrkParametersIdentificationHelpers
################################################################################

# Declare the package name:
atlas_subdir( TrkParametersIdentificationHelpers )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloGeoHelpers
                          DetectorDescription/AtlasDetDescr
                          Tracking/TrkEvent/TrkParameters )

# Component(s) in the package:
atlas_add_library( TrkParametersIdentificationHelpers
                   src/*.cxx
                   PUBLIC_HEADERS TrkParametersIdentificationHelpers
                   LINK_LIBRARIES CaloGeoHelpers AtlasDetDescr TrkParameters )

atlas_add_executable( TestTrkParametersIdentificationHelpers
                      exe/TestTrkParametersIdentificationHelpers.cxx
                      LINK_LIBRARIES CaloGeoHelpers AtlasDetDescr TrkParameters TrkParametersIdentificationHelpers )

