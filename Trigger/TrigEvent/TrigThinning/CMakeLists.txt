################################################################################
# Package: TrigThinning
################################################################################

# Declare the package name:
atlas_subdir( TrigThinning )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODTrigEgamma
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces )

# Component(s) in the package:
atlas_add_component( TrigThinning
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel xAODTrigEgamma GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )

