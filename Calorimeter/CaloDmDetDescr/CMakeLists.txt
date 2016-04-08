################################################################################
# Package: CaloDmDetDescr
################################################################################

# Declare the package name:
atlas_subdir( CaloDmDetDescr )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/StoreGate
                          PRIVATE
                          Control/AthenaKernel
                          GaudiKernel
                          Tools/PathResolver )

# Component(s) in the package:
atlas_add_library( CaloDmDetDescr
                   src/*.cxx
                   PUBLIC_HEADERS CaloDmDetDescr
                   LINK_LIBRARIES CaloIdentifier StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel PathResolver )

# Install files from the package:
atlas_install_runtime( share/DeadMaterialRegionDescription_1.10c.txt )

