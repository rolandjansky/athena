################################################################################
# Package: TBCaloConditions
################################################################################

# Declare the package name:
atlas_subdir( TBCaloConditions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/CondDBObjects
                          Event/EventInfo )

# Component(s) in the package:
atlas_add_library( TBCaloConditionsLib
                   src/*.cxx
                   PUBLIC_HEADERS TBCaloConditions
                   LINK_LIBRARIES GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities CondDBObjects EventInfo )

atlas_add_component( TBCaloConditions
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities CondDBObjects EventInfo TBCaloConditionsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

