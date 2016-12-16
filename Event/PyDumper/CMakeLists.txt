# $Id: CMakeLists.txt 789091 2016-12-10 05:10:19Z ssnyder $
################################################################################
# Package: PyDumper
################################################################################

# Declare the package name:
atlas_subdir( PyDumper )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers )

# Cling needs to be able to see the headers.
atlas_install_headers( PyDumper )

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
