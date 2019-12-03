################################################################################
# Package: CaloAnaEx
################################################################################

# Declare the package name:
atlas_subdir( CaloAnaEx )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/CaloAnaEx_TestConfiguration.xml share/*.C share/*.css share/HistoComparison.txt share/*Checks.py )

