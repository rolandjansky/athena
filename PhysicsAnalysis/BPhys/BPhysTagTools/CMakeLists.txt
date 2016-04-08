################################################################################
# Package: BPhysTagTools
################################################################################

# Declare the package name:
atlas_subdir( BPhysTagTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PhysicsAnalysis/EventTag/TagEvent )

# Component(s) in the package:
atlas_add_library( BPhysTagToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS BPhysTagTools
                   LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities GaudiKernel TagEvent )

atlas_add_component( BPhysTagTools
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities GaudiKernel TagEvent BPhysTagToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

