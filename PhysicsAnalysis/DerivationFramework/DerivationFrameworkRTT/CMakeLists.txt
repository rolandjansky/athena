################################################################################
# Package: DerivationFrameworkRTT
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkRTT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel )

# Install files from the package:
atlas_install_headers( DerivationFrameworkRTT )
atlas_install_scripts( share/*.sh )

atlas_install_runtime( test/DerivationFrameworkRTT_TestConfiguration.xml )
