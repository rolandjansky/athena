################################################################################
# Package: CaloBadChannelTool
################################################################################

# Declare the package name:
atlas_subdir( CaloBadChannelTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate )

# Component(s) in the package:
atlas_add_component( CaloBadChannelTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloConditions CaloIdentifier AthenaBaseComps GaudiKernel StoreGateLib SGtests )

# Install files from the package:
atlas_install_headers( CaloBadChannelTool )

