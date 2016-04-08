################################################################################
# Package: L1TopoMonitoring
################################################################################

# Declare the package name:
atlas_subdir( L1TopoMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigT1/L1Topo/L1TopoConfig
                          Trigger/TrigT1/L1Topo/L1TopoRDO
                          Trigger/TrigT1/TrigT1Result )

# Component(s) in the package:
atlas_add_component( L1TopoMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel L1TopoConfig L1TopoRDO TrigT1Result )

# Install files from the package:
atlas_install_headers( L1TopoMonitoring )
atlas_install_joboptions( share/*.py )

