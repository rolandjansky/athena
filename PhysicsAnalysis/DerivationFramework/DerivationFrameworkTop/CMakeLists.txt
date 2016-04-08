################################################################################
# Package: DerivationFrameworkTop
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkTop )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PRIVATE
                          Event/xAOD/xAODEventInfo )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkTopLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkTop
                   LINK_LIBRARIES AthenaBaseComps xAODTruth GaudiKernel
                   PRIVATE_LINK_LIBRARIES xAODEventInfo )

atlas_add_component( DerivationFrameworkTop
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTruth GaudiKernel xAODEventInfo DerivationFrameworkTopLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

