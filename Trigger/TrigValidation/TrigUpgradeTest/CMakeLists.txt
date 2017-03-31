################################################################################
# Package: TrigUpgradeTest
################################################################################

# Declare the package name:
atlas_subdir( TrigUpgradeTest )

# Declare the package's dependencies:
#atlas_depends_on_subdirs( PUBLIC
#                          TestPolicy
#                          )

atlas_add_test( GraphView1 SCRIPT scripts/GraphView.sh
                ENVIRONMENT THREADS=1     )
atlas_add_test( GraphView2 SCRIPT scripts/GraphView.sh
                ENVIRONMENT THREADS=2     )
atlas_add_test( GraphView64 SCRIPT scripts/GraphView.sh
                ENVIRONMENT THREADS=64              )

atlas_add_test( IDRunMC     SCRIPT scripts/IDRunMC.sh
                )
atlas_add_test( IDRunData     SCRIPT scripts/IDRunData.sh
                )

