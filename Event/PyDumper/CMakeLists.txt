# $Id: CMakeLists.txt 782292 2016-11-04 08:51:36Z krasznaa $
################################################################################
# Package: PyDumper
################################################################################

# Declare the package name:
atlas_subdir( PyDumper )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers )

# Component(s) in the package:
atlas_add_dictionary( PyDumperDictDict
   PyDumper/PyDumperDict.h
   PyDumper/selection.xml
   LINK_LIBRARIES AthContainers )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( bin/sg-dump.py )

# Aliases:
atlas_add_alias( sg-dump "sg-dump.py" )
