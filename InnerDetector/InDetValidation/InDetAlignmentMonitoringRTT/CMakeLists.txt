################################################################################
# Package: InDetAlignmentMonitoringRTT
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignmentMonitoringRTT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/InDetAlignmentMonitoringRTT_TestConfiguration.xml )

