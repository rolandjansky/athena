################################################################################
# Package: InDetTrigRecExample
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigRecExample )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/**.py )
atlas_install_runtime( test/InDetTrigRecExample_TestConfiguration.xml share/jobOptions_RTT_InDetTrigRecExample.py share/jobOptions_RTT_InDetTrigRecExample_doReadBS.py share/plotHistos.C share/jobOptions_RTT_InDetTrigRecExample_backTracking.py share/InDetTrigRecNtupleCreation.py )
atlas_install_scripts( share/RTT_NoBField_transform.sh )

