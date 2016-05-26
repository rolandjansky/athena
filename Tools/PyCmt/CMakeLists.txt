################################################################################
# Package: PyCmt
################################################################################

# Declare the package name:
atlas_subdir( PyCmt )

# External dependencies:
find_package( PythonLibs )

# this line failed automatic conversion in cmt2cmake :
# cmtpath_pattern_reverse  macro_append includes ' $(ppcmd)"<path>/$(cmt_installarea_prefix)/$(CMTCONFIG)/include" '

# Install files from the package:
atlas_install_python_modules( python/*.py python/pyshell python/pkgbuild )
atlas_install_scripts( bin/make_pkgbuild.py )

