################################################################################
# Package: InDetTrackClusterAssValidation
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackClusterAssValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTruthData
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PRIVATE
                          GaudiKernel
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( HepMC )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( InDetTrackClusterAssValidation
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps InDetPrepRawData TrkSpacePoint TrkTruthData InDetRecToolInterfaces GaudiKernel TrkRIO_OnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( InDetTrackClusterAssValidation )

