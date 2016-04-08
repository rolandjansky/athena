################################################################################
# Package: MuonTrkPhysMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MuonTrkPhysMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTMathLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTGraphicsLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTTableLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( MuonTrkPhysMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel )

# Install files from the package:
atlas_install_headers( MuonTrkPhysMonitoring )
atlas_install_joboptions( share/*.py )

