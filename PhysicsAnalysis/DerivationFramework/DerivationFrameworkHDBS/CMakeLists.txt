################################################################################
# Package: DerivationFrameworkHDBS
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkHDBS )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PRIVATE
                          Control/AthenaKernel )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkHDBSLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkHDBS
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaKernel )

atlas_add_component( DerivationFrameworkHDBS
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel AthenaKernel DerivationFrameworkHDBSLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

