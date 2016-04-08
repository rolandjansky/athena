################################################################################
# Package: RecExAlgs
################################################################################

# Declare the package name:
atlas_subdir( RecExAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/SGTools
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate )

# Component(s) in the package:
atlas_add_component( RecExAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps SGTools GaudiKernel StoreGateLib SGtests )

# Install files from the package:
atlas_install_headers( RecExAlgs )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

