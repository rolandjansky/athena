################################################################################
# Package: L1TopoSimulationUtils
################################################################################

# Declare the package name:
atlas_subdir( L1TopoSimulationUtils )

# Trigger dependencies
atlas_depends_on_subdirs(
   PUBLIC
   Trigger/TrigT1/L1Topo/L1TopoEvent )

# Component(s) in the package:
atlas_add_library( L1TopoSimulationUtils
   L1TopoSimulationUtils/*.h Root/*.cxx
   PUBLIC_HEADERS L1TopoSimulationUtils
   LINK_LIBRARIES L1TopoEvent )
