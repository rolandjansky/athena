################################################################################
# Package: TrigFTKToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrigFTKToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData
                          Trigger/TrigEvent/TrigInDetEvent )

# Install files from the package:
atlas_install_headers( TrigFTKToolInterfaces )

