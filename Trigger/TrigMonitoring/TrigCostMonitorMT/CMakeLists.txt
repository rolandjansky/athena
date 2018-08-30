################################################################################
# Package: TrigCostMonitorMT
################################################################################

# Declare the package name:
atlas_subdir( TrigCostMonitorMT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          TestPolicy )

# Component(s) in the package:
atlas_add_component( TrigCostMonitorMT
                     src/*.cxx
                     src/components/*.cxx 
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps )

# Install files from the package:
atlas_install_headers( TrigCostMonitorMT )
atlas_install_joboptions( share/*.py )
