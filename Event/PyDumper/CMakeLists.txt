# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PyDumper )

# Cling needs to be able to see the headers:
atlas_add_library( PyDumperLib
                   PyDumper/PySTLAdaptor.h
                   INTERFACE
                   PUBLIC_HEADERS PyDumper
                   LINK_LIBRARIES AthContainers )

# Component(s) in the package:
atlas_add_dictionary( PyDumperDict
   PyDumper/PyDumperDict.h
   PyDumper/selection.xml
   LINK_LIBRARIES AthContainers PyDumperLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( bin/sg-dump.py )

# Aliases:
atlas_add_alias( sg-dump "sg-dump.py" )
