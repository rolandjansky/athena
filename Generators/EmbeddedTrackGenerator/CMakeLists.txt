################################################################################
# Package: EmbeddedTrackGenerator
################################################################################

# Declare the package name:
atlas_subdir( EmbeddedTrackGenerator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/PileUpTools
                          GaudiKernel
                          Generators/GeneratorModules
                          PRIVATE
                          Event/EventInfo
                          Reconstruction/Jet/JetEvent
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( EmbeddedTrackGenerator
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} PileUpToolsLib GaudiKernel GeneratorModulesLib EventInfo JetEvent VxVertex )

# Install files from the package:
atlas_install_headers( EmbeddedTrackGenerator )
atlas_install_joboptions( share/*.py )

