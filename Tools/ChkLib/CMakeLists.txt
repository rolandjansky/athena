################################################################################
# Package: ChkLib
################################################################################

# Declare the package name:
atlas_subdir( ChkLib )

# Component(s) in the package:
atlas_add_executable( ChkLib
                      src/ChkLib.cxx )

# Install files from the package:
atlas_install_runtime( share/*.files share/*.symbols )

