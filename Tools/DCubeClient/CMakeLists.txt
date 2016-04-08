################################################################################
# Package: DCubeClient
################################################################################

# Declare the package name:
atlas_subdir( DCubeClient )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/DCubeClient_jobOptions.py )
atlas_install_runtime( test/DCubeClient_TestConfiguration.xml test/*.C share/DCubeValid.xml )
atlas_install_scripts( python/dcube.py python/dcubeConvert.py )
atlas_install_xmls( share/DCubeValid.xml )

