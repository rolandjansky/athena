################################################################################
# Package: AthExHistNtup
################################################################################

# Declare the package name:
atlas_subdir( AthExHistNtup )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Event/EventInfo
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( AthExHistNtup
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps EventInfo GaudiKernel )

# Install files from the package:
atlas_install_headers( AthExHistNtup )
atlas_install_joboptions( share/HistNtupOptions.txt share/HistNtupOptions.py )

