################################################################################
# Package: LArSim
################################################################################

# Declare the package name:
atlas_subdir( LArSim )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          LArCalorimeter/LArSimEvent
                          PRIVATE
                          Calorimeter/CaloIdentifier
                          Control/StoreGate )

# Component(s) in the package:
atlas_add_component( LArSim
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel LArSimEvent CaloIdentifier StoreGateLib SGtests )

# Install files from the package:
atlas_install_headers( LArSim )
atlas_install_joboptions( share/*.txt share/*.py )

