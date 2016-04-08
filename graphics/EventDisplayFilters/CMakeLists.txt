################################################################################
# Package: EventDisplayFilters
################################################################################

# Declare the package name:
atlas_subdir( EventDisplayFilters )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          PRIVATE
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          Tracking/TrkEvent/TrkSpacePoint
                          Trigger/TrigT1/TrigT1Result )

# Component(s) in the package:
atlas_add_component( EventDisplayFilters
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests GaudiKernel TileEvent TileIdentifier TrkSpacePoint TrigT1Result )

# Install files from the package:
atlas_install_headers( EventDisplayFilters )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

