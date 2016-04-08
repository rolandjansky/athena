################################################################################
# Package: IOVDbTestAlg
################################################################################

# Declare the package name:
atlas_subdir( IOVDbTestAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasTest/DatabaseTest/IOVDbTestConditions
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          TestPolicy
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/RegistrationServices
                          Event/EventInfo )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( IOVDbTestAlg
                     src/IOVDbTestAlg.cxx
                     src/IOVDbTestCoolDCS.cxx
                     src/IOVDbTestAlg_entries.cxx
                     src/IOVDbTestAlg_load.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} IOVDbTestConditions AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities EventInfo )

# Install files from the package:
atlas_install_headers( IOVDbTestAlg )
atlas_install_joboptions( share/*.py )

