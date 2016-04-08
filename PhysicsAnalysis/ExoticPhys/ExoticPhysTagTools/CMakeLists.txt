################################################################################
# Package: ExoticPhysTagTools
################################################################################

# Declare the package name:
atlas_subdir( ExoticPhysTagTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PhysicsAnalysis/EventTag/TagEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( ExoticPhysTagToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS ExoticPhysTagTools
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities GaudiKernel TagEvent
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} )

atlas_add_component( ExoticPhysTagTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaPoolUtilities GaudiKernel TagEvent ExoticPhysTagToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

