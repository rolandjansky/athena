################################################################################
# Package: InDetAlignmentMonitoringRTT
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignmentMonitoringRTT )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/InDetAlignmentMonitoringRTT_TestConfiguration.xml )

