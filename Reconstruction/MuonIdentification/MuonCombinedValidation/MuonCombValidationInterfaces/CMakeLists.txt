################################################################################
# Package: MuonCombValidationInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonCombValidationInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
                          Reconstruction/MuonIdentification/MuonSegmentTaggers/MuonSegmentTaggerToolInterfaces
                          Tracking/TrkEvent/TrkParameters )

# Install files from the package:
atlas_install_headers( MuonCombValidationInterfaces )

