################################################################################
# Package: TrkCollectionAliasAlg
################################################################################

# Declare the package name:
atlas_subdir( TrkCollectionAliasAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( TrkCollectionAliasAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkTrack )

# Install files from the package:
atlas_install_headers( TrkCollectionAliasAlg )

