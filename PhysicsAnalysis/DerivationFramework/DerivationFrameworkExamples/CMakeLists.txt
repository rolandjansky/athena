################################################################################
# Package: DerivationFrameworkExamples
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkExamples )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODMuon
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Event/xAOD/xAODTracking )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkExamplesLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkExamples
                   LINK_LIBRARIES AthenaBaseComps xAODMuon GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaKernel xAODTracking )

atlas_add_component( DerivationFrameworkExamples
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODMuon GaudiKernel AthenaKernel xAODTracking DerivationFrameworkExamplesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

