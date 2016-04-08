################################################################################
# Package: MuonSegmMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel )

# tag ROOTGraphicsLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( MuonSegmMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel )

# Install files from the package:
atlas_install_headers( MuonSegmMonitoring )
atlas_install_joboptions( share/*.py )

