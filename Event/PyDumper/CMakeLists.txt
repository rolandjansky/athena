# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PyDumper )

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
