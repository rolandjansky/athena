################################################################################
# Package: InDetMonitoringATN
################################################################################

# Declare the package name:
atlas_subdir( InDetMonitoringATN )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/jobOptions_monitoring.py test/InDetMonitoringATN_Subdetector.sh )

