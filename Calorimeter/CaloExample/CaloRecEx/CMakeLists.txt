################################################################################
# Package: CaloRecEx
################################################################################

# Declare the package name:
atlas_subdir( CaloRecEx )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/CaloRecEx_TestConfiguration.xml share/*.C share/*.css share/CaloRecEx_HistoComparison.txt share/*Checks.py )

