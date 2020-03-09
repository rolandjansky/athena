################################################################################
# Package: InDetPerformanceMonitoringRTT
################################################################################

# Declare the package name:
atlas_subdir( InDetPerformanceMonitoringRTT )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/InDetPerformanceMonitoringRTT_TestConfiguration.xml )

