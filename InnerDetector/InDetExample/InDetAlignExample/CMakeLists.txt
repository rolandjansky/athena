################################################################################
# Package: InDetAlignExample
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignExample )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py share/InDetAlignExampleFlags/*.py share/Data/*.txt )
atlas_install_runtime( test/InDetAlignExample_TestConfiguration.xml macros/*.cc html/*.html )
atlas_install_scripts( share/RunIterator_Run2Rel19.py )

