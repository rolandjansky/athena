################################################################################
# Package: TrigUpgradeTest
################################################################################

# Declare the package name:
atlas_subdir( TrigUpgradeTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy
                          )
atlas_install_runtime( test/TrigUpgradeTest_TestConfiguration.xml )
