################################################################################
# Package: LArCalibProcessing
################################################################################

# Declare the package name:
atlas_subdir( LArCalibProcessing )

# Necessary external(s):
find_package( cx_Oracle )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/LArCalibProcessing_TestConfiguration.xml )
atlas_install_scripts( share/LAr_updateHVcalib_online.sh share/LAr_updateHVcalibNoise.sh )

