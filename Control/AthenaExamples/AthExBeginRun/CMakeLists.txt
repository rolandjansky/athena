################################################################################
# Package: AthExBeginRun
################################################################################

# Declare the package name:
atlas_subdir( AthExBeginRun )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/DatabaseTest/IOVDbTestConditions
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/RegistrationServices
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( AthExBeginRun
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES IOVDbTestConditions AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel )

# Install files from the package:
atlas_install_joboptions( share/*.py )

