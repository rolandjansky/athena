################################################################################
# Package: PanTauAnalysis
################################################################################

# Declare the package name:
atlas_subdir( PanTauAnalysis )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTMathLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTGraphicsLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTTableLibs was not recognized in automatic conversion in cmt2cmake

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

