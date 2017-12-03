################################################################################
# Package: AFP_DBTools
################################################################################

# Declare the package name:
atlas_subdir( AFP_DBTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel)

# External dependencies:

# Component(s) in the package:
atlas_add_component( AFP_DBTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities GaudiKernel)

# Install files from the package:
atlas_install_headers( AFP_DBTools )
atlas_install_joboptions( share/*.py )

