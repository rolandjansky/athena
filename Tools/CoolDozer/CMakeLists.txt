################################################################################
# Package: CoolDozer
################################################################################

# Declare the package name:
atlas_subdir( CoolDozer )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/CoolDozer_jobOptions.py share/CoolDozer_fakePerfMon_jobOptions.py )
atlas_install_runtime( test/CoolDozer_TestConfiguration.xml share/RandomDozerAppStandalone.py share/RTTRandomDozerApp.py )
atlas_install_scripts( share/dozerReader.py share/perfMonDozer.py )
atlas_install_xmls( config/CoolDozerConfig.xml )

