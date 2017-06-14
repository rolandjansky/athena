################################################################################
# Package: CaloAnaEx
################################################################################

# Declare the package name:
atlas_subdir( CaloAnaEx )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/CaloAnaEx_TestConfiguration.xml share/*.C share/*.css share/HistoComparison.txt share/*Checks.py )

