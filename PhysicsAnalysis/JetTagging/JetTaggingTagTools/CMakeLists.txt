################################################################################
# Package: JetTaggingTagTools
################################################################################

# Declare the package name:
atlas_subdir( JetTaggingTagTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PhysicsAnalysis/EventTag/TagEvent
                          PRIVATE
                          Reconstruction/Jet/JetEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( JetTaggingTagToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS JetTaggingTagTools
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities GaudiKernel TagEvent StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} JetEvent )

atlas_add_component( JetTaggingTagTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities GaudiKernel TagEvent JetEvent JetTaggingTagToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

